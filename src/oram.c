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


struct oram *oram_init(const int blk_size, const int group_size, const int group_count) {
	const int group_total = group_size * blk_size;
	struct storage dev = storage_init(total + 2(sqrt(total)));
	
	SHA256_CTX *ctx; 

	sha256_init(actual_oram -> sha_ctx); 
	uint8_t hash_val[HASH_LEN];
	for (int i = 0; i < group_count; i++) {
		crypto_hash(i, &ctx, hash_val);
		uint8_t buf1[sizeof(group_info)];
		struct group_info g_info = (struct group_info)buf1;
		g_info->state = state.old;
		g_info->idx = i;
		g_info->hash_val = hash_val;
		storage_write(dev, i*(group_total + sizeof(group_info)), sizeof(group_info), buf1);

		uint8_t buf2[blk_size * group_size];
		gen_random(buf2, blk_size * group_size);
		storage_write(dev, i*(group_total + sizeof(group_info)) + sizeof(group_info), buf2);

	}

	struct oram *actual_oram = malloc(sizeof(struct oram));
	actual_oram -> blk_size = blk_size;
	actual_oram -> group_size = group_size;
	actual_oram -> group_count = group_count;
	actual_oram -> dev = dev;
	actual_oram -> sha_ctx = ctx;
	return actual_oram;
}


int oram_access(const struct oram *oram, const int idx, const enum code, void *buffer) {
	if (code.equals(opcode.read)) {
		storage_read(oram->dev, idx*(oram->group_size * oram->blk_size + sizeof(group_info)), oram->group_size * oram->blk_size, &buffer);
	}
	if (code.equals(opcode.write)) {
		storage_write(oram->dev, idx*(oram->group_size * oram->blk_size + sizeof(group_info)), oram->group_size * oram->blk_size, &buffer);
	}
}

void gen_random(uint8_t *s, const int len) {
    static const char alphanum[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
}




