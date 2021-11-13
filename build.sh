cmake -DCMAKE_BUILD_TYPE=Debug -DPYTHON_EXECUTABLE=/usr/bin/python3 -G Ninja -DCMAKE_PREFIX_PATH=$HOME/Qt/5.15.2/gcc_64;$HOME/bin/llvm . -B build
cmake --build build
