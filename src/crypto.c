#include <utils.h>
#include <string.h>
#include <stdlib.h>
#include <sha256.h>
#include <stdint.h>

/*
 * Dummy encrypt function that does not modify content.
 *
 * @param data: data to be encrypted.
 * @param size: size in bytes to be encrypted.
 * @param buf: buffer to hold encrypted data.
 *
 * @return 0 on success.
 * @return other numbers on failure.
 */
int encrypt(const void *data, int size, void *buf)
{
	memcpy(buf, data, size);
	return 0;
}

/*
 * Dummy decrypt function that does not modify content.
 *
 * @param data: data to be decrypted.
 * @param size: size in bytes to be decrypted.
 * @param buf: buffer to hold decrypted data.
 *
 * @return 0.
 * @return other numbers on failure.
 */
int decrypt(const void *data, int size, void *buf)
{
	memcpy(buf, data, size);
	return 0;
}

/*
 * Generate a 256 hash using SHA256.
 *
 * @param idx: index of the block to calculate hash value.
 * @param ctx: SHA256 context vector so far.
 * @param hash_val: 32 byte buffer that stores the resulting hash value.
 */
void crypto_hash(int idx, SHA256_CTX *ctx, uint8_t *hash_val)
{
	uint8_t text[4];
	for (int i = 0; i < 4; i++) {
		text[i] = idx & 0xFF;
		idx >>= 8;
	}
	sha256_update(ctx, text, 4);
	sha256_final(ctx, hash_val);
}
