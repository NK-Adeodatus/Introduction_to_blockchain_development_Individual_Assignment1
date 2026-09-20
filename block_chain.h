#include <time.h>
#include "registry.h"
#include "crypto.h"
typedef struct Block {
    int index;
    time_t timestamp;
    char book_id[20];
    char book_title[80];
    char member_id[20];
    char member_name[50];
    char action[10]; // "borrow" or "return" or "overdue"

    char previous_hash[64];	
    char signature[72]; // Signature of the block data using the private key of the member
    char hash[65]; // Hash of the block data
} Block;

typedef struct Blockchain {
    Block *blocks;
    int num_blocks;
} Blockchain;

char *calculate_hash(const Block *block);
int is_book_on_loan(const char *book_id, const Blockchain *blockchain);
Block* find_outstanding_borrow(const char *book_id, const Blockchain *blockchain);
int add_block(Blockchain *blockchain, Book *book, Member *member, const char *action, const KeyPair *keypair);
int create_genesis_block(Blockchain *blockchain, const KeyPair *keypair);
int is_chain_valid(const Blockchain *blockchain, const KeyPair *keypair);
