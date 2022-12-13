set(DILITHIUM_SRCS
    sign.c
    packing.c
    polyvec.c
    poly.c
    ntt.c
    reduce.c
    rounding.c
    randombytes.c)
set(DILITHIUM_FIPS202_SRCS ${DILITHIUM_SRCS} symmetric-shake.c)
set(DILITHIUM_AES_SRCS ${DILITHIUM_SRCS} symmetric-aes.c)
set(FIPS202_SRCS fips202.c)
set(AES256CTR_SRCS aes256ctr.c)
set(TEST_DILITHIUM_SRCS test/test_dilithium.c)
set(TEST_VECTORS_SRCS test/test_vectors.c)
set(TEST_SPEED_SRCS test/test_speed.c test/speed_print.c test/cpucycles.c randombytes.c)

set(DILITHIUM_SOVERSION "3.1")
set(DILITHIUM_INSTALL_INCLUDEDIR include/dilithium-${DILITHIUM_SOVERSION})

if(MSVC)
  add_compile_options(/nologo /O2 /W4 /wd4146 /wd4244)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
  add_compile_options(-Wmissing-prototypes -Wredundant-decls -Wshadow -Wpointer-arith)
  add_compile_options(-O3 -fomit-frame-pointer)
endif()

add_library(fips202_ref STATIC ${FIPS202_SRCS})
add_library(aes256ctr_ref STATIC ${AES256CTR_SRCS})

# Dilithium 2
add_library(dilithium2_ref STATIC ${DILITHIUM_FIPS202_SRCS})
add_library(dilithium2aes_ref STATIC ${DILITHIUM_AES_SRCS})
target_compile_definitions(dilithium2_ref PUBLIC DILITHIUM_MODE=2)
target_compile_definitions(dilithium2aes_ref PUBLIC DILITHIUM_MODE=2 DILITHIUM_USE_AES)
target_link_libraries(dilithium2_ref INTERFACE fips202_ref)
target_link_libraries(dilithium2aes_ref INTERFACE fips202_ref aes256ctr_ref)

if(BUILD_TESTING)
  add_executable(test_dilithium2_ref ${TEST_DILITHIUM_SRCS})
  add_executable(test_vectors2_ref ${TEST_VECTORS_SRCS})
  add_executable(test_dilithium2aes_ref ${TEST_DILITHIUM_SRCS})
  add_executable(test_vectors2aes_ref ${TEST_VECTORS_SRCS})

  target_link_libraries(test_dilithium2_ref dilithium2_ref)
  target_link_libraries(test_vectors2_ref dilithium2_ref)
  target_link_libraries(test_dilithium2aes_ref dilithium2aes_ref)
  target_link_libraries(test_vectors2aes_ref dilithium2aes_ref)

  add_test(NAME dilithium2_ref COMMAND test_dilithium2_ref)
  add_test(NAME dilithium2aes_ref COMMAND test_dilithium2aes_ref)
endif()

# Dilithium 3
add_library(dilithium3_ref STATIC ${DILITHIUM_FIPS202_SRCS})
add_library(dilithium3aes_ref STATIC ${DILITHIUM_AES_SRCS})
target_compile_definitions(dilithium3_ref PUBLIC DILITHIUM_MODE=3)
target_compile_definitions(dilithium3aes_ref PUBLIC DILITHIUM_MODE=3 DILITHIUM_USE_AES)
target_link_libraries(dilithium3_ref INTERFACE fips202_ref)
target_link_libraries(dilithium3aes_ref INTERFACE fips202_ref aes256ctr_ref)

if(BUILD_TESTING)
  add_executable(test_dilithium3_ref ${TEST_DILITHIUM_SRCS})
  add_executable(test_vectors3_ref ${TEST_VECTORS_SRCS})
  add_executable(test_dilithium3aes_ref ${TEST_DILITHIUM_SRCS})
  add_executable(test_vectors3aes_ref ${TEST_VECTORS_SRCS})
  target_link_libraries(test_dilithium3_ref dilithium3_ref)
  target_link_libraries(test_vectors3_ref dilithium3_ref)
  target_link_libraries(test_dilithium3aes_ref dilithium3aes_ref)
  target_link_libraries(test_vectors3aes_ref dilithium3aes_ref)
  add_test(NAME dilithium3_ref COMMAND test_dilithium3_ref)
  add_test(NAME dilithium3aes_ref COMMAND test_dilithium3aes_ref)
endif()

# Dilithium 5
add_library(dilithium5_ref STATIC ${DILITHIUM_FIPS202_SRCS})
add_library(dilithium5aes_ref STATIC ${DILITHIUM_AES_SRCS})
target_compile_definitions(dilithium5_ref PUBLIC DILITHIUM_MODE=5)
target_compile_definitions(dilithium5aes_ref PUBLIC DILITHIUM_MODE=5 DILITHIUM_USE_AES)
target_link_libraries(dilithium5_ref INTERFACE fips202_ref)
target_link_libraries(dilithium5aes_ref INTERFACE fips202_ref aes256ctr_ref)


if(BUILD_TESTING)
  add_executable(test_dilithium5_ref ${TEST_DILITHIUM_SRCS})
  add_executable(test_vectors5_ref ${TEST_VECTORS_SRCS})
  add_executable(test_dilithium5aes_ref ${TEST_DILITHIUM_SRCS})
  add_executable(test_vectors5aes_ref ${TEST_VECTORS_SRCS})
  target_link_libraries(test_dilithium5_ref dilithium5_ref)
  target_link_libraries(test_vectors5_ref dilithium5_ref)
  target_link_libraries(test_dilithium5aes_ref dilithium5aes_ref)
  target_link_libraries(test_vectors5aes_ref dilithium5aes_ref)

  add_test(NAME dilithium5_ref COMMAND test_dilithium5_ref)
  add_test(NAME dilithium5aes_ref COMMAND test_dilithium5aes_ref)
endif()

set_target_properties(
  dilithium2_ref dilithium2aes_ref dilithium3_ref dilithium3aes_ref
  dilithium5_ref dilithium5aes_ref PROPERTIES PUBLIC_HEADER api.h)

# Install all static libraries
install(
  TARGETS aes256ctr_ref
          fips202_ref
          dilithium2_ref
          dilithium2aes_ref
          dilithium3_ref
          dilithium3aes_ref
          dilithium5_ref
          dilithium5aes_ref
  EXPORT dilithiumTargets
  ARCHIVE DESTINATION lib/dilithium-${DILITHIUM_SOVERSION}
  INCLUDES
  DESTINATION include/dilithium-${DILITHIUM_SOVERSION}
  PUBLIC_HEADER DESTINATION include/dilithium-${DILITHIUM_SOVERSION})

# Vector Tests
if(BUILD_TESTING)
  if(WIN32)
    add_test(NAME vectors2_ref
             COMMAND PowerShell -Command
                     "$<TARGET_FILE:test_vectors2_ref> | dos2unix > tvecs2")
    add_test(
      NAME vectors2aes_ref
      COMMAND PowerShell -Command
              "$<TARGET_FILE:test_vectors2aes_ref> | dos2unix > tvecs2aes")
    add_test(NAME vectors3_ref
             COMMAND PowerShell -Command
                     "$<TARGET_FILE:test_vectors3_ref> | dos2unix > tvecs3")
    add_test(
      NAME vectors3aes_ref
      COMMAND PowerShell -Command
              "$<TARGET_FILE:test_vectors3aes_ref> | dos2unix > tvecs3aes")
    add_test(NAME vectors5_ref
             COMMAND PowerShell -Command
                     "$<TARGET_FILE:test_vectors5_ref> | dos2unix > tvecs5")
    add_test(
      NAME vectors5aes_ref
      COMMAND PowerShell -Command
              "$<TARGET_FILE:test_vectors5aes_ref> | dos2unix > tvecs5aes")
  else()
    add_test(NAME vectors2_ref
             COMMAND sh -c "\"$<TARGET_FILE:test_vectors2_ref>\" > tvecs2")
    add_test(NAME vectors2aes_ref
             COMMAND sh -c
                     "\"$<TARGET_FILE:test_vectors2aes_ref>\" > tvecs2aes")
    add_test(NAME vectors3_ref
             COMMAND sh -c "\"$<TARGET_FILE:test_vectors3_ref>\" > tvecs3")
    add_test(NAME vectors3aes_ref
             COMMAND sh -c
                     "\"$<TARGET_FILE:test_vectors3aes_ref>\" > tvecs3aes")
    add_test(NAME vectors5_ref
             COMMAND sh -c "\"$<TARGET_FILE:test_vectors5_ref>\" > tvecs5")
    add_test(NAME vectors5aes_ref
             COMMAND sh -c
                     "\"$<TARGET_FILE:test_vectors5aes_ref>\" > tvecs5aes")
  endif()

  add_test(NAME hashes COMMAND sha256sum -c ../../SHA256SUMS)
  set_tests_properties(
    vectors2_ref vectors2aes_ref vectors3_ref vectors3aes_ref vectors5_ref
    vectors5aes_ref PROPERTIES FIXTURES_SETUP createVectorFiles)
  set_tests_properties(hashes PROPERTIES FIXTURES_REQUIRED createVectorFiles)
endif()

if(SHARED)
  # set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--export-all-symbols")

  add_library(libpqcrystals_fips202_ref SHARED $<TARGET_OBJECTS:fips202_ref>)
  add_library(libpqcrystals_aes256ctr_ref SHARED
              $<TARGET_OBJECTS:aes256ctr_ref>)

  add_library(libpqcrystals_dilithium2_ref SHARED
              $<TARGET_OBJECTS:dilithium2_ref>)
  add_library(libpqcrystals_dilithium2aes_ref SHARED
              $<TARGET_OBJECTS:dilithium2aes_ref>)
  add_library(libpqcrystals_dilithium3_ref SHARED
              $<TARGET_OBJECTS:dilithium3_ref>)
  add_library(libpqcrystals_dilithium3aes_ref SHARED
              $<TARGET_OBJECTS:dilithium3aes_ref>)
  add_library(libpqcrystals_dilithium5_ref SHARED
              $<TARGET_OBJECTS:dilithium5_ref>)
  add_library(libpqcrystals_dilithium5aes_ref SHARED
              $<TARGET_OBJECTS:dilithium5aes_ref>)
  install(
  TARGETS libpqcrystals_fips202_ref
     libpqcrystals_aes256ctr_ref
     libpqcrystals_dilithium2_ref
     libpqcrystals_dilithium2aes_ref
     libpqcrystals_dilithium3_ref
     libpqcrystals_dilithium3aes_ref
     libpqcrystals_dilithium5_ref
     libpqcrystals_dilithium5aes_ref
  EXPORT dilithiumTargets
  LIBRARY DESTINATION lib/dilithium-${DILITHIUM_SOVERSION}
  INCLUDES
  DESTINATION include/dilithium-${DILITHIUM_SOVERSION}
  PUBLIC_HEADER DESTINATION include/dilithium-${DILITHIUM_SOVERSION})
endif()

include(CMakePackageConfigHelpers)
configure_package_config_file(
  ${CMAKE_SOURCE_DIR}/Config.cmake.in "dilithiumConfig.cmake"
  INSTALL_DESTINATION lib/cmake/dilithium-${DILITHIUM_SOVERSION})

write_basic_package_version_file(
  "dilithiumConfigVersion.cmake"
  VERSION ${DILITHIUM_SOVERSION}
  COMPATIBILITY SameMajorVersion)

install(
  EXPORT dilithiumTargets
  FILE dilithiumTargets.cmake
  DESTINATION lib/cmake/dilithium-${DILITHIUM_SOVERSION})

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/dilithiumConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/dilithiumConfigVersion.cmake"
        DESTINATION lib/cmake/dilithium-${DILITHIUM_SOVERSION})
