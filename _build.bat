mkdir temp_src
cd temp_src

:: SFML
git clone https://github.com/SFML/SFML.git
cd SFML
rmdir /s /q build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=TRUE -DSFML_USE_SYSTEM_DEPS=FALSE -DCMAKE_TLS_VERIFY=0
cmake --build .
cmake --install . --prefix "E:/External_library/SFML_DY"

:: libzmq
cd ..
cd ..
git clone https://github.com/zeromq/libzmq
cd libzmq
rmdir /s /q build
mkdir build
cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release -DPOLLER=epoll
cmake --build . --config Release
cmake --install . --prefix "E:/External_library/ZMQ_DY"



:: cppzmq
cd ..
cd ..
git clone https://github.com/zeromq/cppzmq
cd cppzmq
rmdir /s /q build
mkdir build
cd build

cmake .. -G Ninja -DCMAKE_PREFIX_PATH="E:/External_library/ZMQ_DY" -DCMAKE_INSTALL_PREFIX="E:/External_library/ZMQ_DY"
cmake --install .



cd ..
cd ..
cd ..

rmdir /s /q temp_src

pause


