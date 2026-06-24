
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wpedantic")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-prototypes")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wredundant-decls")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wshadow")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wpointer-arith")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mavx2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mpopcnt")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mtune=native")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")

set(securitylevel 2 3 5)

# build fips202 shared library
add_library(pqccrystals_fips202x4_avx2 SHARED fips202x4.c f1600x4.S fips202.c)

set(SRCS
  sign.c
  packing.c
  polyvec.c
  poly.c
  ntt.S
  invntt.S
  pointwise.S
  shuffle.S
  consts.c
  rejsample.c
  rounding.c
  symmetric-shake.c
)

# build algorithm shared library
foreach(level IN LISTS securitylevel)
  set(name pqccrystals_dilithium${level}_avx2)

  add_library(${name} SHARED ${SRCS})
  target_link_libraries(${name} pqccrystals_fips202x4_avx2)
  target_compile_definitions(${name} PUBLIC DILITHIUM_MODE=${level})
  target_include_directories(${name} PRIVATE .)
endforeach()

# build test_dilithium
foreach(level IN LISTS securitylevel)
  add_executable(test_dilithium${level}_avx2 test/test_dilithium.c)
  target_link_libraries(test_dilithium${level}_avx2
    pqccrystals_fips202x4_avx2
    pqccrystals_dilithium${level}_avx2
    randombytes
  )
endforeach()

# build test_vector
foreach(level IN LISTS securitylevel)
  add_executable(test_vectors${level}_avx2 test/test_vectors.c)
  target_link_libraries(test_vectors${level}_avx2
    pqccrystals_fips202x4_avx2
    pqccrystals_dilithium${level}_avx2)
endforeach()

# build test_speed
foreach(level IN LISTS securitylevel)
  add_executable(test_speed${level}_avx2
    test/cpucycles.c
    test/speed_print.c
    test/test_speed.c
  )
  target_link_libraries(test_speed${level}_avx2
    pqccrystals_fips202x4_avx2
    pqccrystals_dilithium${level}_avx2
    randombytes
  )
endforeach()

# build test_mul
add_executable(test_mul_avx2
  test/test_mul.c
)
target_link_libraries(test_mul_avx2
  pqccrystals_fips202x4_avx2
  pqccrystals_dilithium2_avx2
  randombytes
)