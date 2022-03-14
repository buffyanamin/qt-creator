# download pre-built libclang from https://download.qt.io/development_releases/prebuilt/libclang/
cmake -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=/usr/bin/python3 -G Ninja -DCMAKE_PREFIX_PATH=/home/bill/Qt/5.15.2/gcc_64:/home/bill/ws_data/libclang/lib/cmake . -B build
#-DClang_DIR=/home/bill/ws_data/libclang/ 
cmake --build build
