CC=gcc
DILITHIUM_MODE ?= 5
USE_AVX2 ?= $(shell if [ `uname -m` = "x86_64" ] && grep -q avx2 /proc/cpuinfo; then echo "1"; else echo "0"; fi)

CFLAGS=-I${DILITHIUM_INCLUDE_DIR} -Wall -Wextra -DDILITHIUM_MODE=$(DILITHIUM_MODE)
CFLAGS += $(shell if [ "$(USE_AVX2)" = "1" ]; then echo "-DHAVE_AVX2"; fi)

LDFLAGS=-L${DILITHIUM_LIB_DIR}
IMPLEMENTATION=$(shell if [ "$(USE_AVX2)" = "1" ]; then echo "avx2"; else echo "ref"; fi)

ifeq ($(USE_AVX2),1)
    LDFLAGS += -lpqcrystals_dilithium$(DILITHIUM_MODE)_avx2 -lpqcrystals_fips202_avx2 -lpqcrystals_fips202x4_avx2
endif
LDFLAGS += -lpqcrystals_dilithium$(DILITHIUM_MODE)_ref -lpqcrystals_fips202_ref -lcrypto

all: generate_secretkey create_signature validate_signature test_dilithium


generate_secretkey: generate_secretkey.c base64.c
	$(CC) $(CFLAGS) -o $@ $< ../$(IMPLEMENTATION)/randombytes.c base64.c $(LDFLAGS)

create_signature: create_signature.c base64.c
	$(CC) $(CFLAGS) -o $@ $< ../$(IMPLEMENTATION)/randombytes.c base64.c $(LDFLAGS)

validate_signature: validate_signature.c base64.c
	$(CC) $(CFLAGS) -o $@ $< ../$(IMPLEMENTATION)/randombytes.c base64.c $(LDFLAGS)

test_dilithium: test_dilithium.c
	$(CC) $(CFLAGS) -o $@ $< ../$(IMPLEMENTATION)/randombytes.c $(LDFLAGS)

clean:
	rm -f generate_secretkey create_signature validate_signature test_dilithium message.txt public.key secret.key signature.bin
