#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/obj_mac.h>
#include "crypto.h"

// Build a canonical buffer of fields to sign (everything BEFORE signature)
static unsigned char *build_signable_data(const Block *block, size_t *out_len)
{
    size_t len = sizeof(block->index) + sizeof(block->timestamp) +
                 sizeof(block->book_id) + sizeof(block->book_title) +
                 sizeof(block->member_id) + sizeof(block->member_name) +
                 sizeof(block->action) + sizeof(block->previous_hash);

    unsigned char *buf = malloc(len);
    if (!buf) return NULL;

    unsigned char *p = buf;
    memcpy(p, &block->index, sizeof(block->index)); p += sizeof(block->index);
    memcpy(p, &block->timestamp, sizeof(block->timestamp)); p += sizeof(block->timestamp);
    memcpy(p, block->book_id, sizeof(block->book_id)); p += sizeof(block->book_id);
    memcpy(p, block->book_title, sizeof(block->book_title)); p += sizeof(block->book_title);
    memcpy(p, block->member_id, sizeof(block->member_id)); p += sizeof(block->member_id);
    memcpy(p, block->member_name, sizeof(block->member_name)); p += sizeof(block->member_name);
    memcpy(p, block->action, sizeof(block->action)); p += sizeof(block->action);
    memcpy(p, block->previous_hash, sizeof(block->previous_hash));
    
    *out_len = len;
    return buf;
}

KeyPair *generate_keypair(void)
{
    KeyPair *kp = malloc(sizeof(KeyPair));
    if (!kp) return NULL;

    EVP_PKEY_CTX *pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL);
    if (!pctx) { free(kp); return NULL; }

    EVP_PKEY_keygen_init(pctx);
    EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pctx, NID_X9_62_prime256v1);

    kp->pkey = NULL;
    EVP_PKEY_keygen(pctx, &kp->pkey);
    EVP_PKEY_CTX_free(pctx);

    return kp;
}

int sign_block(Block *block, KeyPair *keypair)
{
    if (!block || !keypair || !keypair->pkey) return 0;

    size_t data_len;
    unsigned char *data = build_signable_data(block, &data_len);
    if (!data) return 0;

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (!mdctx) { free(data); return 0; }

    if (EVP_DigestSignInit(mdctx, NULL, EVP_sha256(), NULL, keypair->pkey) <= 0) {
        EVP_MD_CTX_free(mdctx); free(data); return 0;
    }

    size_t sig_len = 0;
    EVP_DigestSign(mdctx, NULL, &sig_len, data, data_len);

    if (sig_len > sizeof(block->signature)) {
        EVP_MD_CTX_free(mdctx); free(data); return 0;
    }

    if (EVP_DigestSign(mdctx, block->signature, &sig_len, data, data_len) <= 0) {
        EVP_MD_CTX_free(mdctx); free(data); return 0;
    }

    block->sig_len = (int)sig_len;

    EVP_MD_CTX_free(mdctx);
    free(data);
    return 1;
}

int verify_block_signature(const Block *block, KeyPair *keypair)
{
    if (!block || !keypair || !keypair->pkey) return 0;

    size_t data_len;
    unsigned char *data = build_signable_data(block, &data_len);
    if (!data) return 0;

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (!mdctx) { free(data); return 0; }

    if (EVP_DigestVerifyInit(mdctx, NULL, EVP_sha256(), NULL, keypair->pkey) <= 0) {
        EVP_MD_CTX_free(mdctx); free(data); return 0;
    }

    int ret = EVP_DigestVerify(mdctx, block->signature, block->sig_len, data, data_len);

    EVP_MD_CTX_free(mdctx);
    free(data);
    return (ret == 1);
}

void free_keypair(KeyPair *keypair)
{
    if (keypair) {
        if (keypair->pkey) EVP_PKEY_free(keypair->pkey);
        free(keypair);
    }
}