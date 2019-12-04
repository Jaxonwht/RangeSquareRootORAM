#include <utils.h>
#include <stdint.h>
#include <stdio.h>

/*
 * Test the SHA25 functions given by obscure third party.
 *
 */
int main(void)
{
	const int max = 10;
	const int repeat = 3;
	SHA256_CTX ctx[max];
	for (int i = 0; i < max; i++) {
		sha256_init(&ctx[i]);
	}
	for (int i = 0; i < repeat; i++) {
		for (int j = 0; j < max; j++) {
			printf("key is %d\n", j);
			const void *hash = crypto_hash(j, &ctx[j]);
			for (int k = 0; k < HASH_LEN; k++) {
				printf("%02x ", *(uint8_t *)(hash + k));
			}
			printf("\n");
		}
	}
	return 0;
}
