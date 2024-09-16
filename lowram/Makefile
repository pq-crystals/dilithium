CC ?= /usr/bin/cc
CFLAGS += -Wall -Wextra -Wpedantic -Wmissing-prototypes -Wredundant-decls \
  -Wshadow -Wvla -Wpointer-arith -O3 -fomit-frame-pointer
NISTFLAGS += -Wno-unused-result -O3 -fomit-frame-pointer
SOURCES = sign.c packing.c polyvec.c poly.c ntt.c reduce.c rounding.c lowram.c smallpoly.c smallntt_3329.c
HEADERS = config.h params.h api.h sign.h packing.h polyvec.h poly.h ntt.h \
  reduce.h rounding.h symmetric.h randombytes.h lowram.h smallpoly.h smallntt.h
KECCAK_SOURCES = $(SOURCES) fips202.c symmetric-shake.c
KECCAK_HEADERS = $(HEADERS) fips202.h

.PHONY: all speed shared clean

all: \
  test/test_dilithium2 \
  test/test_dilithium3 \
  test/test_dilithium5 \
  test/test_vectors2 \
  test/test_vectors3 \
  test/test_vectors5

speed: \
  test/test_mul \
  test/test_speed2 \
  test/test_speed3 \
  test/test_speed5 \

shared: \
  libpqcrystals_dilithium2_lowram.so \
  libpqcrystals_dilithium3_lowram.so \
  libpqcrystals_dilithium5_lowram.so \
  libpqcrystals_fips202_lowram.so \

libpqcrystals_fips202_lowram.so: fips202.c fips202.h
	$(CC) -shared -fPIC $(CFLAGS) -o $@ $<

libpqcrystals_dilithium2_lowram.so: $(SOURCES) $(HEADERS) symmetric-shake.c
	$(CC) -shared -fPIC $(CFLAGS) -DDILITHIUM_MODE=2 \
	  -o $@ $(SOURCES) symmetric-shake.c

libpqcrystals_dilithium3_lowram.so: $(SOURCES) $(HEADERS) symmetric-shake.c
	$(CC) -shared -fPIC $(CFLAGS) -DDILITHIUM_MODE=3 \
	  -o $@ $(SOURCES) symmetric-shake.c

libpqcrystals_dilithium5_lowram.so: $(SOURCES) $(HEADERS) symmetric-shake.c
	$(CC) -shared -fPIC $(CFLAGS) -DDILITHIUM_MODE=5 \
	  -o $@ $(SOURCES) symmetric-shake.c

test/test_dilithium2: test/test_dilithium.c randombytes.c $(KECCAK_SOURCES) \
  $(KECCAK_HEADERS)
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=2 \
	  -o $@ $< randombytes.c $(KECCAK_SOURCES)

test/test_dilithium3: test/test_dilithium.c randombytes.c $(KECCAK_SOURCES) \
  $(KECCAK_HEADERS)
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=3 \
	  -o $@ $< randombytes.c $(KECCAK_SOURCES)

test/test_dilithium5: test/test_dilithium.c randombytes.c $(KECCAK_SOURCES) \
  $(KECCAK_HEADERS)
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=5 \
	  -o $@ $< randombytes.c $(KECCAK_SOURCES)

test/test_vectors2: test/test_vectors.c $(KECCAK_SOURCES) \
  $(KECCAK_HEADERS)
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=2 \
	  -o $@ $< $(KECCAK_SOURCES)

test/test_vectors3: test/test_vectors.c $(KECCAK_SOURCES) $(KECCAK_HEADERS)
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=3 \
	  -o $@ $< $(KECCAK_SOURCES)

test/test_vectors5: test/test_vectors.c $(KECCAK_SOURCES) \
  $(KECCAK_HEADERS)
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=5 \
	  -o $@ $< $(KECCAK_SOURCES)

test/test_speed2: test/test_speed.c test/speed_print.c test/speed_print.h \
  test/cpucycles.c test/cpucycles.h randombytes.c $(KECCAK_SOURCES) \
  $(KECCAK_HEADERS)
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=2 \
	  -o $@ $< test/speed_print.c test/cpucycles.c randombytes.c \
	  $(KECCAK_SOURCES)

test/test_speed3: test/test_speed.c test/speed_print.c test/speed_print.h \
  test/cpucycles.c test/cpucycles.h randombytes.c $(KECCAK_SOURCES) \
  $(KECCAK_HEADERS)
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=3 \
	  -o $@ $< test/speed_print.c test/cpucycles.c randombytes.c \
	  $(KECCAK_SOURCES)

test/test_speed5: test/test_speed.c test/speed_print.c test/speed_print.h \
  test/cpucycles.c test/cpucycles.h randombytes.c $(KECCAK_SOURCES) \
  $(KECCAK_HEADERS)
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=5 \
	  -o $@ $< test/speed_print.c test/cpucycles.c randombytes.c \
	  $(KECCAK_SOURCES)

test/test_mul: test/test_mul.c randombytes.c $(KECCAK_SOURCES) $(KECCAK_HEADERS)
	$(CC) $(CFLAGS) -UDBENCH -o $@ $< randombytes.c $(KECCAK_SOURCES)

clean:
	rm -f *~ test/*~ *.gcno *.gcda *.lcov
	rm -f libpqcrystals_dilithium2_lowram.so
	rm -f libpqcrystals_dilithium3_lowram.so
	rm -f libpqcrystals_dilithium5_lowram.so
	rm -f libpqcrystals_fips202_lowram.so
	rm -f test/test_dilithium2
	rm -f test/test_dilithium3
	rm -f test/test_dilithium5
	rm -f test/test_vectors2
	rm -f test/test_vectors3
	rm -f test/test_vectors5
	rm -f test/test_speed2
	rm -f test/test_speed3
	rm -f test/test_speed5
	rm -f test/test_mul
