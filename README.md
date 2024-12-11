# Dilithium

[![Build Status](https://travis-ci.org/pq-crystals/dilithium.svg?branch=master)](https://travis-ci.org/pq-crystals/dilithium) [![Coverage Status](https://coveralls.io/repos/github/pq-crystals/dilithium/badge.svg?branch=master)](https://coveralls.io/github/pq-crystals/dilithium?branch=master)

This repository contains the official reference implementation of the [Dilithium](https://www.pq-crystals.org/dilithium/) signature scheme, and an optimized implementation for x86 CPUs supporting the AVX2 instruction set. Dilithium is standardized as [FIPS 204](https://csrc.nist.gov/pubs/fips/204/final).

## Running Dilithium

Dilithium can be used via Nix flakes. Make sure you have Nix [installed with flakes enabled](https://nixos.org/manual/nix/stable/installation/installing-binary.html#installation-with-flakes).

To try Dilithium directly on the command line without cloning this repository, run:
```sh
nix run github:pq-crystals/dilithium -- --help
```

The following commands create a signing key and a signature for a file "message.txt":
```sh
nix run github:pq-crystals/dilithium -- --keygen -v 2 -p public.key -s secret.key
nix run github:pq-crystals/dilithium -- --sign -v 2 -p public.key -s secret.key -i message.txt -S signature.bin
```

Use the `--verify` flag to verify a signature:
```sh
nix run github:pq-crystals/dilithium -- --verify -v 2 -p public.key -i message.txt -S signature.bin
```

## Build instructions

The implementations contain several test and benchmarking programs and a Makefile to facilitate compilation.

### Prerequisites

Some of the test programs require [OpenSSL](https://openssl.org). If the OpenSSL header files and/or shared libraries do not lie in one of the standard locations on your system, it is necessary to specify their location via compiler and linker flags in the environment variables `CFLAGS`, `NISTFLAGS`, and `LDFLAGS`.

For example, on macOS you can install OpenSSL via [Homebrew](https://brew.sh) by running
```sh
brew install openssl
```
Then, run
```sh
export CFLAGS="-I/opt/homebrew/opt/openssl@1.1/include"
export NISTFLAGS="-I/opt/homebrew/opt/openssl@1.1/include"
export LDFLAGS="-L/opt/homebrew/opt/openssl@1.1/lib"
```
before compilation to add the OpenSSL header and library locations to the respective search paths.

### Test programs

To compile the test programs on Linux or macOS, go to the `ref/` or `avx2/` directory and run
```sh
make
```
This produces the executables
```sh
test/test_dilithium$ALG
test/test_vectors$ALG
```
where `$ALG` ranges over the parameter sets 2, 3, and 5.

* `test_dilithium$ALG` tests 10000 times to generate keys, sign a random message of 59 bytes and verify the produced signature. Also, the program will try to verify wrong signatures where a single random byte of a valid signature was randomly distorted. The program will abort with an error message and return -1 if there was an error. Otherwise it will output the key and signature sizes and return 0.
* `test_vectors$ALG` performs further tests of internal functions and prints deterministically generated test vectors for several intermediate values that occur in the Dilithium algorithms. Namely, a 48 byte seed, the matrix A corresponding to the first 32 bytes of seed, a short secret vector s corresponding to the first 32 bytes of seed and nonce 0, a masking vector y corresponding to the seed and nonce 0, the high bits w1 and the low bits w0 of the vector w = Ay, the power-of-two rounding t1 of w and the corresponding low part t0, and the challenge c for the seed and w1. This program is meant to help to ensure compatibility of independent implementations.

### Benchmarking programs

For benchmarking the implementations, we provide speed test programs for x86 CPUs that use the Time Step Counter (TSC) or the actual cycle counter provided by the Performance Measurement Counters (PMC) to measure performance. To compile the programs run
```sh
make speed
```
This produces the executables
```sh
test/test_speed$ALG
```
for all parameter sets `$ALG` as above. The programs report the median and average cycle counts of 10000 executions of various internal functions and the API functions for key generation, signing and verification. By default the Time Step Counter is used. If instead you want to obtain the actual cycle counts from the Performance Measurement Counters export `CFLAGS="-DUSE_RDPMC"` before compilation.

Please note that the reference implementation in `ref/` is not optimized for any platform, and, since it prioritises clean code, is significantly slower than a trivially optimized but still platform-independent implementation. Hence benchmarking the reference code does not provide representative results.

Our Dilithium implementations are contained in the [SUPERCOP](https://bench.cr.yp.to) benchmarking framework. See [here](http://bench.cr.yp.to/results-sign.html#amd64-kizomba) for current cycle counts on an Intel KabyLake CPU.

## Development using Nix

This repository provides a Nix flake with the following outputs:

- `packages.dilithium`: The main Dilithium CLI tools
- `packages.dilithium-lib`: The Dilithium shared libraries
- `devShells.default`: A development environment with all dependencies

### Flake Usage Examples

To use Dilithium as a dependency in your own flake:
```nix
{
  inputs.dilithium.url = "github:pq-crystals/dilithium";
  
  outputs = { self, nixpkgs, dilithium }: {
    # Use the library
    packages.default = pkgs.stdenv.mkDerivation {
      buildInputs = [ dilithium.packages.${system}.dilithium-lib ];
      # ...
    };
  };
}
```

The following command will enter the development shell and provide the necessary environment variables (CFLAGS and LDFLAGS required to link against Dilithium and OpenSSL):
```sh
nix develop github:pq-crystals/dilithium
```

The development shell provides:
- All build dependencies
- Dilithium libraries and headers
- Proper environment variables (CFLAGS, LDFLAGS, etc.)

To compile and run tests described above, the same make commands work in either directory:
```sh
make
```

This produces the same test executables:
```sh
test/test_dilithium$ALG
test/test_vectors$ALG
```
where `$ALG` ranges over the parameter sets 2, 3, and 5.

For performance benchmarking, you can additionally run and execute the speed tests:
```sh
make speed
test/test_speed$ALG
```
These programs measure performance using the CPU's Time Step Counter (TSC) by default, reporting median and average cycle counts over 10000 executions of key operations.

## Randomized signing

By default our code implements Dilithium's hedged signing mode. To change this to the deterministic signing mode, undefine the `DILITHIUM_RANDOMIZED_SIGNING` preprocessor macro at compilation by either commenting the line
```sh
#define DILITHIUM_RANDOMIZED_SIGNING
```
in config.h, or adding `-UDILITHIUM_RANDOMIZED_SIGNING` to the compiler flags in the environment variable `CFLAGS`.

## Shared libraries

All implementations can be compiled into shared libraries by running
```sh
make shared
```
For example in the directory `ref/` of the reference implementation, this produces the libraries
```sh
libpqcrystals_dilithium$ALG_ref.so
```
for all parameter sets `$ALG`, and the required symmetric crypto library
```
libpqcrystals_fips202_ref.so
```
All global symbols in the libraries lie in the namespaces `pqcrystals_dilithium$ALG_ref` and `libpqcrystals_fips202_ref`. Hence it is possible to link a program against all libraries simultaneously and obtain access to all implementations for all parameter sets. The corresponding API header file is `ref/api.h`, which contains prototypes for all API functions and preprocessor defines for the key and signature lengths.
