# Dilithium

[![Build Status](https://travis-ci.org/pq-crystals/dilithium.svg?branch=master)](https://travis-ci.org/pq-crystals/dilithium) [![Coverage Status](https://coveralls.io/repos/github/pq-crystals/dilithium/badge.svg?branch=master)](https://coveralls.io/github/pq-crystals/dilithium?branch=master)

This directory contains our implementation of [Dilithium](https://eprint.iacr.org/2017/633). Both the reference code and the AVX2 optimized code are in the directories ref/ and avx2/, respectively. It contains a test program called test/test_dilithium that can be compiled on unix by running `make` in ref/ resp. avx2/.
