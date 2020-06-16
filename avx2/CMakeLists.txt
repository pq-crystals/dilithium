set(SRCS sign.c packing.c polyvec.c poly.c ntt.S invntt.S pointwise.S consts.c rejsample.c reduce.S rounding.c)
set(TEST_SRCS test/test_dilithium.c randombytes.c)
set(SPEED_SRCS test/test_speed.c test/speed_print.c test/cpucycles.c test/cpucycles.h randombytes.c)
set(VECTOR_SRCS test/test_vectors.c rng.c)
set(PQCKAT_SRCS PQCgenKAT_sign.c rng.c)
#intentionally omitting fips202.c -- trust it's in ref...
set(KECCAK_FILES fips202x4.c keccak4x/KeccakP-1600-times4-SIMD256.c)
set(AES_FILES aes256ctr.c)
set(AES_SRCS ${SRCS})

set(LOCAL_CFLAGS -Wall -Wextra -Wpedantic -Wmissing-prototypes -Wredundant-decls -Wshadow -Wpointer-arith -mavx2 -maes -O3 -fomit-frame-pointer -flto)

# First, do libraries:
add_library(dil_keccak_avx2 OBJECT ${KECCAK_FILES}) 
target_include_directories(dil_keccak_avx2 PRIVATE ${PROJECT_SOURCE_DIR}/avx2)
target_compile_options(dil_keccak_avx2  PUBLIC ${LOCAL_CFLAGS})
set(_AVX2_OBJS $<TARGET_OBJECTS:dil_keccak_avx2>)

add_library(dil_aes_avx2 OBJECT ${AES_FILES}) 
target_include_directories(dil_aes_avx2 PRIVATE ${PROJECT_SOURCE_DIR}/avx2)
target_compile_options(dil_aes_avx2  PUBLIC ${LOCAL_CFLAGS})
set(_AVX2_OBJS ${_AVX2_OBJS} $<TARGET_OBJECTS:dil_aes_avx2>)

# Plain
foreach(X RANGE 2 4)
   # Plain lib:
   add_library(dil${X}lib_avx2 OBJECT ${SRCS} symmetric-shake.c)
   target_compile_options(dil${X}lib_avx2 PUBLIC ${LOCAL_CFLAGS} -DDILITHIUM_MODE=${X})
   target_include_directories(dil${X}lib_avx2 PRIVATE ${PROJECT_SOURCE_DIR}/avx2)
   set(_AVX2_OBJS ${_AVX2_OBJS} $<TARGET_OBJECTS:dil${X}lib_avx2>)

   # Plain test:
   add_executable(test_dil${X}_avx2 ${TEST_SRCS})
   target_compile_options(test_dil${X}_avx2 PUBLIC ${LOCAL_CFLAGS} -DDILITHIUM_MODE=${X})
   target_link_libraries(test_dil${X}_avx2 dilithium)
   #add_test(test${X}dil test_dil${X}_avx2)

   # Plain speed
   add_executable(test_speed${X}_avx2 ${SPEED_SRCS})
   target_compile_options(test_speed${X}_avx2 PUBLIC ${LOCAL_CFLAGS} -DDILITHIUM_MODE=${X})
   target_link_libraries(test_speed${X}_avx2 dilithium)
   #add_test(test_speed${X}_avx2 test_speed${X}_avx2)

   # Plain test vectors
   add_executable(test_vectors${X}_avx2 ${VECTOR_SRCS})
   target_link_libraries(test_vectors${X}_avx2 PRIVATE dilithium ${OPENSSL_CRYPTO_LIBRARY})
   target_compile_options(test_vectors${X}_avx2 PUBLIC ${LOCAL_CFLAGS} -DDILITHIUM_MODE=${X})
   if (WIN32)
      add_test(NAME test_vectors${X}_avx2 COMMAND ${CMAKE_COMMAND} -E chdir $<TARGET_FILE_DIR:test_vectors${X}_avx2> $ENV{ComSpec} /c "$<TARGET_FILE_NAME:test_vectors${X}_avx2> > tvecs${X}")
   else()
      add_test(NAME test_vectors${X}_avx2 COMMAND sh -c "$<TARGET_FILE:test_vectors${X}_avx2> > tvecs${X}")
   endif()

   # Plain PQCKATs
   add_executable(PQCgenKAT_sign${X}_avx2 ${PQCKAT_SRCS})
   target_compile_options(PQCgenKAT_sign${X}_avx2 PUBLIC ${LOCAL_CFLAGS} -DDILITHIUM_MODE=${X})
   target_link_libraries(PQCgenKAT_sign${X}_avx2 PRIVATE dilithium ${OPENSSL_CRYPTO_LIBRARY})
   add_test(PQCgenKAT_sign${X}_avx2 PQCgenKAT_sign${X}_avx2)

   # AES lib
   add_library(dil${X}libaes_avx2 OBJECT ${AES_SRCS})
   target_compile_options(dil${X}libaes_avx2 PUBLIC ${LOCAL_CFLAGS} -DDILITHIUM_MODE=${X} -DDILITHIUM_USE_AES)
   target_include_directories(dil${X}libaes_avx2 PRIVATE ${PROJECT_SOURCE_DIR}/avx2)
   set(_AVX2_OBJS ${_AVX2_OBJS} $<TARGET_OBJECTS:dil${X}libaes_avx2>)

   # AES test
   add_executable(test_dil${X}aes_avx2 ${TEST_SRCS})
   target_link_libraries(test_dil${X}aes_avx2 dilithium)
   target_compile_options(test_dil${X}aes_avx2 PUBLIC ${LOCAL_CFLAGS} -DDILITHIUM_MODE=${X} -DDILITHIUM_USE_AES)
   #add_test(test_dil${X}aes_avx2 test_dil${X}aes_avx2)

   # AES speed
   add_executable(test_speed${X}aes_avx2 ${SPEED_SRCS})
   target_link_libraries(test_speed${X}aes_avx2 dilithium)
   target_compile_options(test_speed${X}aes_avx2 PUBLIC ${LOCAL_CFLAGS} -DDILITHIUM_MODE=${X} -DDILITHIUM_USE_AES)
   #add_test(test_speed${X}aes_avx2 test_speed${X}aes_avx2)

   # AES test vectors
   add_executable(test_vectors${X}aes_avx2 ${VECTOR_SRCS})
   target_link_libraries(test_vectors${X}aes_avx2 PRIVATE dilithium ${OPENSSL_CRYPTO_LIBRARY})
   target_compile_options(test_vectors${X}aes_avx2 PUBLIC ${LOCAL_CFLAGS} -DDILITHIUM_MODE=${X} -DDILITHIUM_USE_AES)
   if (WIN32)
      add_test(NAME test_vectors${X}aes_avx2 COMMAND ${CMAKE_COMMAND} -E chdir $<TARGET_FILE_DIR:test_vectors${X}aes_avx2> $ENV{ComSpec} /c "$<TARGET_FILE_NAME:test_vectors${X}aes_avx2> > tvecs${X}aes")
   else()
      add_test(NAME test_vectors${X}aes_avx2 COMMAND sh -c "$<TARGET_FILE:test_vectors${X}aes_avx2> > tvecs${X}aes")
   endif()

   # AES PQCKATs
   add_executable(PQCgenKAT_sign${X}aes_avx2 ${PQCKAT_SRCS})
   target_compile_options(PQCgenKAT_sign${X}aes_avx2 PUBLIC ${LOCAL_CFLAGS} -DDILITHIUM_MODE=${X} -DDILITHIUM_USE_AES)
   target_link_libraries(PQCgenKAT_sign${X}aes_avx2 PRIVATE dilithium ${OPENSSL_CRYPTO_LIBRARY})
   add_test(PQCgenKAT_sign${X}aes_avx2 PQCgenKAT_sign${X}aes_avx2)
endforeach()

add_test(Hashes_test_avx2 sha256sum -c ../../SHA256SUMS)

set(AVX2_OBJS ${_AVX2_OBJS} PARENT_SCOPE)
