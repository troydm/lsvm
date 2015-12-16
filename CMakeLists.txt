cmake_minimum_required (VERSION 2.6)
project (lsvm)

set (LSVM_INC "${PROJECT_SOURCE_DIR}/include")
set (LSVM_SRC "${PROJECT_SOURCE_DIR}/src")

# The version number.
set (LSVM_VERSION_MAJOR 0)
set (LSVM_VERSION_MINOR 1)

# Compiler options
set ( CMAKE_CXX_FLAGS "-Wall -std=c++0x -O3 -g")

# OS detection
set (MAC_OS_X "UNDEF")
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # Mac OS X specific code
    set (MAC_OS_X "MAC_OS_X")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

set (LINUX "UNDEF")
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Linux specific code
    set (LINUX "LINUX")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# configure a header file to pass some of the CMake settings
# to the source code
configure_file ("${LSVM_INC}/config.h.in" "${LSVM_INC}/config.h")

# add the binary tree to the search path for include files
# so that we will find config.h
include_directories("${LSVM_INC}")

 
add_subdirectory ("${LSVM_SRC}")
