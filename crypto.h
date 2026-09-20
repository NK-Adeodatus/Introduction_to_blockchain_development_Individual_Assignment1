

typedef struct KeyPair {
    char public_key[256];
    char private_key[256];
} KeyPair;

KeyPair generate_keypair();
int sign_block(const KeyPair *keypair, const Block *block, char *signature);