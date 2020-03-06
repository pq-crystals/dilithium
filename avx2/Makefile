CC ?= /usr/bin/cc
CFLAGS += -Wall -Wextra -Wpedantic -Wmissing-prototypes -Wredundant-decls \
  -march=native -mtune=native -O3 -fomit-frame-pointer
NISTFLAGS += -march=native -mtune=native -O3 -fomit-frame-pointer
SOURCES = sign.c polyvec.c poly.c packing.c ntt.s invntt.s pointwise.S \
  nttconsts.c rejsample.c reduce.s rounding.c
HEADERS = config.h api.h params.h sign.h polyvec.h poly.h packing.h ntt.h \
  rejsample.h reduce.h rounding.h symmetric.h
KECCAK_SOURCES = $(SOURCES) fips202.c fips202x4.c \
  keccak4x/KeccakP-1600-times4-SIMD256.o
KECCAK_HEADERS = $(HEADERS) fips202.h fips202x4.h
AES_SOURCES = $(SOURCES) fips202.c aes256ctr.c
AES_HEADERS = $(HEADERS) fips202.h aes256ctr.h

all: PQCgenKAT_sign test/test_vectors test/test_dilithium

keccak4x/KeccakP-1600-times4-SIMD256.o: keccak4x/KeccakP-1600-times4-SIMD256.c \
  keccak4x/align.h keccak4x/brg_endian.h keccak4x/KeccakP-1600-times4-SnP.h \
  keccak4x/KeccakP-1600-unrolling.macros keccak4x/SIMD256-config.h
	$(CC) $(CFLAGS) -c $< -o $@

PQCgenKAT_sign: PQCgenKAT_sign.c rng.c $(KECCAK_SOURCES) rng.h $(KECCAK_HEADERS)
	$(CC) $(NISTFLAGS) $< rng.c $(KECCAK_SOURCES) -o $@ -lcrypto

PQCgenKAT_sign-AES: PQCgenKAT_sign.c rng.c $(AES_SOURCES) rng.h $(AES_HEADERS)
	$(CC) $(NISTFLAGS) -DUSE_AES $< rng.c $(AES_SOURCES) -o $@ -lcrypto

test/test_vectors: test/test_vectors.c rng.c $(KECCAK_SOURCES) rng.h \
  $(KECCAK_HEADERS)
	$(CC) $(NISTFLAGS) $< rng.c $(KECCAK_SOURCES) -o $@ -lcrypto

test/test_vectors-AES: test/test_vectors.c rng.c $(AES_SOURCES) rng.h \
  $(AES_HEADERS)
	$(CC) $(NISTFLAGS) -DUSE_AES $< rng.c $(AES_SOURCES) -o $@ -lcrypto

test/test_dilithium: test/test_dilithium.c randombytes.c test/cpucycles.c \
  test/speed.c $(KECCAK_SOURCES) randombytes.h test/cpucycles.h test/speed.h \
  $(KECCAK_HEADERS)
	$(CC) $(CFLAGS) $< randombytes.c test/cpucycles.c test/speed.c \
	  $(KECCAK_SOURCES) -o $@

test/test_dilithium-AES: test/test_dilithium.c randombytes.c test/cpucycles.c \
  test/speed.c $(AES_SOURCES) randombytes.h test/cpucycles.h test/speed.h \
  $(AES_HEADERS)
	$(CC) $(CFLAGS) -DUSE_AES $< randombytes.c test/cpucycles.c \
	  test/speed.c $(AES_SOURCES) -o $@

test/test_mul: test/test_mul.c randombytes.c test/cpucycles.c test/speed.c \
  $(KECCAK_SOURCES) randombytes.h test/cpucycles.h test/speed.h \
  $(KECCAK_HEADERS)
	$(CC) $(CFLAGS) -UDBENCH $< randombytes.c test/cpucycles.c \
	  test/speed.c $(KECCAK_SOURCES) -o $@

.PHONY: clean

clean:
	rm -f *~ test/*~
	rm -f keccak4x/KeccakP-1600-times4-SIMD256.o
	rm -f PQCgenKAT_sign
	rm -f PQCgenKAT_sign-AES
	rm -f test/test_vectors
	rm -f test/test_vectors-AES
	rm -f test/test_dilithium
	rm -f test/test_dilithium-AES
	rm -f test/test_mul
