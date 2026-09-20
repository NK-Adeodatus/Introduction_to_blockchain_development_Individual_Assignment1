#include <stdio.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include "block_chain.h"

/**
 * calculate_hash - compute the SHA-256 hash of a block's fields
 * @block: pointer to the block whose fields will be hashed
 *
 * Return: pointer to a heap-allocated 65-byte hex string (64 chars +
 *         null terminator), or NULL on failure
 */
char *calculate_hash(Block *block)
{
	EVP_MD_CTX *ctx;
	unsigned char digest[EVP_MAX_MD_SIZE];
	unsigned int digest_len, i;
	char *hash_str;

	if (block == NULL)
		return (NULL);

	ctx = EVP_MD_CTX_new();
	if (ctx == NULL)
		return (NULL);

	if (EVP_DigestInit_ex(ctx, EVP_sha256(), NULL) != 1 ||
	    EVP_DigestUpdate(ctx, &block->index, sizeof(block->index)) != 1 ||
	    EVP_DigestUpdate(ctx, &block->timestamp, sizeof(block->timestamp)) != 1 ||
	    EVP_DigestUpdate(ctx, block->book_id, sizeof(block->book_id)) != 1 ||
	    EVP_DigestUpdate(ctx, block->book_title, sizeof(block->book_title)) != 1 ||
	    EVP_DigestUpdate(ctx, block->member_id, sizeof(block->member_id)) != 1 ||
	    EVP_DigestUpdate(ctx, block->member_name, sizeof(block->member_name)) != 1 ||
	    EVP_DigestUpdate(ctx, block->action, sizeof(block->action)) != 1 ||
	    EVP_DigestUpdate(ctx, block->previous_hash, sizeof(block->previous_hash)) != 1 ||
	    EVP_DigestUpdate(ctx, block->signature, sizeof(block->signature)) != 1 ||
	    EVP_DigestFinal_ex(ctx, digest, &digest_len) != 1)
	{
		EVP_MD_CTX_free(ctx);
		return (NULL);
	}

	EVP_MD_CTX_free(ctx);

	hash_str = malloc((digest_len * 2) + 1);
	if (hash_str == NULL)
		return (NULL);

	for (i = 0; i < digest_len; i++)
		sprintf(hash_str + (i * 2), "%02x", digest[i]);

	return (hash_str);
}

int is_book_on_loan(const char *book_id, const Blockchain *blockchain)
{
    if (book_id == NULL || blockchain == NULL)
        return 0;

    for (int i = 0; i < blockchain->num_blocks; i++) {
        Block *block = &blockchain->blocks[i];
        if (strcmp(block->book_id, book_id) == 0 && strcmp(block->action, "borrow") == 0) {
            return 1; // Book is currently on loan
        }
    }
    return 0; // Book is not on loan
}

Block* find_outstanding_borrow(const char *book_id, const Blockchain *blockchain)
{
    if (book_id == NULL || blockchain == NULL)
        return NULL;

    for (int i = 0; i < blockchain->num_blocks; i++) {
        Block *block = &blockchain->blocks[i];
        if (strcmp(block->book_id, book_id) == 0 && strcmp(block->action, "borrow") == 0) {
            return block; // Found the outstanding borrow block
        }
    }
    return NULL; // No outstanding borrow found
}

int add_block(Blockchain *blockchain, Book *book, Member *member, const char *action, const KeyPair *keypair)
{
    if (blockchain == NULL || book == NULL || member == NULL || action == NULL || keypair == NULL)
        return -1;

    // Allocate memory for the new block
    Block *new_block = realloc(blockchain->blocks, sizeof(Block) * (blockchain->num_blocks + 1));
    if (new_block == NULL)
        return -1; // Memory allocation failed

    blockchain->blocks = new_block;
    Block *block = &blockchain->blocks[blockchain->num_blocks];

    // Fill in the block details
    block->index = blockchain->num_blocks;
    block->timestamp = time(NULL);
    strncpy(block->book_id, book->book_id, sizeof(block->book_id));
    strncpy(block->book_title, book->title, sizeof(block->book_title));
    strncpy(block->member_id, member->member_id, sizeof(block->member_id));
    strncpy(block->member_name, member->full_name, sizeof(block->member_name));
    strncpy(block->action, action, sizeof(block->action));

    // Set previous hash
    if (blockchain->num_blocks > 0) {
        strncpy(block->previous_hash, blockchain->blocks[blockchain->num_blocks - 1].hash, sizeof(block->previous_hash));
    } else {
        memset(block->previous_hash, 0, sizeof(block->previous_hash)); // Genesis block
    }

    // Calculate the hash of the block
    char *hash = calculate_hash(block);
    if (hash == NULL)
        return -1; // Hash calculation failed

    strncpy(block->hash, hash, sizeof(block->hash));
    free(hash);

    // Sign the block
    if (sign_block(keypair, block, block->signature) != 0)
        return -1; // Signing failed

    blockchain->num_blocks++;
    return 0; // Success
}

int create_genesis_block(Blockchain *blockchain, const KeyPair *keypair)
{
    if (blockchain == NULL || keypair == NULL)
        return -1;

    // Allocate memory for the genesis block
    blockchain->blocks = malloc(sizeof(Block));
    if (blockchain->blocks == NULL)
        return -1; // Memory allocation failed

    Block *genesis_block = &blockchain->blocks[0];
    genesis_block->index = 0;
    genesis_block->timestamp = time(NULL);
    memset(genesis_block->book_id, 0, sizeof(genesis_block->book_id));
    memset(genesis_block->book_title, 0, sizeof(genesis_block->book_title));
    memset(genesis_block->member_id, 0, sizeof(genesis_block->member_id));
    memset(genesis_block->member_name, 0, sizeof(genesis_block->member_name));
    strncpy(genesis_block->action, "genesis", sizeof(genesis_block->action));
    memset(genesis_block->previous_hash, 0, sizeof(genesis_block->previous_hash));

    // Calculate the hash of the genesis block
    char *hash = calculate_hash(genesis_block);
    if (hash == NULL)
        return -1; // Hash calculation failed

    strncpy(genesis_block->hash, hash, sizeof(genesis_block->hash));
    free(hash);

    // Sign the genesis block
    if (sign_block(keypair, genesis_block, genesis_block->signature) != 0)
        return -1; // Signing failed

    blockchain->num_blocks = 1;
    return 0; // Success
}

int is_chain_valid(const Blockchain *blockchain, const KeyPair *keypair)
{
    if (blockchain == NULL || keypair == NULL)
        return 0;

    for (int i = 0; i < blockchain->num_blocks; i++) {
        Block *block = &blockchain->blocks[i];

        // Verify the block's hash
        char *calculated_hash = calculate_hash(block);
        if (calculated_hash == NULL || strcmp(calculated_hash, block->hash) != 0) {
            free(calculated_hash);
            return 0; // Invalid hash
        }
        free(calculated_hash);

        // Verify the block's signature
        if (verify_signature(keypair, block, block->signature) != 1) {
            return 0; // Invalid signature
        }

        // Verify the previous hash
        if (i > 0 && strcmp(block->previous_hash, blockchain->blocks[i - 1].hash) != 0) {
            return 0; // Invalid previous hash
        }
    }
    return 1; // Chain is valid
}

