cmake_minimum_required(VERSION 3.17)

set(CMAKE_TLS_VERIFY OFF)

# 导出一个 json 供 VSCode / CLion 等提供代码提示补全
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(test VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 引入自动下载模块
include(FetchContent)

# ==========================================================
# 1. 配置 ZeroMQ (libzmq - C语言底层库)
# ==========================================================
find_package(ZeroMQ QUIET) # QUIET 表示找不到别报错
if(NOT ZeroMQ_FOUND)
    message(STATUS "本地未找到 ZeroMQ，正在从 GitHub 自动下载...")
    # 关闭 ZMQ 自己的测试和额外工具，加快我们项目的编译速度
    set(ZMQ_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(WITH_PERF_TOOL OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
            ZeroMQ
            GIT_REPOSITORY https://github.com/zeromq/libzmq.git
            # GIT_TAG        v4.3.5 # 建议使用稳定版本
    )
    FetchContent_MakeAvailable(ZeroMQ)
endif()

# ==========================================================
# 2. 配置 cppzmq (C++ 包装层，依赖于上面下载的 libzmq)
# ==========================================================
find_package(cppzmq QUIET)
if(NOT cppzmq_FOUND)
    message(STATUS "本地未找到 cppzmq，正在从 GitHub 自动下载...")
    set(CPPZMQ_BUILD_TESTS OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
            cppzmq
            GIT_REPOSITORY https://github.com/zeromq/cppzmq.git
            # GIT_TAG        v4.10.0
    )
    FetchContent_MakeAvailable(cppzmq)
endif()

# ==========================================================
# 3. 配置 SFML 3
# ==========================================================
find_package(SFML 3 QUIET COMPONENTS Graphics Window System Network Audio)
if(NOT SFML_FOUND)
    message(STATUS "本地未找到 SFML 3，正在从 GitHub 自动下载...")
    # 关闭 SFML 的自带示例和文档生成
    set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
            SFML
            GIT_REPOSITORY https://github.com/SFML/SFML.git
            # GIT_TAG        3.0.0 # SFML 3.x 分支
    )
    FetchContent_MakeAvailable(SFML)
endif()

# ==========================================================
# 4. 编译我们的主程序并链接
# ==========================================================
add_executable(test main.cpp)

# 链接库 (推荐使用 PRIVATE 限定符，这是现代 CMake 的最佳实践)
target_link_libraries(test PRIVATE
        cppzmq
        SFML::Graphics
        SFML::Audio
        SFML::Window
        SFML::System
        SFML::Network
)

# 【关于静态库的一点说明】
# 当你使用 FetchContent 自动下载源码并与你的项目一起编译时，
# CMake 默认会把这些依赖编译成【静态库】并直接塞进你的 exe 里。
# 好处是：你不需要再去管 dll 动态库放在哪里的问题了，直接运行 exe 即可！
# 如果代码报错需要定义宏，你可以把下面这句取消注释：
# target_compile_definitions(test PRIVATE SFML_STATIC)