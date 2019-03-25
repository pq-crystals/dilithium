CC ?= /usr/bin/cc
CFLAGS += -Wall -Wextra -march=native -mtune=native -O3 -fomit-frame-pointer
CFLAGS += -DUSE_RDPMC
#CFLAGS += -DDBENCH
NISTFLAGS += -march=native -mtune=native -O3 -fomit-frame-pointer
SOURCES = sign.c polyvec.c poly.c packing.c ntt.c reduce.c rounding.c
HEADERS = api.h params.h sign.h polyvec.h poly.h packing.h ntt.h reduce.h \
  rounding.h symmetric.h
KECCAK_SOURCES = $(SOURCES) fips202.c
KECCAK_HEADERS = $(HEADERS) fips202.h
NINETIES_SOURCES = $(SOURCES) fips202.c aes256ctr.c sha256.c sha512.c
NINETIES_HEADERS = $(HEADERS) fips202.h aes256ctr.h sha2.h

all: PQCgenKAT_sign test/test_vectors test/test_dilithium

PQCgenKAT_sign: PQCgenKAT_sign.c rng.c $(KECCAK_SOURCES) rng.h $(KECCAK_HEADERS)
	$(CC) $(NISTFLAGS) $< rng.c $(KECCAK_SOURCES) -o $@ -lcrypto

test/test_vectors: test/test_vectors.c rng.c $(KECCAK_SOURCES) rng.h \
  $(KECCAK_HEADERS)
	$(CC) $(NISTFLAGS) $< rng.c $(KECCAK_SOURCES) -o $@ -lcrypto

test/test_dilithium: test/test_dilithium.c randombytes.c test/cpucycles.c \
  test/speed.c $(KECCAK_SOURCES) randombytes.h test/cpucycles.h test/speed.h \
  $(KECCAK_HEADERS)
	$(CC) $(CFLAGS) $< randombytes.c test/cpucycles.c test/speed.c \
	  $(KECCAK_SOURCES) -o $@

test/test_dilithium90s: test/test_dilithium.c randombytes.c test/cpucycles.c \
  test/speed.c $(NINETIES_SOURCES) randombytes.h test/cpucycles.h test/speed.h \
  $(NINETIES_HEADERS)
	$(CC) $(CFLAGS) -DDILITHIUM_90S $< randombytes.c test/cpucycles.c \
	  test/speed.c $(NINETIES_SOURCES) -o $@

test/test_mul: test/test_mul.c randombytes.c test/cpucycles.c est/speed.c \
  $(KECCAK_SOURCES) randombytes.h test/cpucycles.h test/speed.h \
  $(NINETIES_HEADERS)
	$(CC) $(CFLAGS) -UDBENCH $< randombytes.c test/cpucycles.c \
	  test/speed.c $(NINETIES_SOURCES) -o $@

.PHONY: clean

clean:
	rm -f *~ test/*~
	rm -f PQCgenKAT_sign
	rm -f test/test_vectors
	rm -f test/test_dilithium
	rm -f test/test_dilithium90s
	rm -f test/test_mul
