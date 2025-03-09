cmake_minimum_required(VERSION 3.10)
project(SC4051-Project-Client)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/include)

# Source directories
file(GLOB SOURCE "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(Client ${SOURCE})
