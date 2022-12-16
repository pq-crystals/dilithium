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

# AddressSanitizerTarget
# ======================
#
# Sets up an ``asan`` target that automates executing tests with
# `AddressSanitizer <https://github.com/google/sanitizers/wiki/AddressSanitizer>`_
# (ASan) - a memory error detector for C/C++.
#
# Usage
# -----
#
# Add the following lines to your project's ``CMakeLists.txt``:
#
# .. code-block:: cmake
#
#  if(CMAKE_BUILD_TYPE STREQUAL ASAN)
#      include(AddressSanitizerTarget)
#  endif()
#
# Then execute CMake with:
#
# .. code-block:: sh
#
#  CXX=clang++ cmake -DCMAKE_BUILD_TYPE=ASAN $SOURCE_DIR
#
# and generate the ASan report for CTest based tests with:
#
# .. code-block:: sh
#
#  cmake --build . --target asan
#
# If necessary CTest parameters can be passed in the ARGS env variable:
#
# .. code-block:: sh
#
#  ARGS="-VV --repeat-until-fail 10" cmake --build . --target asan
#
# Configuration
# -------------
#
# This module reads the following configuration variables:
#
# ``ASAN_OPTIONS``
#  `Run-time flags <https://github.com/google/sanitizers/wiki/AddressSanitizerFlags#run-time-flags>`__
#  for the AddressSanitizer.
#
# ``LSAN_OPTIONS``
#  `Run-time flags <https://github.com/google/sanitizers/wiki/AddressSanitizerLeakSanitizer#flags>`__
#  for the LeakSanitizer (LSan).
#

if(NOT CMAKE_BUILD_TYPE STREQUAL ASAN)
    message(FATAL_ERROR "AddressSanitizerTarget.cmake requires CMake to be "
                        "called with -DCMAKE_BUILD_TYPE=ASAN")
endif()

# ASAN build type
set(_ASAN_FLAGS "-fsanitize=address -fno-omit-frame-pointer")
set(_ASAN_FLAGS "${_ASAN_FLAGS} -fsanitize-address-use-after-scope")
set(_ASAN_FLAGS "${_ASAN_FLAGS} -g -O1")
set(CMAKE_CXX_FLAGS_ASAN ${_ASAN_FLAGS} CACHE STRING
    "Flags used by the C++ compiler during ASAN builds." FORCE
)
set(CMAKE_C_FLAGS_ASAN ${_ASAN_FLAGS} CACHE STRING
    "Flags used by the C compiler during ASAN builds." FORCE
)
set(CMAKE_EXE_LINKER_FLAGS_ASAN ${_ASAN_FLAGS} CACHE STRING
    "Flags used for linking binaries during ASAN builds." FORCE
)
set(CMAKE_SHARED_LINKER_FLAGS_ASAN ${_ASAN_FLAGS} CACHE STRING
    "Flags used for linking shared libraries during ASAN builds." FORCE
)
mark_as_advanced(
    CMAKE_CXX_FLAGS_ASAN CMAKE_C_FLAGS_ASAN CMAKE_EXE_LINKER_FLAGS_ASAN
    CMAKE_SHARED_LINKER_FLAGS_ASAN CMAKE_STATIC_LINKER_FLAGS_ASAN
)
unset(_ASAN_FLAGS)

if(CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    find_library(ASAN_LIBRARY NAMES asan libasan.so.6 libasan.so.5)

    set(ASAN_FIND_REQUIRED TRUE)
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(ASAN
        REQUIRED_VARS ASAN_LIBRARY
    )
    mark_as_advanced(ASAN_LIBRARY)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL Clang)
    # TODO: Setup proper check for the asan lib (part of compiler-rt-staticdev)
    set(ASAN_FOUND TRUE)
endif()

# Add asan target.
string(CONCAT _asan_options
    "detect_odr_violation=0,max_free_fill_size=65535,"
    "detect_stack_use_after_return=true,"
    "${ASAN_OPTIONS}"
)
add_custom_target(asan
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}

    # working escaping for make: \${ARGS} \$\${ARGS}
    # working escaping for ninja: \$\${ARGS}
    # No luck with VERBATIM option.
    COMMAND ${CMAKE_COMMAND} -E env
                ASAN_OPTIONS=${_asan_options} LSAN_OPTIONS=${LSAN_OPTIONS}
            ${CMAKE_CTEST_COMMAND} --output-on-failure "\$\${ARGS}"

    COMMENT "Generate ASan report"
    USES_TERMINAL # Ensure ninja outputs to stdout.
)
unset(_asan_options)