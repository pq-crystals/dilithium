CC ?= /usr/bin/cc
CFLAGS += -Wall -Wextra -Wpedantic -Wmissing-prototypes -Wredundant-decls \
   -Wshadow -Wpointer-arith -march=native -mtune=native -O3
NISTFLAGS += -Wno-unused-result -march=native -mtune=native -O3
SOURCES = sign.c packing.c polyvec.c poly.c ntt.c reduce.c rounding.c
HEADERS = config.h params.h api.h sign.h packing.h polyvec.h poly.h ntt.h \
  reduce.h rounding.h symmetric.h
KECCAK_SOURCES = $(SOURCES) fips202.c symmetric-shake.c
KECCAK_HEADERS = $(HEADERS) fips202.h
AES_SOURCES = $(SOURCES) fips202.c aes256ctr.c symmetric-aes.c
AES_HEADERS = $(HEADERS) fips202.h aes256ctr.h

all: \
  test/test_dilithium2 \
  test/test_dilithium3 \
  test/test_dilithium4 \
  test/test_dilithium2aes \
  test/test_dilithium3aes \
  test/test_dilithium4aes \
  test/test_vectors2 \
  test/test_vectors3 \
  test/test_vectors4 \
  test/test_vectors2aes \
  test/test_vectors3aes \
  test/test_vectors4aes \
  test/test_speed2 \
  test/test_speed3 \
  test/test_speed4 \
  test/test_speed2aes \
  test/test_speed3aes \
  test/test_speed4aes \
  PQCgenKAT_sign2 \
  PQCgenKAT_sign3 \
  PQCgenKAT_sign4 \
  PQCgenKAT_sign2aes \
  PQCgenKAT_sign3aes \
  PQCgenKAT_sign4aes

shared: \
  dilithium2_ref.so \
  dilithium3_ref.so \
  dilithium4_ref.so \
  dilithium2aes_ref.so \
  dilithium3aes_ref.so \
  dilithium4aes_ref.so \
  pqcrystals_ref.so

pqcrystals_ref.so: fips202.c fips202.h
	$(CC) -shared -fPIC $(CFLAGS) -o $@ $<

dilithium2_ref.so: $(SOURCES) $(HEADERS) symmetric-shake.c
	$(CC) -shared -fPIC $(CFLAGS) -DDILITHIUM_MODE=2 \
	  -o $@ $(SOURCES) symmetric-shake.c

dilithium3_ref.so: $(SOURCES) $(HEADERS) symmetric-shake.c
	$(CC) -shared -fPIC $(CFLAGS) -DDILITHIUM_MODE=3 \
	  -o $@ $(SOURCES) symmetric-shake.c

dilithium4_ref.so: $(SOURCES) $(HEADERS) symmetric-shake.c
	$(CC) -shared -fPIC $(CFLAGS) -DDILITHIUM_MODE=4 \
	  -o $@ $(SOURCES) symmetric-shake.c

dilithium2aes_ref.so: $(SOURCES) $(HEADERS) symmetric-aes.c
	$(CC) -shared -fPIC $(CFLAGS) -DDILITHIUM_MODE=2 -DDILITHIUM_USE_AES \
	   -o $@ $(SOURCES) symmetric-aes.c

dilithium3aes_ref.so: $(SOURCES) $(HEADERS) symmetric-aes.c
	$(CC) -shared -fPIC $(CFLAGS) -DDILITHIUM_MODE=3 -DDILITHIUM_USE_AES \
	   -o $@ $(SOURCES) symmetric-aes.c

dilithium4aes_ref.so: $(SOURCES) $(HEADERS) symmetric-aes.c
	$(CC) -shared -fPIC $(CFLAGS) -DDILITHIUM_MODE=4 -DDILITHIUM_USE_AES \
	   -o $@ $(SOURCES) symmetric-aes.c

test/test_dilithium2: test/test_dilithium.c randombytes.c $(KECCAK_SOURCES) \
  randombytes.h $(KECCAK_HEADERS)
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=2 \
	  -o $@ $< randombytes.c $(KECCAK_SOURCES)

test/test_dilithium3: test/test_dilithium.c randombytes.c $(KECCAK_SOURCES) \
  randombytes.h $(KECCAK_HEADERS)
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=3 \
	  -o $@ $< randombytes.c $(KECCAK_SOURCES)

test/test_dilithium4: test/test_dilithium.c randombytes.c $(KECCAK_SOURCES) \
  randombytes.h $(KECCAK_HEADERS)
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=4 \
	  -o $@ $< randombytes.c $(KECCAK_SOURCES)

test/test_dilithium2aes: test/test_dilithium.c randombytes.c $(AES_SOURCES) \
  randombytes.h $(AES_HEADERS)
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=2 -DDILITHIUM_USE_AES \
	  -o $@ $< randombytes.c $(AES_SOURCES)

test/test_dilithium3aes: test/test_dilithium.c randombytes.c $(AES_SOURCES) \
  randombytes.h $(AES_HEADERS)
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=3 -DDILITHIUM_USE_AES \
	  -o $@ $< randombytes.c $(AES_SOURCES)

test/test_dilithium4aes: test/test_dilithium.c randombytes.c $(AES_SOURCES) \
  randombytes.h $(AES_HEADERS)
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=4 -DDILITHIUM_USE_AES \
	  -o $@ $< randombytes.c $(AES_SOURCES)

test/test_vectors2: test/test_vectors.c rng.c $(KECCAK_SOURCES) \
  rng.h $(KECCAK_HEADERS)
	$(CC) $(NISTFLAGS) -DDILITHIUM_MODE=2 \
	  -o $@ $< rng.c $(KECCAK_SOURCES) -lcrypto

test/test_vectors3: test/test_vectors.c rng.c $(KECCAK_SOURCES) \
  rng.h $(KECCAK_HEADERS)
	$(CC) $(NISTFLAGS) -DDILITHIUM_MODE=3 \
	  -o $@ $< rng.c $(KECCAK_SOURCES) -lcrypto

test/test_vectors4: test/test_vectors.c rng.c $(KECCAK_SOURCES) \
  rng.h $(KECCAK_HEADERS)
	$(CC) $(NISTFLAGS) -DDILITHIUM_MODE=4 \
	  -o $@ $< rng.c $(KECCAK_SOURCES) -lcrypto

test/test_vectors2aes: test/test_vectors.c rng.c $(AES_SOURCES) \
  rng.h $(AES_HEADERS)
	$(CC) $(NISTFLAGS) -DDILITHIUM_MODE=2 -DDILITHIUM_USE_AES \
	  -o $@ $< rng.c $(AES_SOURCES) -lcrypto

test/test_vectors3aes: test/test_vectors.c rng.c $(AES_SOURCES) \
  rng.h $(AES_HEADERS)
	$(CC) $(NISTFLAGS) -DDILITHIUM_MODE=3 -DDILITHIUM_USE_AES \
	  -o $@ $< rng.c $(AES_SOURCES) -lcrypto

test/test_vectors4aes: test/test_vectors.c rng.c $(AES_SOURCES) \
  rng.h $(AES_HEADERS)
	$(CC) $(NISTFLAGS) -DDILITHIUM_MODE=4 -DDILITHIUM_USE_AES \
	  -o $@ $< rng.c $(AES_SOURCES) -lcrypto

test/test_speed2: test/test_speed.c test/speed_print.c test/cpucycles.c \
  randombytes.c $(KECCAK_SOURCES) \
  test/speed_print.h test/cpucycles.h randombytes.h $(KECCAK_HEADERS)
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=2 \
	  -o $@ $< test/speed_print.c test/cpucycles.c randombytes.c \
	  $(KECCAK_SOURCES)

test/test_speed3: test/test_speed.c test/speed_print.c test/cpucycles.c \
  randombytes.c $(KECCAK_SOURCES) \
  test/speed_print.h test/cpucycles.h randombytes.h $(KECCAK_HEADERS)
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=3 \
	  -o $@ $< test/speed_print.c test/cpucycles.c randombytes.c \
	  $(KECCAK_SOURCES)

test/test_speed4: test/test_speed.c test/speed_print.c test/cpucycles.c \
  randombytes.c $(KECCAK_SOURCES) \
  test/speed_print.h test/cpucycles.h randombytes.h $(KECCAK_HEADERS)
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=4 \
	  -o $@ $< test/speed_print.c test/cpucycles.c randombytes.c \
	  $(KECCAK_SOURCES)

test/test_speed2aes: test/test_speed.c test/speed_print.c test/cpucycles.c \
  randombytes.c $(AES_SOURCES) \
  test/speed_print.h test/cpucycles.h randombytes.h $(AES_HEADERS)
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=2 -DDILITHIUM_USE_AES \
	  -o $@ $< test/speed_print.c test/cpucycles.c randombytes.c \
	  $(AES_SOURCES)

test/test_speed3aes: test/test_speed.c test/speed_print.c test/cpucycles.c \
  randombytes.c $(AES_SOURCES) \
  test/speed_print.h test/cpucycles.h randombytes.h $(AES_HEADERS)
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=3 -DDILITHIUM_USE_AES \
	  -o $@ $< test/speed_print.c test/cpucycles.c randombytes.c \
	  $(AES_SOURCES)

test/test_speed4aes: test/test_speed.c test/speed_print.c test/cpucycles.c \
  randombytes.c $(AES_SOURCES) \
  test/speed_print.h test/cpucycles.h randombytes.h $(AES_HEADERS)
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=4 -DDILITHIUM_USE_AES \
	  -o $@ $< test/speed_print.c test/cpucycles.c randombytes.c \
	  $(AES_SOURCES)

test/test_mul: test/test_mul.c randombytes.c $(KECCAK_SOURCES) \
  randombytes.h $(KECCAK_HEADERS)
	$(CC) $(CFLAGS) -UDBENCH -o $@ $< randombytes.c $(KECCAK_SOURCES)

PQCgenKAT_sign2: PQCgenKAT_sign.c rng.c $(KECCAK_SOURCES) \
  rng.h $(KECCAK_HEADERS)
	$(CC) $(NISTFLAGS) -DDILITHIUM_MODE=2 \
	  -o $@ $< rng.c $(KECCAK_SOURCES) -lcrypto

PQCgenKAT_sign3: PQCgenKAT_sign.c rng.c $(KECCAK_SOURCES) \
  rng.h $(KECCAK_HEADERS)
	$(CC) $(NISTFLAGS) -DDILITHIUM_MODE=3 \
	  -o $@ $< rng.c $(KECCAK_SOURCES) -lcrypto

PQCgenKAT_sign4: PQCgenKAT_sign.c rng.c $(KECCAK_SOURCES) \
  rng.h $(KECCAK_HEADERS)
	$(CC) $(NISTFLAGS) -DDILITHIUM_MODE=4 \
	  -o $@ $< rng.c $(KECCAK_SOURCES) -lcrypto

PQCgenKAT_sign2aes: PQCgenKAT_sign.c rng.c $(AES_SOURCES) \
  rng.h $(AES_HEADERS)
	$(CC) $(NISTFLAGS) -DDILITHIUM_MODE=2 -DDILITHIUM_USE_AES \
	  -o $@ $< rng.c $(AES_SOURCES) -lcrypto

PQCgenKAT_sign3aes: PQCgenKAT_sign.c rng.c $(AES_SOURCES) \
  rng.h $(AES_HEADERS)
	$(CC) $(NISTFLAGS) -DDILITHIUM_MODE=3 -DDILITHIUM_USE_AES \
	  -o $@ $< rng.c $(AES_SOURCES) -lcrypto

PQCgenKAT_sign4aes: PQCgenKAT_sign.c rng.c $(AES_SOURCES) \
  rng.h $(AES_HEADERS)
	$(CC) $(NISTFLAGS) -DDILITHIUM_MODE=4 -DDILITHIUM_USE_AES \
	  -o $@ $< rng.c $(AES_SOURCES) -lcrypto

clean:
	rm -f *~ test/*~
	rm -f dilithium2_ref.so
	rm -f dilithium3_ref.so
	rm -f dilithium4_ref.so
	rm -f dilithium2aes_ref.so
	rm -f dilithium3aes_ref.so
	rm -f dilithium4aes_ref.so
	rm -f pqcrystals_ref.so
	rm -f test/test_dilithium2
	rm -f test/test_dilithium3
	rm -f test/test_dilithium4
	rm -f test/test_dilithium2aes
	rm -f test/test_dilithium3aes
	rm -f test/test_dilithium4aes
	rm -f test/test_vectors2
	rm -f test/test_vectors3
	rm -f test/test_vectors4
	rm -f test/test_vectors2aes
	rm -f test/test_vectors3aes
	rm -f test/test_vectors4aes
	rm -f test/test_speed2
	rm -f test/test_speed3
	rm -f test/test_speed4
	rm -f test/test_speed2aes
	rm -f test/test_speed3aes
	rm -f test/test_speed4aes
	rm -f test/test_mul
	rm -f PQCgenKAT_sign2
	rm -f PQCgenKAT_sign3
	rm -f PQCgenKAT_sign4
	rm -f PQCgenKAT_sign2aes
	rm -f PQCgenKAT_sign3aes
	rm -f PQCgenKAT_sign4aes
