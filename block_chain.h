#ifndef BLOCK_CHAIN_H
#define BLOCK_CHAIN_H

#include <time.h>
#include "registry.h"

// Forward declare KeyPair to avoid circular dependency
typedef struct KeyPair KeyPair;

typedef struct Block {
    int index;
    time_t timestamp;
    char book_id[20];
    char book_title[80];
    char member_id[20];
    char member_name[50];
    char action[10]; // "BORROWED", "RETURNED", "OVERDUE"
    char previous_hash[65];
    unsigned char signature[72]; // ECDSA digital signature
    int sig_len; // Actual length of the signature
    char hash[65]; // SHA-256 hash of all above fields combined
} Block;

typedef struct Blockchain {
    Block *blocks; // Dynamically allocated array of blocks
    int num_blocks;
} Blockchain;

// Function prototypes
char *calculate_hash(const Block *block);
int is_book_on_loan(const Blockchain *blockchain, const char *book_id);
Block* find_outstanding_borrow(const Blockchain *blockchain, const char *book_id);
int add_block(Blockchain *blockchain, const Book *book, const Member *member, const char *action, KeyPair *keypair);
int create_genesis_block(Blockchain *blockchain, KeyPair *keypair);
int is_chain_valid(const Blockchain *blockchain, KeyPair *keypair);
int save_blockchain(const Blockchain *blockchain, const char *filename);
int load_blockchain(Blockchain *blockchain, const char *filename);

#endif // BLOCK_CHAIN_H
