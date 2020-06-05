#!/bin/sh -e

for dir in ref avx2; do
  make -C $dir
  for alg in 2 2aes 3 3aes 4 4aes; do
    ./$dir/test/test_vectors$alg > tvecs$alg
    ./$dir/PQCgenKAT_sign$alg
  done
  # sha256sum not guaranteed to be installed in OSX
  if [ `uname` == "Darwin" ]; then
     shasum -a256 -c SHA256SUMS
  else
     sha256sum -c SHA256SUMS
  fi
done
