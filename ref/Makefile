CC = /usr/bin/cc
CFLAGS += -Wall -Wextra -march=native -mtune=native -O3 -fomit-frame-pointer
CFLAGS += -DUSE_RDPMC
NISTFLAGS = -march=native -mtune=native -O3 -fomit-frame-pointer -fPIC
SOURCES = sign.c polyvec.c packing.c poly.c reduce.c ntt.c rounding.c fips202.c
HEADERS = api.h params.h sign.h polyvec.h packing.h poly.h reduce.h ntt.h \
  rounding.h fips202.h

all: PQCgenKAT_sign test/test_vectors test/test_dilithium

PQCgenKAT_sign: PQCgenKAT_sign.c rng.c $(SOURCES) rng.h $(HEADERS)
	$(CC) $(NISTFLAGS) $< rng.c $(SOURCES) -o $@ -lcrypto -ldl

test/test_vectors: test/test_vectors.c rng.c $(SOURCES) rng.h $(HEADERS)
	$(CC) $(NISTFLAGS) $< rng.c $(SOURCES) -o $@ -lcrypto -ldl

test/test_dilithium: test/test_dilithium.c randombytes.c test/cpucycles.c \
  test/speed.c $(SOURCES) randombytes.h test/cpucycles.h test/speed.h $(HEADERS)
	$(CC) $(CFLAGS) $< randombytes.c test/cpucycles.c test/speed.c \
	  $(SOURCES) -o $@

test/test_mul: test/test_mul.c randombytes.c test/cpucycles.c test/speed.c \
  $(SOURCES) randombytes.h test/cpucycles.h test/speed.h $(HEADERS)
	$(CC) $(CFLAGS) -UDBENCH $< randombytes.c test/cpucycles.c \
	  test/speed.c $(SOURCES) -o $@

.PHONY: clean

clean:
	rm -f *~ test/*~
	rm -f PQCgenKAT_sign
	rm -f test/test_vectors
	rm -f test/test_dilithium
	rm -f test/test_mul
