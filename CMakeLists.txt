cmake_minimum_required(VERSION 3.16)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # 导出一个 json

project(test VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_DIR E:/SFML_DY/lib/cmake/SFML)

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System Network Audio)

add_executable(test main.cpp)

target_link_libraries(test SFML::Graphics SFML::Audio SFML::Window SFML::System SFML::Network)

# target_compile_definitions(test PRIVATE SFML_STATIC) # 静态库


