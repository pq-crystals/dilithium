# Dilithium

[![Build Status](https://travis-ci.org/pq-crystals/dilithium.svg?branch=master)](https://travis-ci.org/pq-crystals/dilithium) [![Coverage Status](https://coveralls.io/repos/github/pq-crystals/dilithium/badge.svg?branch=master)](https://coveralls.io/github/pq-crystals/dilithium?branch=master)

This directory contains our implementation of [Dilithium](https://eprint.iacr.org/2017/633). Both the reference code and the AVX2 optimized code are in the directories ref/ and avx2/, respectively. It contains a test program called test/test_dilithium that can be compiled on unix by running `make` in ref/ resp. avx2/.

## CMake

Also available is a highly portable [cmake](https://cmake.org) based build system that permits building the same sources into a summary library as well as all the same tests.

For fastest build performance, use of [Ninja](https://ninja-build.org) is recommended.

All tests can be run by invoking the (your-favourite-build-tool-command-here) `test` target.

### Worked example

By calling 
```
mkdir build-ninja && cd build-ninja && cmake -DBUILD_SHARED_LIBS=ON -GNinja .. && ninja && ninja test
```

the whole Dilithium software family gets built in a highly portable as well as an avx2-optimized version, tested and delivered in a shared library.

For example, by running `./avx2/test_speed4aes_avx2` in the newly created 'build-ninja' folder, performance testing of `Dilithium4-AES` in the optimized AVX2 variant is executed.

The resultant library might also be installed using the `install` target.
