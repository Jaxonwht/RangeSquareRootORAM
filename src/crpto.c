#include <utils.h>
#include <string.h>
#include <stdlib.h>
#include <sha256.h>

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
int encrypt(const void *data, const int size, void *buf)
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
int decrypt(const void *data, const int size, void *buf)
{
	memcpy(buf, data, size);
	return 0;
}

/*
 * Generate a 256 hash using SHA256.
 *
 * @param idx: index of the block to calculate hash value.
 * @param ctx: SHA256 context vector so far.
 *
 * @return void array representing the 256 bit hash value.
 */
void *crypto_hash(int idx, SHA256_CTX *const ctx)
{
	void *hash_val = malloc(HASH_LEN);
	BYTE text[4];
	for (int i = 0; i < 4; i++) {
		text[i] = idx & 0xFF;
		idx >>= 8;
	}
	sha256_update(ctx, text, 4);
	sha256_final(ctx, hash_val);
	return hash_val;
}
