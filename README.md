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
