#include <utils.h>
#include <string.h>
#include <stdlib.h>

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
 * @param key: key of hash function.
 *
 * @return void array representing the 256 bit hash value.
 */
void *crypto_hash(const int idx, const int key)
{
	void *hash_val = malloc(HASH_LEN);
	return hash_val;
}
