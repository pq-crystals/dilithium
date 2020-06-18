set(SRCS sign.c packing.c polyvec.c poly.c ntt.c reduce.c rounding.c)
set(TEST_SRCS test/test_dilithium.c randombytes.c)
set(SPEED_SRCS test/test_speed.c test/speed_print.c test/cpucycles.c test/cpucycles.h randombytes.c)
set(VECTOR_SRCS test/test_vectors.c rng.c)
set(PQCKAT_SRCS PQCgenKAT_sign.c rng.c)
set(AES_FILES aes256ctr.c)
set(KECCAK_FILES fips202.c)
set(AES_SRCS ${SRCS} symmetric-aes.c)

if(CMAKE_C_COMPILER_ID MATCHES "Clang")
	add_compile_options(-g)
	add_compile_options(-Wall)
	add_compile_options(-Wextra)
	add_compile_options(-Wpedantic)
	add_compile_options(-Wmissing-prototypes)
	add_compile_options(-Wredundant-decls)
	add_compile_options(-Wshadow)
	add_compile_options(-Wpointer-arith)

elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
	add_compile_options(-Werror)
	add_compile_options(-Wall)
	add_compile_options(-Wextra)
	add_compile_options(-Wpedantic)
	add_compile_options(-Wmissing-prototypes)
	add_compile_options(-Wredundant-decls)
	add_compile_options(-Wshadow)
	add_compile_options(-Wpointer-arith)
	add_compile_options(-O3)

elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
	# Warning C4146 is raised when a unary minus operator is applied to an
	# unsigned type; this has nonetheless been standard and portable for as
	# long as there has been a C standard, and we need it for constant-time
	# computations. Thus, we disable that spurious warning.
	add_compile_options(/wd4146)
endif()


# First, do libraries:
add_library(dil_fips_ref OBJECT ${KECCAK_FILES} ${AES_FILES})
set(_REF_OBJS $<TARGET_OBJECTS:dil_fips_ref>)

# Plain tests
foreach(X RANGE 2 4)
   add_library(dil${X}lib_ref OBJECT ${SRCS} symmetric-shake.c)
   target_compile_options(dil${X}lib_ref PUBLIC -DDILITHIUM_MODE=${X})
   add_executable(test_dil${X}_ref ${TEST_SRCS})
   target_link_libraries(test_dil${X}_ref PUBLIC dilithium)
   #add_test(test_dil${X}_ref test_dil${X}_ref)
   set(_REF_OBJS ${_REF_OBJS} $<TARGET_OBJECTS:dil${X}lib_ref>)

   if(NOT WIN32)
   # Plain speed - not yet supported in Windows: TBD
   add_executable(test_speed${X}_ref ${SPEED_SRCS})
   target_compile_options(test_speed${X}_ref PUBLIC -DDILITHIUM_MODE=${X})
   target_link_libraries(test_speed${X}_ref dilithium)
   #add_test(test_speed${X}_ref test_speed${X}_ref)
   endif()

   # Plain test vectors
   add_executable(test_vectors${X}_ref ${VECTOR_SRCS})
   target_compile_options(test_vectors${X}_ref PUBLIC -DDILITHIUM_MODE=${X})
   target_link_libraries(test_vectors${X}_ref PRIVATE dilithium ${OPENSSL_CRYPTO_LIBRARY})
   if (WIN32) 
      add_test(NAME test_vectors${X}_ref COMMAND ${CMAKE_COMMAND} -E chdir $<TARGET_FILE_DIR:test_vectors${X}_ref> $ENV{ComSpec} /c "$<TARGET_FILE_NAME:test_vectors${X}_ref> | dos2unix > ../tvecs${X}")
   else()
      add_test(NAME test_vectors${X}_ref COMMAND sh -c "$<TARGET_FILE:test_vectors${X}_ref> > tvecs${X}")
   endif()

   # Plain PQCKATs
   add_executable(PQCgenKAT_sign${X}_ref ${PQCKAT_SRCS})
   target_compile_options(PQCgenKAT_sign${X}_ref PUBLIC -DDILITHIUM_MODE=${X})
   target_link_libraries(PQCgenKAT_sign${X}_ref PRIVATE dilithium ${OPENSSL_CRYPTO_LIBRARY})
   if (WIN32) 
      # Necessary cludge to make hashes be ignorant of Windows CRLF file formatting:
      add_test(NAME PQCgenKAT_sign${X}_ref COMMAND ${CMAKE_COMMAND} -E chdir $<TARGET_FILE_DIR:PQCgenKAT_sign${X}_ref> $ENV{ComSpec} /c "$<TARGET_FILE_NAME:PQCgenKAT_sign${X}_ref> && dos2unix -n PQCsignKAT_Dilithium${X}.rsp ../PQCsignKAT_Dilithium${X}.rsp && dos2unix -n PQCsignKAT_Dilithium${X}.req ../PQCsignKAT_Dilithium${X}.req")
   else()
      add_test(PQCgenKAT_sign${X}_ref PQCgenKAT_sign${X}_ref)
   endif()

   # AES tests
   add_library(dil${X}aeslib_ref OBJECT ${AES_SRCS})
   target_compile_options(dil${X}aeslib_ref PUBLIC -DDILITHIUM_MODE=${X} -DDILITHIUM_USE_AES)
   add_executable(test_dil${X}aes_ref ${TEST_SRCS})
   target_compile_options(test_dil${X}aes_ref PUBLIC -DDILITHIUM_MODE=${X} -DDILITHIUM_USE_AES)
   target_link_libraries(test_dil${X}aes_ref dilithium)
   #add_test(test_dil${X}aes_ref test_dil${X}aes_ref)
   set(_REF_OBJS ${_REF_OBJS} $<TARGET_OBJECTS:dil${X}aeslib_ref>)

   if(NOT WIN32)
   # AES speed - not yet supported in Windows: TBD
   add_executable(test_speed${X}aes_ref ${SPEED_SRCS})
   target_compile_options(test_speed${X}aes_ref PUBLIC -DDILITHIUM_MODE=${X} -DDILITHIUM_USE_AES)
   target_link_libraries(test_speed${X}aes_ref dilithium)
   #add_test(test_speed${X}aes_ref test_speed${X}aes_ref)
   endif()

   # AES test vectors
   add_executable(test_vectors${X}aes_ref ${VECTOR_SRCS})
   target_compile_options(test_vectors${X}aes_ref PUBLIC -DDILITHIUM_MODE=${X} -DDILITHIUM_USE_AES)
   target_link_libraries(test_vectors${X}aes_ref PRIVATE dilithium ${OPENSSL_CRYPTO_LIBRARY})
   if (WIN32) 
      add_test(NAME test_vectors${X}aes_ref COMMAND ${CMAKE_COMMAND} -E chdir $<TARGET_FILE_DIR:test_vectors${X}aes_ref> $ENV{ComSpec} /c "$<TARGET_FILE_NAME:test_vectors${X}aes_ref> | dos2unix > ../tvecs${X}aes")
   else()
      add_test(NAME test_vectors${X}aes_ref COMMAND sh -c "$<TARGET_FILE:test_vectors${X}aes_ref> > tvecs${X}aes")
   endif()

   # AES PQCKATs
   add_executable(PQCgenKAT_sign${X}aes_ref ${PQCKAT_SRCS})
   target_compile_options(PQCgenKAT_sign${X}aes_ref PUBLIC -DDILITHIUM_MODE=${X} -DDILITHIUM_USE_AES)
   target_link_libraries(PQCgenKAT_sign${X}aes_ref PRIVATE dilithium ${OPENSSL_CRYPTO_LIBRARY})
   if (WIN32) 
      # Necessary cludge to make hashes be ignorant of Windows CRLF file formatting:
      add_test(NAME PQCgenKAT_sign${X}aes_ref COMMAND ${CMAKE_COMMAND} -E chdir $<TARGET_FILE_DIR:PQCgenKAT_sign${X}aes_ref> $ENV{ComSpec} /c "$<TARGET_FILE_NAME:PQCgenKAT_sign${X}aes_ref> && dos2unix -n PQCsignKAT_Dilithium${X}-AES.rsp ../PQCsignKAT_Dilithium${X}-AES.rsp && dos2unix -n PQCsignKAT_Dilithium${X}-AES.req ../PQCsignKAT_Dilithium${X}-AES.req")
   else()
      add_test(PQCgenKAT_sign${X}aes_ref PQCgenKAT_sign${X}aes_ref)
   endif()
endforeach()

add_test(Hashes_test_ref sha256sum -c ../../SHA256SUMS)

set(REF_OBJS ${_REF_OBJS} PARENT_SCOPE)

