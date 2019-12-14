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

	oram_sort(actual_oram, compare_hash, 0, group_count + sqrt(group_count));
	return actual_oram;
}


int oram_access(const struct oram *oram, int idx, enum opcode code, void *buffer) {
	const int group_total = oram->group_size * oram->blk_size;
	const int m_total = (group_total + sizeof(struct group_info)) * group_count;
	const int total = group_total + sizeof(struct group_info);
	if (code == READ) {
		uint8_t buf[group_total + sizeof(struct group_info)];
		int tag_found = 0;
		
		for (int i = oram->group_count + sqrt(oram->group_count) + 1, i <= oram->group_count + 2*sqrt(oram->group_count); i++) {
			storage_read(oram->dev, i*(group_total + sizeof(struct group_info)), group_total + sizeof(struct group_info), buf);
			struct group_info *g_info = malloc(sizeof(struct group_info));
			memcpy(g_info, buf, sizeof(struct group_info));
			if(g_info->idx == idx) {
				buffer = buf;
				tag_found = 1;
			}
		}
		free(g_info);

		uint8_t buf2[group_total + sizeof(struct group_info)];
		if(tag_found == 1) {
			if (oram -> dummy_count > sqrt(oram->group_count))
				oram_return(oram);
			uint8_t hash_val_dummy[HASH_LEN];
			sha256_final(&oram->sha_ctx[oram->group_count + dummy_count], hash_val_dummy);
			int location = binary_search(hash_val_dummy, 0, group_count + sqrt(group_count) -1);
			if (location != -1) {
				storage_read(oram->dev, (location-1)*total, group_total + sizeof(struct group_info), buf2);
				dummy_count += 1;
			}
		} 
		else {
			uint8_t hash_val[HASH_LEN];
			sha256_final(&oram->sha_ctx[idx], hash_val);

			int left = 0; int right = group_count + sqrt(group_count) -1;
			int dest_location = binary_search(hash_val, left, right);
			
			if (dest_location != -1) {
				storage_read(oram->dev, (dest_location -1)*total, total, buf2);
				buffer = buf2;
				struct group_info *g_info = malloc(sizeof(struct group_info));
				memcpy(g_info, buf2, sizeof(struct group_info));
				g_info->state = updated;
				
				if (oram->shelter_count > sqrt(oram->group_count))
					oram_return(oram);	
				storage_write(oram->dev, m_total + sqrt(oram->group_count)*total+ oram->shelter_count*total, total, g_info);
				oram->shelter_count += 1;
				free(g_info);

			}
		
		}
	}
	if (code == WRITE) {
		// storage_write(oram->dev, idx * (group_total + sizeof(struct group_info)), group_total, buffer);
	}
	return 0;
}

int binary_search(uint8_t hash_val, int left, int right) {
	while (left <= right) {
				int m = left + (right - left) / 2;
				uint8_t hash_val_temp[HASH_LEN];
				sha256_final(&oram->sha_ctx[m], hash_val_temp);
				if (memcmp(hash_val, hash_val_temp, HASH_LEN) == 0) {
					return m;
				}
				if (memcmp(hash_val, hash_val_temp, HASH_LEN) < 0)
					left = m+1;
				else
					right = m-1;
			}
	return -1;		
}

int oram_return(struct oram *oram) {
	const int total = oram->group_size * oram->blk_size + sizeof(struct group_info);
	oram_sort(oram, compare_restore, 0, oram->group_count + 2*sqrt(oram->group_count));
	oram_sort(oram, compare_hash, 0, oram->group_count + sqrt(oram->group_count));
	oram->dummy_count = 0;
	oram->shelter_count = 0;
	uint8_t buf[sizeof(struct group_info)];
	for (int i = 0; i <= oram->group_count + 2*sqrt(group_count); i++) {
		storage_read(oram->dev, i*total, sizeof(struct group_info), buf);
		struct group_info *g_info;
		g_info = buf;
		g_info -> state = old;
		storage_write(oram->dev, i*total, sizeof(struct group_info), g_info);
	}
	return 0;
}


int oram_destroy(struct oram *oram)
{
	storage_destroy(oram->dev);
	free(oram);
	return 0;
}
