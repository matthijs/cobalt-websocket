// websocket connection with correct teardown
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/cobalt.hpp>
#include <boost/url.hpp>
#include <exception>

namespace cobalt = boost::cobalt;

using executor_type = cobalt::use_op_t::executor_with_default<cobalt::executor>;
using socket_type = typename boost::asio::ip::tcp::socket::rebind_executor<
    executor_type>::other;
using ssl_socket_type = boost::asio::ssl::stream<socket_type>;
using websocket_type = boost::beast::websocket::stream<ssl_socket_type>;

cobalt::promise<socket_type> connect_tcp(std::string_view host,
                                         std::string_view port) {
  try {
    // Resolve possible endpoints
    boost::asio::ip::tcp::resolver resolve{cobalt::this_thread::get_executor()};
    auto endpoints = co_await resolve.async_resolve(host, port, cobalt::use_op);

    // Connect to one of the endpoints
    socket_type sock{cobalt::this_thread::get_executor()};
    co_await boost::asio::async_connect(sock, endpoints);
    co_return sock;
  } catch (const std::exception &e) {
    std::printf("connect_tcp: exception: %s\n", e.what());
    throw;
  }
}

cobalt::promise<ssl_socket_type> connect(std::string_view host,
                                         std::string_view port,
                                         boost::asio::ssl::context &ctx) {
  try {
    // Resolve possible endpoints
    boost::asio::ip::tcp::resolver resolve{cobalt::this_thread::get_executor()};
    auto endpoints = co_await resolve.async_resolve(host, port, cobalt::use_op);

    // Connect to one of the endpoints
    ssl_socket_type sock{cobalt::this_thread::get_executor(), ctx};
    co_await boost::asio::async_connect(sock.lowest_layer(), endpoints);

    // Handshake
    co_await sock.async_handshake(boost::asio::ssl::stream_base::client);

    // Return the connected socket
    co_return sock;
  } catch (const std::exception &e) {
    std::printf("connect: exception: %s\n", e.what());
    throw;
  }
}

// connect/disconnect websocket
cobalt::promise<websocket_type> connect_ws(boost::urls::url_view uri,
                                           boost::asio::ssl::context &ctx) {
  try {
    // Create a websocket stream and connect to the endpoint
    websocket_type ws{co_await connect(uri.host(), uri.port(), ctx)};

    // Websocket handshake
    co_await ws.async_handshake(uri.host(), uri.encoded_target());

    // Return the websocket
    co_return ws;
  } catch (const std::exception &e) {
    std::printf("connect_ws: exception: %s\n", e.what());
    throw;
  }
}

cobalt::promise<void> disconnect_ws(websocket_type ws, std::exception_ptr &ep) {

  // Check if there is an exception
  if (ep) {
    std::printf("disconnect: there is an exception...\n");
  }

  // Reset the exception ptr
  ep = nullptr;

  // Send close frame
  std::printf("disconnect: sending close frame\n");
  try {
    co_await ws.async_close(boost::beast::websocket::close_code::none);
  } catch (const std::exception &e) {
    std::printf("disconnect: async_close: exception: %s\n", e.what());
  }

  // Wait for error
  std::printf("disconnect: waiting on error\n");
  try {
    boost::beast::flat_buffer buf;
    co_await ws.async_read(buf);
  } catch (const std::exception &e) {
    std::printf("disconnect: async_read: exception: %s\n", e.what());
  }

  // Now disconnect SSL on this stream
  try {
    co_await ws.next_layer().async_shutdown();
  } catch (const std::exception &e) {
    std::printf("disconnect: ssl: exception: %s\n", e.what());
  }

  // Disable sending / receiving
  boost::system::error_code ec_shutdown;
  ws.next_layer().next_layer().shutdown(
      boost::asio::ip::tcp::socket::shutdown_both, ec_shutdown);
  std::printf("disconnect: disable sending / receiving: %s\n",
              ec_shutdown.message().c_str());

  // Close the socket
  boost::system::error_code ec_close;
  ws.next_layer().next_layer().close(ec_close);
  std::printf("disconnect: close socket: %s\n", ec_close.message().c_str());

  std::printf("disconnect: connection closed succesfully\n");
  co_return;
}

cobalt::promise<void> session(websocket_type &ws) {
  try {
    std::printf("session: subscribe\n");
    std::string request =
        R"({"event":"subscribe","channel":"ticker","symbol":"tBTCUSD"})";
    co_await ws.async_write(boost::asio::buffer(request));

    std::printf("session: reading data\n");
    boost::beast::flat_buffer buf;
    while (!co_await cobalt::this_coro::cancelled && ws.is_open()) {
      auto size = co_await ws.async_read(buf);
      std::string s{static_cast<const char *>(buf.cdata().data()), size};
      std::printf("%s\n", s.c_str());
      buf.consume(size);
    }
    co_return;
  } catch (const std::exception &e) {
    std::printf("session: exception: %s\n", e.what());
    throw;
  }
}

cobalt::main co_main(int argc, char **argv) {
  try {
    while (!co_await cobalt::this_coro::cancelled) {
      boost::asio::ssl::context ctx{boost::asio::ssl::context::tls_client};
      std::string endpoint = "wss://api-pub.bitfinex.com:443/ws/2";
      boost::urls::url uri = boost::urls::parse_uri(endpoint).value();
      co_await cobalt::with(co_await connect_ws(uri, ctx), &session,
                            &disconnect_ws);
    }
  } catch (const std::exception &e) {
    std::printf("main: exception: %s\n", e.what());
  }

  co_return 0;
}
