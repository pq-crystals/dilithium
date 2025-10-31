
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wpedantic")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-prototypes")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wredundant-decls")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wshadow")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wpointer-arith")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fomit-frame-pointer")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -z noexecstack")
set(NISTFLAGS "${NISTFLAGS}" -Wno-unused-result -O3 -fomit-frame-pointer)

set(securitylevel 2 3 5)

# build randombytes
add_library(randombytes SHARED randombytes.c)

# build nist rng
add_library(rng SHARED nistkat/rng.c)
target_link_libraries(rng OpenSSL::Crypto)

set(SRCS
  sign.c
  packing.c
  polyvec.c
  poly.c
  ntt.c
  reduce.c
  rounding.c
  symmetric-shake.c
)

add_library(pqccrystals_fips202_ref SHARED fips202.c)

# build algorithm shared library
foreach(level IN LISTS securitylevel)
  set(name pqccrystals_dilithium${level}_ref)

  add_library(${name} SHARED ${SRCS})
  target_link_libraries(${name} pqccrystals_fips202_ref)
  target_compile_definitions(${name} PUBLIC DILITHIUM_MODE=${level})
endforeach()

# build test_dilithium
foreach(level IN LISTS securitylevel)
  add_executable(test_dilithium${level} test/test_dilithium.c)
  target_link_libraries(test_dilithium${level}
    pqccrystals_fips202_ref
    pqccrystals_dilithium${level}_ref
    randombytes)
endforeach()

# build test_vector
foreach(level IN LISTS securitylevel)
  add_executable(test_vectors${level} test/test_vectors.c)
  target_link_libraries(test_vectors${level}
    pqccrystals_fips202_ref
    pqccrystals_dilithium${level}_ref)
endforeach()

# build test_speed
foreach(level IN LISTS securitylevel)
  add_executable(test_speed${level}
    test/cpucycles.c
    test/speed_print.c
    test/test_speed.c
	)
  target_link_libraries(test_speed${level}
    pqccrystals_fips202_ref
    pqccrystals_dilithium${level}_ref
    randombytes)
endforeach()

# build test_mul
add_executable(test_mul
    test/test_mul.c
)
target_link_libraries(test_mul
    pqccrystals_fips202_ref
    pqccrystals_dilithium2_ref
    randombytes
)

# build nistkat
foreach(level IN LISTS securitylevel)
  add_compile_options(${NISTFLAGS})
  add_executable(PQCgenKAT_sign${level} nistkat/PQCgenKAT_sign.c)
  target_link_libraries(PQCgenKAT_sign${level}
    pqccrystals_fips202_ref
    pqccrystals_dilithium${level}_ref
    rng)
endforeach()
