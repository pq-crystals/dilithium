CC=gcc
USE_AVX2 ?= $(shell if [ `uname -m` = "x86_64" ] && grep -q avx2 /proc/cpuinfo; then echo "1"; else echo "0"; fi)

CFLAGS=-I${DILITHIUM_INCLUDE_DIR} -Wall -Wextra
CFLAGS += $(shell if grep -q avx2 /proc/cpuinfo; then echo "-DDILITHIUM_USE_AVX2 -mavx2"; fi)

LDFLAGS ?= -L${DILITHIUM_LIB_DIR}
IMPLEMENTATION=$(shell if [ "$(USE_AVX2)" = "1" ]; then echo "avx2"; else echo "ref"; fi)

# Add all Dilithium versions to linker flags
ifeq ($(USE_AVX2),1)
    LDFLAGS += -lpqcrystals_dilithium2_avx2 -lpqcrystals_dilithium3_avx2 -lpqcrystals_dilithium5_avx2 \
               -lpqcrystals_fips202_avx2 -lpqcrystals_fips202x4_avx2
else
    LDFLAGS += -lpqcrystals_dilithium2_ref -lpqcrystals_dilithium3_ref -lpqcrystals_dilithium5_ref -lpqcrystals_fips202_ref
endif
LDFLAGS += -lcrypto


all: dilithium2-keygen dilithium2-sign dilithium2-verify \
     dilithium3-keygen dilithium3-sign dilithium3-verify \
     dilithium5-keygen dilithium5-sign dilithium5-verify

dilithium2-keygen: keygen.c base64.c
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=2 -o $@ $< ../$(IMPLEMENTATION)/randombytes.c base64.c $(LDFLAGS)

dilithium2-sign: sign.c base64.c
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=2 -o $@ $< ../$(IMPLEMENTATION)/randombytes.c base64.c $(LDFLAGS)

dilithium2-verify: verify.c base64.c
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=2 -o $@ $< ../$(IMPLEMENTATION)/randombytes.c base64.c $(LDFLAGS)

dilithium3-keygen: keygen.c base64.c
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=3 -o $@ $< ../$(IMPLEMENTATION)/randombytes.c base64.c $(LDFLAGS)

dilithium3-sign: sign.c base64.c
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=3 -o $@ $< ../$(IMPLEMENTATION)/randombytes.c base64.c $(LDFLAGS)

dilithium3-verify: verify.c base64.c
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=3 -o $@ $< ../$(IMPLEMENTATION)/randombytes.c base64.c $(LDFLAGS)

dilithium5-keygen: keygen.c base64.c
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=5 -o $@ $< ../$(IMPLEMENTATION)/randombytes.c base64.c $(LDFLAGS)

dilithium5-sign: sign.c base64.c
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=5 -o $@ $< ../$(IMPLEMENTATION)/randombytes.c base64.c $(LDFLAGS)

dilithium5-verify: verify.c base64.c
	$(CC) $(CFLAGS) -DDILITHIUM_MODE=5 -o $@ $< ../$(IMPLEMENTATION)/randombytes.c base64.c $(LDFLAGS)

test: all
	./runtest.sh

clean:
	rm -f dilithium2-keygen dilithium2-sign dilithium2-verify \
	      dilithium3-keygen dilithium3-sign dilithium3-verify \
	      dilithium5-keygen dilithium5-sign dilithium5-verify \
	      message*.txt public*.key secret*.key signature*.bin

.PHONY: all test clean
