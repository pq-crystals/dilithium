#!/bin/sh -e

ARCH="${ARCH:-amd64}"
ARCH="${TRAVIS_CPU_ARCH:-$ARCH}"

if [ "$ARCH" = "amd64" -a "$TRAVIS_OS_NAME" != "osx" ]; then
  DIRS="ref avx2"
else
  DIRS="ref"
fi

for dir in $DIRS; do
  make -C $dir
  for alg in 2 2aes 3 3aes 4 4aes; do
    ./$dir/test/test_dilithium$alg
    ./$dir/test/test_vectors$alg > tvecs$alg
    ./$dir/PQCgenKAT_sign$alg
  done
  shasum -a 256 -c SHA256SUMS
done

exit 0
