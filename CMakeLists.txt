cmake_minimum_required(VERSION 3.16)
project(resoca)

set(CMAKE_CXX_STANDARD 17)
# Fixes for Boost::log
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lboost_log -lpthread -DBOOST_LOG_DYN_LINK")

find_library(BOOST 1.7.0 REQUIRED)

file(GLOB server-src
        "src/server/*.cpp"
        "src/server/*.hpp")

file(GLOB shared-src
        "src/shared/*.cpp"
        "src/shared/*.hpp")

add_executable(resoca-server ${server-src} ${shared-src})
target_link_libraries(resoca-server ${BOOST_LIBRARIES})