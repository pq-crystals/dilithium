#ifndef API_H
#define API_H

#include <stddef.h>
#include <stdint.h>

#define libpqcrystals_dilithium2_ref_PUBLICKEYBYTES 1184
#define libpqcrystals_dilithium2_ref_SECRETKEYBYTES 2800
#define libpqcrystals_dilithium2_ref_BYTES 2044

int libpqcrystals_dilithium2_ref_keypair(uint8_t *pk, uint8_t *sk);

int libpqcrystals_dilithium2_ref_signature(uint8_t *sig, size_t *siglen,
                                           const uint8_t *m, size_t mlen,
                                           const uint8_t *sk);

int libpqcrystals_dilithium2_ref(uint8_t *sm, size_t *smlen,
                                 const uint8_t *m, size_t mlen,
                                 const uint8_t *sk);

int libpqcrystals_dilithium2_ref_verify(const uint8_t *sig, size_t siglen,
                                        const uint8_t *m, size_t mlen,
                                        const uint8_t *pk);

int libpqcrystals_dilithium2_ref_open(uint8_t *m, size_t *mlen,
                                      const uint8_t *sm, size_t smlen,
                                      const uint8_t *pk);

#define libpqcrystals_dilithium2aes_ref_PUBLICKEYBYTES \
        libpqcrystals_dilithium2_ref_PUBLICKEYBYTES
#define libpqcrystals_dilithium2aes_ref_SECRETKEYBYTES \
        libpqcrystals_dilithium2_ref_SECRETKEYBYTES
#define libpqcrystals_dilithium2aes_ref_BYTES \
        libpqcrystals_dilithium2_ref_BYTES

int libpqcrystals_dilithium2aes_ref_keypair(uint8_t *pk, uint8_t *sk);

int libpqcrystals_dilithium2aes_ref_signature(uint8_t *sig, size_t *siglen,
                                           const uint8_t *m, size_t mlen,
                                           const uint8_t *sk);

int libpqcrystals_dilithium2aes_ref(uint8_t *sm, size_t *smlen,
                                 const uint8_t *m, size_t mlen,
                                 const uint8_t *sk);

int libpqcrystals_dilithium2aes_ref_verify(const uint8_t *sig, size_t siglen,
                                        const uint8_t *m, size_t mlen,
                                        const uint8_t *pk);

int libpqcrystals_dilithium2aes_ref_open(uint8_t *m, size_t *mlen,
                                      const uint8_t *sm, size_t smlen,
                                      const uint8_t *pk);

#define libpqcrystals_dilithium3_ref_PUBLICKEYBYTES 1472
#define libpqcrystals_dilithium3_ref_SECRETKEYBYTES 3504
#define libpqcrystals_dilithium3_ref_BYTES 2701

int libpqcrystals_dilithium3_ref_keypair(uint8_t *pk, uint8_t *sk);

int libpqcrystals_dilithium3_ref_signature(uint8_t *sig, size_t *siglen,
                                           const uint8_t *m, size_t mlen,
                                           const uint8_t *sk);

int libpqcrystals_dilithium3_ref(uint8_t *sm, size_t *smlen,
                                 const uint8_t *m, size_t mlen,
                                 const uint8_t *sk);

int libpqcrystals_dilithium3_ref_verify(const uint8_t *sig, size_t siglen,
                                        const uint8_t *m, size_t mlen,
                                        const uint8_t *pk);

int libpqcrystals_dilithium3_ref_open(uint8_t *m, size_t *mlen,
                                      const uint8_t *sm, size_t smlen,
                                      const uint8_t *pk);

#define libpqcrystals_dilithium3aes_ref_PUBLICKEYBYTES \
        libpqcrystals_dilithium3_ref_PUBLICKEYBYTES
#define libpqcrystals_dilithium3aes_ref_SECRETKEYBYTES \
        libpqcrystals_dilithium3_ref_SECRETKEYBYTES
#define libpqcrystals_dilithium3aes_ref_BYTES \
        libpqcrystals_dilithium3_ref_BYTES

int libpqcrystals_dilithium3aes_ref_keypair(uint8_t *pk, uint8_t *sk);

int libpqcrystals_dilithium3aes_ref_signature(uint8_t *sig, size_t *siglen,
                                              const uint8_t *m, size_t mlen,
                                              const uint8_t *sk);

int libpqcrystals_dilithium3aes_ref(uint8_t *sm, size_t *smlen,
                                    const uint8_t *m, size_t mlen,
                                    const uint8_t *sk);

int libpqcrystals_dilithium3aes_ref_verify(const uint8_t *sig, size_t siglen,
                                           const uint8_t *m, size_t mlen,
                                           const uint8_t *pk);

int libpqcrystals_dilithium3aes_ref_open(uint8_t *m, size_t *mlen,
                                         const uint8_t *sm, size_t smlen,
                                         const uint8_t *pk);

#define libpqcrystals_dilithium4_ref_PUBLICKEYBYTES 1760
#define libpqcrystals_dilithium4_ref_SECRETKEYBYTES 3856
#define libpqcrystals_dilithium4_ref_BYTES 3366

int libpqcrystals_dilithium4_ref_keypair(uint8_t *pk, uint8_t *sk);

int libpqcrystals_dilithium4_ref_signature(uint8_t *sig, size_t *siglen,
                                           const uint8_t *m, size_t mlen,
                                           const uint8_t *sk);

int libpqcrystals_dilithium4_ref(uint8_t *sm, size_t *smlen,
                                 const uint8_t *m, size_t mlen,
                                 const uint8_t *sk);

int libpqcrystals_dilithium4_ref_verify(const uint8_t *sig, size_t siglen,
                                        const uint8_t *m, size_t mlen,
                                        const uint8_t *pk);

int libpqcrystals_dilithium4_ref_open(uint8_t *m, size_t *mlen,
                                      const uint8_t *sm, size_t smlen,
                                      const uint8_t *pk);

#define libpqcrystals_dilithium4aes_ref_PUBLICKEYBYTES \
        libpqcrystals_dilithium4_ref_PUBLICKEYBYTES
#define libpqcrystals_dilithium4aes_ref_SECRETKEYBYTES \
        libpqcrystals_dilithium4_ref_SECRETKEYBYTES
#define libpqcrystals_dilithium4aes_ref_BYTES \
        libpqcrystals_dilithium4_ref_BYTES

int libpqcrystals_dilithium4aes_ref_keypair(uint8_t *pk, uint8_t *sk);

int libpqcrystals_dilithium4aes_ref_signature(uint8_t *sig, size_t *siglen,
                                              const uint8_t *m, size_t mlen,
                                              const uint8_t *sk);

int libpqcrystals_dilithium4aes_ref(uint8_t *sm, size_t *smlen,
                                    const uint8_t *m, size_t mlen,
                                    const uint8_t *sk);

int libpqcrystals_dilithium4aes_ref_verify(const uint8_t *sig, size_t siglen,
                                           const uint8_t *m, size_t mlen,
                                           const uint8_t *pk);

int libpqcrystals_dilithium4aes_ref_open(uint8_t *m, size_t *mlen,
                                         const uint8_t *sm, size_t smlen,
                                         const uint8_t *pk);


#endif
