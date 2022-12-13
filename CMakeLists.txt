cmake_minimum_required(VERSION 3.10)

project(dilithium LANGUAGES C ASM VERSION 3.1.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

#find_package(OpenSSL REQUIRED)
#include_directories(${OPENSSL_INCLUDE_DIR})

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# CMake modules provided by the project
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)

if(BUILD_TESTING)
    message(STATUS "Testing enabled.")
    enable_testing()
endif()

if(CMAKE_BUILD_TYPE STREQUAL ASAN)
    message(STATUS "Address sanitizing enabled.")
    include(ASanitizerTarget)
endif()

if(CMAKE_BUILD_TYPE STREQUAL UBSAN)
    message(STATUS "Undefined behaviour sanitizing enabled.")
    include(UBSanitizerTarget)
endif()

add_subdirectory(ref)
#add_subdirectory(avx2)
