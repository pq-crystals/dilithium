cmake_minimum_required (VERSION 3.5)
# option() honors normal variables.
# see: https://cmake.org/cmake/help/git-stage/policy/CMP0077.html
if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif()
# Honor symbol visibility properties for all target types.
# see: https://cmake.org/cmake/help/git-stage/policy/CMP0063.html
if(POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW)
endif()


project(dilithium C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
    set(CMAKE_GENERATOR_CC cl)
else()
include(FindUnixCommands)
endif()

# find openssl installation for test vector generation
if(NOT DEFINED OPENSSL_ROOT_DIR)
        if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin")
            set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl@1.1")
        elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
            set(OPENSSL_ROOT_DIR "/usr")
        endif()
endif()
find_package(OpenSSL 1.1.1 REQUIRED)

enable_testing()

add_subdirectory(ref)
add_subdirectory(avx2)

add_library(dilithium ${REF_OBJS} ${AVX2_OBJS})

set(PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/ref/sign.h)

set_target_properties(dilithium
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    VERSION 0.1.0
    SOVERSION 0
    # For Windows DLLs
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

install(TARGETS dilithium
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(FILES ${PUBLIC_HEADERS}
        DESTINATION include/dilithium)

