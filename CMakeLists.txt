cmake_minimum_required (VERSION 2.6)
project (lsvm)

set (LSVM_INC "${PROJECT_SOURCE_DIR}/include")
set (LSVM_SRC "${PROJECT_SOURCE_DIR}/src")

# The version number.
set (LSVM_VERSION_MAJOR 0)
set (LSVM_VERSION_MINOR 1)

# Compiler options
set ( CMAKE_CXX_FLAGS "-Wall -std=c++0x -O3 -g")

# configure a header file to pass some of the CMake settings
# to the source code
configure_file ("${LSVM_INC}/config.h.in" "${LSVM_INC}/config.h")

# add the binary tree to the search path for include files
# so that we will find config.h
include_directories("${LSVM_INC}")

 
add_subdirectory ("${LSVM_SRC}")
