# Minimum required version
cmake_minimum_required (VERSION 3.0)

# Project definition
project(main LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "Using ${CMAKE_CXX_COMPILER} (compiler id: ${CMAKE_CXX_COMPILER_ID})")

# Include various cmake stuff for c++
include(cmake/compiler-options.cmake)
include(cmake/c++-standards.cmake)
include(cmake/code-coverage.cmake)
include(cmake/sanitizers.cmake)
include(cmake/tools.cmake)
include(cmake/link-time-optimization.cmake)

# Default to std=c++20
cxx_20()

# Variables
set(CMAKE_CXX_FLAGS "-g -Wall -stdlib=libc++")
set(BOOST_ROOT "/home/matthijs/boost/boost_1_84_0")

# Find required libraries
find_package(Boost 1.84.0 REQUIRED COMPONENTS cobalt system url)
find_package(Threads)
find_package(OpenSSL REQUIRED)
find_package(fmt REQUIRED)

# Includes
include_directories(${Boost_INCLUDE_DIRS})

# Executable
add_executable(main
	main.cpp)

# Link
target_link_libraries(main Threads::Threads fmt::fmt ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})
