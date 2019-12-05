#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <instruction.h>
#include <oram.h>
#include <storage.h>
#include <sha256.h>
#include <utils.h>
#include "sha256.h"

static void gen_random(uint8_t *s, int len) {
	for (int i = 0; i < len; ++i) {
		s[i] = rand() & 0xFF;
	}
}

struct oram *oram_init(const int blk_size, const int group_size, const int group_count) {
	const int group_total = group_size * blk_size;
	struct storage *dev = storage_init(group_total + sqrt(group_total));
	struct oram *actual_oram = malloc(sizeof(struct oram) + sizeof(SHA256_CTX) * group_count);
	for (int i = 0; i < group_count; i++) {
		SHA256_CTX *const ctx = &actual_oram->sha_ctx[i];
		sha256_init(ctx);
		struct group_info g_info;
		crypto_hash(i, ctx, g_info.hash_val);
		g_info.state = old;
		g_info.idx = i;
		storage_write(dev, i * (group_total + sizeof(struct group_info)), sizeof(struct group_info), &g_info);

		uint8_t buf2[group_total];
		gen_random(buf2, group_total);
		storage_write(dev, i * (group_total + sizeof(struct group_info)) + sizeof(struct group_info), group_total, buf2);
	}

	actual_oram->blk_size = blk_size;
	actual_oram->group_size = group_size;
	actual_oram->group_count = group_count;
	actual_oram->dev = dev;
	return actual_oram;
}


int oram_access(const struct oram *oram, int idx, enum opcode code, void *buffer) {
	const int group_total = oram->group_size * oram->blk_size;
	if (code == READ) {
		storage_read(oram->dev, idx * (group_total + sizeof(struct group_info)), group_total, buffer);
	}
	if (code == WRITE) {
		storage_write(oram->dev, idx * (group_total + sizeof(struct group_info)), group_total, buffer);
	}
	return 0;
}

int oram_destroy(struct oram *oram)
{
	storage_destroy(oram->dev);
	free(oram);
	return 0;
}
