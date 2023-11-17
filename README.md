Make sure boost 1.84.0 (current beta) is installed

After that:
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
./main
```
