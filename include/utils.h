#ifndef UTILS_H
#define UTILS_H
#define HASH_LEN 32

int encrypt(const void *data, const int size, void *buf);

int decrypt(const void *data, const int size, void *buf);

void *crypto_hash(int idx, const int key);

#endif