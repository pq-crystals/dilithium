#!/bin/sh

make clean;

for m in 0 1 2 3; do
  export NISTFLAGS="-DMODE=$m";
  make PQCgenKAT_sign;
  make PQCgenKAT_sign-90s;
  make test/test_vectors;
  make test/test_vectors-90s;

  ./PQCgenKAT_sign;
  for FILE in PQCsignKAT_*.req PQCsignKAT_*.rsp; do
    mv "$FILE" "$(echo $FILE | sed s/^PQCsignKAT/PQCsignKAT-SHAKE/)";
  done

  ./PQCgenKAT_sign-90s;
  for FILE in PQCsignKAT_*.req PQCsignKAT_*.rsp; do
    mv "$FILE" "$(echo $FILE | sed s/^PQCsignKAT/PQCsignKAT-90s/)";
  done

  ./test/test_vectors > testvectors$m-SHAKE;
  ./test/test_vectors-90s > testvectors$m-90s;

  rm PQCgenKAT_sign;
  rm PQCgenKAT_sign-90s;
  rm test/test_vectors;
  rm test/test_vectors-90s;
done

make clean;
sha256sum -c ../SHA256SUMS;
rm testvectors* PQCsignKAT*.req PQCsignKAT*.rsp;


