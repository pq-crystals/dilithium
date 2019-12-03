#!/bin/sh -e

make clean;

for m in 1 2 3 4; do
  export NISTFLAGS="-DMODE=$m";
  make PQCgenKAT_sign;
  make PQCgenKAT_sign-AES;
  make test/test_vectors;
  make test/test_vectors-AES;

  ./PQCgenKAT_sign;
  for FILE in PQCsignKAT_*.req PQCsignKAT_*.rsp; do
    mv "$FILE" "$(echo $FILE | sed s/^PQCsignKAT/PQCsignKAT-SHAKE/)";
  done

  ./PQCgenKAT_sign-AES;
  for FILE in PQCsignKAT_*.req PQCsignKAT_*.rsp; do
    mv "$FILE" "$(echo $FILE | sed s/^PQCsignKAT/PQCsignKAT-AES/)";
  done

  ./test/test_vectors > testvectors$m-SHAKE;
  ./test/test_vectors-AES > testvectors$m-AES;

  rm PQCgenKAT_sign;
  rm PQCgenKAT_sign-AES;
  rm test/test_vectors;
  rm test/test_vectors-AES;
done

make clean;
sha256sum -c ../SHA256SUMS;
rm testvectors* PQCsignKAT*.req PQCsignKAT*.rsp;


