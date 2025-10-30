# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.html).

### Summary
This version introduces modifications to the `ref` implementation for performance analysis and benchmarking purposes. The core cryptographic logic of the original Dilithium algorithm remains unchanged. All modifications are confined to the `ref` directory and supplementary testing scripts.

### Added
- **Performance Benchmarking Framework:**
  - Introduced a `timing_info_t` struct in `ref/sign.h` to capture execution time for key generation, signing, and verification steps.
  - Integrated `clock_gettime` with `CLOCK_MONOTONIC` in `ref/sign.c` to precisely measure the duration of `crypto_sign_keypair`, `crypto_sign_signature`, and `crypto_sign_verify`. This required adding `#include <time.h>` and defining `_POSIX_C_SOURCE`.
  - Added `print_timing_info()` function to display aggregated timing results.
- **Automated Testing Support:**
  - Added `run_test` function prototype in `ref/sign.h` to facilitate running tests multiple times for stable performance metrics.
- **CPU Usage Test Script:**
  - Added `ref/cpu_used_test.sh`, a utility script to measure the average CPU percentage consumed by the test executable over multiple runs.
- **Sample Input File:**
  - Added `ref/test/input.txt` to provide a consistent sample input for testing and debugging.

### Changed
- **Disabled Debug Outputs:**
  - The extensive `printf` statements previously used for tracing the algorithm's flow in `ref/sign.c` have been disabled to allow for clean performance measurement. The original version with these outputs is preserved in `sign.c.old` for reference.
  - The temporary debug feature in `ref/test/test_dilithium.c` that wrote detailed outputs to `output.txt` has also been disabled. The test now only reads from `input.txt`.
- **Code Formatting:**
  - Applied consistent code formatting (e.g., spacing, newlines) across multiple files in the `ref` directory to improve readability.
- **Makefile Adjustments:**
  - Modified `ref/Makefile` to disable the compilation of `test_vectors` targets, streamlining the build process for performance testing.

### Removed
- (No features removed from the core implementation)

### Acknowledgements
This project is a fork of the official [pq-crystals/dilithium](https://github.com/pq-crystals/dilithium) repository. The modifications, available at [quannguyen247/dilithium-dev](https://github.com/quannguyen247/dilithium-dev), are focused on performance analysis and benchmarking. The core cryptographic logic of the original public domain implementation of CRYSTALS-Dilithium remains unchanged.