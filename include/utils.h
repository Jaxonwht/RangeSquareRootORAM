#ifndef UTILS_H
#define UTILS_H
#define HASH_LEN 32

#include <sha256.h>
#include <stdint.h>
#include <sys/time.h>

int mkdir_force(const char *dir_name);

int oram_encrypt(const void *data, int size, void *buf);

int oram_decrypt(const void *data, int size, void *buf);

void crypto_hash(int idx, SHA256_CTX *ctx, uint8_t *hash_val);

long timediffusec(struct timeval *tv1, struct timeval *tv2);

int two_power_ceiling(int n);

int get_rand(int low, int high);

#endif
