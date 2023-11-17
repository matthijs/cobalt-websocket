# Minimum required version
cmake_minimum_required (VERSION 3.0)

# Project definition
project(main LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "Using ${CMAKE_CXX_COMPILER} (compiler id: ${CMAKE_CXX_COMPILER_ID})")

# Variables
set(CMAKE_CXX_FLAGS "-g -Wall -stdlib=libc++")
set(BOOST_ROOT "/home/matthijs/boost/boost_1_84_0")

# Find required libraries
find_package(Boost 1.84.0 REQUIRED COMPONENTS cobalt system url)
find_package(Threads)
find_package(OpenSSL REQUIRED)

# Includes
include_directories(${Boost_INCLUDE_DIRS})

# Executable
add_executable(main
	main.cpp)

# standard 20
set_property(TARGET main PROPERTY CXX_STANDARD 20)

# Link
target_link_libraries(main Threads::Threads ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})
