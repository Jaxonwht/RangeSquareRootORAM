#ifndef UTILS_H
#define UTILS_H
#define HASH_LEN 32

#include <sha256.h>
#include <stdint.h>

int encrypt(const void *data, int size, void *buf);

int decrypt(const void *data, int size, void *buf);

void crypto_hash(int idx, SHA256_CTX *ctx, uint8_t *hash_val);

#endif
