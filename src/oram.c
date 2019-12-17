#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <instruction.h>
#include <oram.h>
#include <storage.h>
#include <sha256.h>
#include <utils.h>

static void gen_random(uint8_t *s, int len) {
	for (int i = 0; i < len; ++i) {
		s[i] = (uint8_t)rand();
	}
}

struct oram *oram_init(const int blk_size, const int group_size, const int group_count, const char *storage_file) {
	const int count_sqrt = (int)sqrt(group_count);
	const int group_total = group_size * blk_size;
	const int all_total = group_total + sizeof(struct group_info);
	const int virtual_cnt = two_power_ceiling(group_count + 2 * count_sqrt);
	struct storage *dev = storage_init(virtual_cnt * all_total, storage_file);
	struct oram *actual_oram = malloc(sizeof(struct oram) + sizeof(SHA256_CTX) * group_count);
	for (int i = 0; i < group_count; i++) {
		SHA256_CTX *const ctx = &actual_oram->sha_ctx[i];
		sha256_init(ctx);
		struct group_info g_info;
		crypto_hash(i, ctx, g_info.hash_val);
		g_info.state = old;
		g_info.idx = i;
		storage_write(dev, i * all_total, sizeof(struct group_info), &g_info);

		uint8_t buf2[group_total];
		gen_random(buf2, group_total);
		storage_write(dev, i * all_total + sizeof(struct group_info), group_total, buf2);
	}

	actual_oram->blk_size = blk_size;
	actual_oram->group_size = group_size;
	actual_oram->group_count = group_count;
	actual_oram->dev = dev;
	actual_oram->virtual_count = virtual_cnt;
	oram_sort(actual_oram, compare_hash, 0, group_count + count_sqrt);
	return actual_oram;
}


static int oram_return(struct oram *oram) {
	const int total = oram->group_size * oram->blk_size + sizeof(struct group_info);
	const int group_count = oram->group_count;
	const int count_sqrt = (int)sqrt(group_count);
	oram_sort(oram, compare_restore, 0, group_count + 2 * count_sqrt);
	// TODO
	/* Update all the contexts */
	oram_sort(oram, compare_hash, 0, group_count + count_sqrt);
	oram->dummy_count = 0;
	oram->shelter_count = 0;
	struct group_info g_info;
	for (int i = 0; i < group_count + 2 * count_sqrt; i++) {
		storage_read(oram->dev, i * total, sizeof(struct group_info), &g_info);
		g_info.state = old;
		storage_write(oram->dev, i * total, sizeof(struct group_info), &g_info);
	}
	return 0;
}

static int binary_search(struct oram *oram, uint8_t *hash_val, int left, int right) {
	while (left <= right) {
		int m = left + ((right - left) >> 1);
		uint8_t hash_val_temp[HASH_LEN];
		sha256_final(&oram->sha_ctx[m], hash_val_temp);
		if (memcmp(hash_val, hash_val_temp, HASH_LEN) == 0)
			return m;
		if (memcmp(hash_val, hash_val_temp, HASH_LEN) < 0)
			left = m + 1;
		else
			right = m - 1;
	}
	return -1;
}

int oram_access(struct oram *oram, int idx, enum opcode code, void *buffer) {
	const int group_total = oram->group_size * oram->blk_size;
	const int total = group_total + sizeof(struct group_info);
	const int group_count = oram->group_count;
	const int count_sqrt = (int)sqrt(group_count);
	const int m_total = total * group_count;
	if (code == READ) {
		uint8_t buf[total];
		int tag_found = 0;
		for (int i = group_count + count_sqrt; i < group_count + 2 * count_sqrt; i++) {
			storage_read(oram->dev, i * total, total, buf);
			struct group_info *g_info = (struct group_info *)buf;
			if (g_info->idx == idx) {
				memcpy(buffer, buf + sizeof(struct group_info), group_total);
				tag_found = 1;
			}
		}
		uint8_t buf2[total];
		if (tag_found == 1) {
			if (oram->dummy_count > count_sqrt)
				oram_return(oram);
			uint8_t hash_val_dummy[HASH_LEN];
			sha256_final(&oram->sha_ctx[group_count + oram->dummy_count], hash_val_dummy);
			int location = binary_search(oram, hash_val_dummy, 0, group_count + count_sqrt - 1);
			if (location != -1) {
				storage_read(oram->dev, location * total, total, buf2);
				oram->dummy_count += 1;
			}
		} else {
			uint8_t hash_val[HASH_LEN];
			sha256_final(&oram->sha_ctx[idx], hash_val);
			int left = 0;
			int right = group_count + count_sqrt - 1;
			int dest_location = binary_search(oram, hash_val, left, right);
			if (dest_location != -1) {
				storage_read(oram->dev, dest_location *total, total, buf2);
				memcpy(buffer, buf2 + sizeof(struct group_info), group_total);
				buffer = buf2;
				struct group_info *const g_info = (struct group_info *)buf2;
				memcpy(g_info, buf2, sizeof(struct group_info));
				g_info->state = updated;
				if (oram->shelter_count > count_sqrt)
					oram_return(oram);
				storage_write(oram->dev, m_total + (count_sqrt + oram->shelter_count) * total, sizeof(struct group_info), &g_info);
				oram->shelter_count += 1;
			}

		}
	}
	if (code == WRITE) {
		// storage_write(oram->dev, idx * (group_total + sizeof(struct group_info)), group_total, buffer);
	}
	return 0;
}

/*
 * Destroy the oram and free relevant resources.
 *
 * @param oram: oram handler.
 *
 * @return 0 on success and -1 otherwise.
 */
int oram_destroy(struct oram *oram)
{
	storage_destroy(oram->dev);
	free(oram);
	return 0;
}

/*
 * Get the number of bytes of disk usage.
 *
 * @param oram: oram handler.
 *
 * @return size in bytes.
 */
unsigned long oram_used_memory(const struct oram *oram)
{
	return get_size(oram->dev);
}
