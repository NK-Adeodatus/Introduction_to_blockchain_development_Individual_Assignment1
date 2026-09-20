#ifndef CRYPTO_H
#define CRYPTO_H

#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/obj_mac.h>
#include "block_chain.h"

struct KeyPair {
    EVP_PKEY *pkey;
};

KeyPair *generate_keypair(void);
int sign_block(Block *block, KeyPair *keypair);
int verify_block_signature(const Block *block, KeyPair *keypair);
void free_keypair(KeyPair *keypair);

#endif // CRYPTO_H
