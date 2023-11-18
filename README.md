Make sure boost 1.84.0 (current beta) is installed

After that:
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
./main
```

After hitting Ctrl-C two problems occur (I guess I am doing something wrong):
1. The program doesn't immediately cancel the read on line 111. It cancels when a message is received.

2. I expect that the async_close (line 65) is send and that async_read (line 74) get an error::closed as stated in the documentation beast (https://www.boost.org/doc/libs/1_84_0_beta1/libs/beast/doc/html/beast/ref/boost__beast__websocket__stream/async_close.html)

3. I then get another exception (Bad file descriptor) on line 81, I guess due to the previous exception something fishy is going on.

Possible output:
```bash
session: subscribe
session: reading data
{"event":"info","version":2,"serverId":"1dc36a80-7e83-4ea8-9a52-a5ccce8fd45c","platform":{"status":1}}
{"event":"subscribed","channel":"ticker","chanId":71182,"symbol":"tBTCUSD","pair":"BTCUSD"}
[71182,[36696,11.74322478,36697,11.78525912,660,0.01831349,36699,628.63809433,36850,35971]]
^C[71182,[36699,8.89659158,36700,12.07675518,660,0.01831349,36699,628.63809433,36850,35971]]
disconnect: sending close frame
disconnect: waiting on error
disconnect: async_read: exception: Operation canceled [system:125 at /home/matthijs/boost/boost_1_84_0/boost/beast/websocket/impl/stream_impl.hpp:355:68 in function 'bool boost::beast::websocket::stream<boost::asio::ssl::stream<boost::asio::basic_stream_socket<boost::asio::ip::tcp, boost::cobalt::use_op_t::executor_with_default<boost::asio::any_io_executor>>>>::impl_type::check_stop_now(error_code &) [NextLayer = boost::asio::ssl::stream<boost::asio::basic_stream_socket<boost::asio::ip::tcp, boost::cobalt::use_op_t::executor_with_default<boost::asio::any_io_executor>>>, deflateSupported = true]']
disconnect: ssl: exception: Bad file descriptor [system:9 at /home/matthijs/boost/boost_1_84_0/boost/asio/detail/reactive_socket_recv_op.hpp:170:37 in function 'static void boost::asio::detail::reactive_socket_recv_op<boost::asio::mutable_buffers_1, boost::asio::ssl::detail::io_op<boost::asio::basic_stream_socket<boost::asio::ip::tcp, boost::cobalt::use_op_t::executor_with_default<boost::asio::any_io_executor>>, boost::asio::ssl::detail::shutdown_op, boost::cobalt::completion_handler<boost::system::error_code>>, boost::cobalt::use_op_t::executor_with_default<boost::asio::any_io_executor>>::do_immediate(operation *, bool, const void *) [MutableBufferSequence = boost::asio::mutable_buffers_1, Handler = boost::asio::ssl::detail::io_op<boost::asio::basic_stream_socket<boost::asio::ip::tcp, boost::cobalt::use_op_t::executor_with_default<boost::asio::any_io_executor>>, boost::asio::ssl::detail::shutdown_op, boost::cobalt::completion_handler<boost::system::error_code>>, IoExecutor = boost::cobalt::use_op_t::executor_with_default<boost::asio::any_io_executor>]']
disconnect: disable sending / receiving: Bad file descriptor
disconnect: close socket: Success
disconnect: connection closed succesfully
```
