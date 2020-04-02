CC ?= /usr/bin/cc
CFLAGS += -Wall -Wextra -Wpedantic -Wmissing-prototypes -Wredundant-decls \
   -march=native -mtune=native -O3
NISTFLAGS += -Wno-unused-result -march=native -mtune=native -O3 -fomit-frame-pointer
SOURCES = sign.c polyvec.c poly.c packing.c ntt.c reduce.c rounding.c
HEADERS = config.h api.h params.h sign.h polyvec.h poly.h packing.h ntt.h \
  reduce.h rounding.h symmetric.h
KECCAK_SOURCES = $(SOURCES) fips202.c symmetric-shake.c
KECCAK_HEADERS = $(HEADERS) fips202.h
AES_SOURCES = $(SOURCES) fips202.c aes256ctr.c symmetric-aes.c
AES_HEADERS = $(HEADERS) fips202.h aes256ctr.h

all: PQCgenKAT_sign test/test_vectors test/test_dilithium test/test_speed

dilithium.so: $(SOURCES) $(HEADERS) symmetric-shake.c
	$(CC) -shared -fPIC $(CFLAGS) $(SOURCES) symmetric-shake.c -o $@

dilithium-AES.so: $(SOURCES) $(HEADERS) symmetric-aes.c
	$(CC) -shared -fPIC $(CFLAGS) -DDILITHIUM_USE_AES \
	  $(SOURCES) symmetric-aes.c -o $@

PQCgenKAT_sign: PQCgenKAT_sign.c rng.c $(KECCAK_SOURCES) rng.h $(KECCAK_HEADERS)
	$(CC) $(NISTFLAGS) $< rng.c $(KECCAK_SOURCES) -o $@ -lcrypto

PQCgenKAT_sign-AES: PQCgenKAT_sign.c rng.c $(AES_SOURCES) rng.h $(AES_HEADERS)
	$(CC) $(NISTFLAGS) -DDILITHIUM_USE_AES $< rng.c $(AES_SOURCES) \
	  -o $@ -lcrypto

test/test_vectors: test/test_vectors.c rng.c $(KECCAK_SOURCES) rng.h \
  $(KECCAK_HEADERS)
	$(CC) $(NISTFLAGS) $< rng.c $(KECCAK_SOURCES) -o $@ -lcrypto

test/test_vectors-AES: test/test_vectors.c rng.c $(AES_SOURCES) rng.h \
  $(AES_HEADERS)
	$(CC) $(NISTFLAGS) -DDILITHIUM_USE_AES $< rng.c $(AES_SOURCES) \
	  -o $@ -lcrypto

test/test_dilithium: test/test_dilithium.c randombytes.c $(KECCAK_SOURCES) \
  randombytes.h $(KECCAK_HEADERS)
	$(CC) $(CFLAGS) $< randombytes.c $(KECCAK_SOURCES) -o $@

test/test_dilithium-AES: test/test_dilithium.c randombytes.c $(AES_SOURCES) \
  randombytes.h $(AES_HEADERS)
	$(CC) $(CFLAGS) -DDILITHIUM_USE_AES $< randombytes.c $(AES_SOURCES) \
	  -o $@

test/test_mul: test/test_mul.c randombytes.c $(KECCAK_SOURCES) randombytes.h \
  $(KECCAK_HEADERS)
	$(CC) $(CFLAGS) -UDBENCH $< randombytes.c $(KECCAK_SOURCES) -o $@

test/test_speed: test/test_speed.c randombytes.c test/cpucycles.c \
  test/speed_print.c $(KECCAK_SOURCES) randombytes.h test/cpucycles.h \
  test/speed_print.h $(KECCAK_HEADERS)
	$(CC) $(CFLAGS) $< randombytes.c test/cpucycles.c \
	  test/speed_print.c $(KECCAK_SOURCES) -o $@

test/test_speed-AES: test/test_speed.c randombytes.c test/cpucycles.c \
  test/speed_print.c $(AES_SOURCES) randombytes.h test/cpucycles.h \
  test/speed_print.h $(AES_HEADERS)
	$(CC) $(CFLAGS) -DDILITHIUM_USE_AES $< randombytes.c test/cpucycles.c \
	  test/speed_print.c $(AES_SOURCES) -o $@

.PHONY: clean

clean:
	rm -f *~ test/*~
	rm -f dilithium.so
	rm -f dilithium-AES.so
	rm -f PQCgenKAT_sign
	rm -f PQCgenKAT_sign-AES
	rm -f test/test_vectors
	rm -f test/test_vectors-AES
	rm -f test/test_dilithium
	rm -f test/test_dilithium-AES
	rm -f test/test_speed
	rm -f test/test_speed-AES
	rm -f test/test_mul
