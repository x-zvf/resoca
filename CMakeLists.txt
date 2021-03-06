cmake_minimum_required(VERSION 3.13)
project(resoca)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER /usr/bin/clang++)

add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

# Fixes for Boost::log
set(CMAKE_EXE_LINKER_FLAGS -static)
set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS FALSE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -g -Wno-c11-extensions -Wno-vla-extension -Wno-zero-length-array")

set(Boost_USE_STATIC_LIBS ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()



#find_library(BOOST 1.7.3 REQUIRED)


file(GLOB server-src
        "src/server/*.cpp"
        "src/server/*.hpp")

file(GLOB shared-src
        "src/shared/*.cpp"
        "src/shared/*.hpp"
        "src/shared/protobuf/cpp/*.cc"
        "src/shared/protobuf/cpp/*.h")

add_executable(resoca-server ${server-src} ${shared-src})
target_link_libraries(resoca-server pthread ${CONAN_LIBS})
