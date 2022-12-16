#
# #%L
# %%
# Copyright (C) 2018 BMW Car IT GmbH
# %%
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# #L%
#

# UBSanitizerTarget
# ======================
#
# Sets up an ``ubsan`` target that automates executing tests with
# `UndefinedBehaviorSanitizer <https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html>`_
# (UBSan) - a fast undefined behavior detector for C/C++.
#
# Usage
# -----
#
# Add the following lines to your project's ``CMakeLists.txt``:
#
# .. code-block:: cmake
#
#  if(CMAKE_BUILD_TYPE STREQUAL UBSAN)
#      include(UBSanitizerTarget)
#  endif()
#
# Then execute CMake with:
#
# .. code-block:: sh
#
#  CXX=clang++ cmake -DCMAKE_BUILD_TYPE=UBSAN $SOURCE_DIR
#
# and generate the UBSan report for CTest based tests with:
#
# .. code-block:: sh
#
#  cmake --build . --target ubsan
#
# If necessary CTest parameters can be passed in the ARGS env variable:
#
# .. code-block:: sh
#
#  ARGS="-VV --repeat-until-fail 10" cmake --build . --target ubsan
#
# Configuration
# -------------
#
# This module reads the following configuration variables:
#
# ``UBSAN_OPTIONS``
#  `Run-time flags <https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html>`__
#  for the UndefinedBehaviorSanitizer.
#

if(NOT CMAKE_BUILD_TYPE STREQUAL UBSAN)
    message(FATAL_ERROR "UBSanitizerTarget.cmake requires CMake to be "
                        "called with -DCMAKE_BUILD_TYPE=UBSAN")
endif()

# UBSAN build type
set(_UBSAN_FLAGS "-fsanitize=undefined -fno-omit-frame-pointer")
set(_UBSAN_FLAGS "${_UBSAN_FLAGS} -fno-sanitize-recover=all")
set(_UBSAN_FLAGS "${_UBSAN_FLAGS} -g -O1")
set(CMAKE_CXX_FLAGS_UBSAN ${_UBSAN_FLAGS} CACHE STRING
    "Flags used by the C++ compiler during UBSAN builds." FORCE
)
set(CMAKE_C_FLAGS_UBSAN ${_UBSAN_FLAGS} CACHE STRING
    "Flags used by the C compiler during UBSAN builds." FORCE
)
set(CMAKE_EXE_LINKER_FLAGS_UBSAN ${_UBSAN_FLAGS} CACHE STRING
    "Flags used for linking binaries during UBSAN builds." FORCE
)
set(CMAKE_SHARED_LINKER_FLAGS_UBSAN ${_UBSAN_FLAGS} CACHE STRING
    "Flags used for linking shared libraries during UBSAN builds." FORCE
)
mark_as_advanced(
    CMAKE_CXX_FLAGS_UBSAN CMAKE_C_FLAGS_UBSAN CMAKE_EXE_LINKER_FLAGS_UBSAN
    CMAKE_SHARED_LINKER_FLAGS_UBSAN CMAKE_STATIC_LINKER_FLAGS_UBSAN
)
unset(_UBSAN_FLAGS)

if(CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    find_library(UBSAN_LIBRARY ubsan)

    set(UBSAN_FIND_REQUIRED TRUE)
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(UBSAN
        REQUIRED_VARS UBSAN_LIBRARY
    )
    mark_as_advanced(UBSAN_LIBRARY)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL Clang)
    # TODO: Setup proper check for the ubsan lib (part of compiler-rt-staticdev)
    set(UBSAN_FOUND TRUE)
endif()

# Add ubsan target.
string(CONCAT _ubsan_options
    "print_stacktrace=1,"
    "${UBSAN_OPTIONS}"
)
add_custom_target(ubsan
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}

    # working escaping for make: \${ARGS} \$\${ARGS}
    # working escaping for ninja: \$\${ARGS}
    # No luck with VERBATIM option.
    COMMAND ${CMAKE_COMMAND} -E env
                UBSAN_OPTIONS=${_ubsan_options}
            ${CMAKE_CTEST_COMMAND} --output-on-failure "\$\${ARGS}"

    COMMENT "Generate UBSan report"
    USES_TERMINAL # Ensure ninja outputs to stdout.
)
unset(_ubsan_options)
