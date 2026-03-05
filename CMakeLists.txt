cmake_minimum_required(VERSION 3.17)

set(CMAKE_TLS_VERIFY OFF)

# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(test VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_DIR E:/External_library/SFML_DY/lib/cmake/SFML)
set(ZeroMQ_DIR E:/External_library/ZMQ_DY/lib/cmake/ZeroMQ)
set(cppzmq_DIR E:/External_library/ZMQ_DY/share/cmake/cppzmq)

find_package(ZeroMQ REQUIRED)
find_package(cppzmq REQUIRED)
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System Network Audio)

add_executable(server server.cpp)
add_executable(client client.cpp)

target_link_libraries(server PRIVATE
        cppzmq
        SFML::Graphics
        SFML::Audio
        SFML::Window
        SFML::System
        SFML::Network
)

target_link_libraries(client PRIVATE
        cppzmq
        SFML::Graphics
        SFML::Audio
        SFML::Window
        SFML::System
        SFML::Network
)
