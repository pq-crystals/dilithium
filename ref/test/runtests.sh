#!/bin/sh

make clean;

for m in 0 1 2 3; do
  NISTFLAGS="-DMODE=$m" make PQCgenKAT_sign test/test_vectors;
  ./PQCgenKAT_sign;
  ./test/test_vectors > testvectors_$m;
  rm PQCgenKAT_sign test/test_vectors;
done

make clean;
sha256sum -c ../SHA256SUMS;
rm testvectors_* PQCsignKAT_*.req PQCsignKAT_*.rsp;


