#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <instruction.h>
#include <oram.h>
#include <storage.h>
#include <sha256.h>
#include <utils.h>

struct oram *oram_init(const int blk_size, const int group_size, const int group_count, const char *storage_file, const void *data) {
	const int count_sqrt = (int)sqrt(group_count);
	const int group_total = group_size * blk_size;
	const int all_total = group_total + sizeof(struct group_info);
	const int virtual_cnt = two_power_ceiling(group_count + 2 * count_sqrt);
	struct storage *dev = storage_init(virtual_cnt * all_total, storage_file);
	struct oram *actual_oram = malloc(sizeof(struct oram) + sizeof(struct oracle) * (group_count + count_sqrt));
	for (int i = 0; i < group_count + count_sqrt; i++) {
		struct oracle *const oracle = &actual_oram->oracles[i];
		SHA256_CTX *const ctx = &oracle->sha_ctx;	
		uint8_t *const hash = oracle->hash_val;
		sha256_init(ctx);
		uint8_t buf[all_total];
		struct group_info *g_info = (struct group_info *)buf;
		uint8_t *group_data = buf + sizeof(struct group_info);
		crypto_hash(i, ctx, g_info->hash_val);
		memcpy(hash, g_info->hash_val, HASH_LEN);
		g_info->state = UNCHANGED;
		g_info->idx = i;
		if (data && i < group_count) {
			memcpy(group_data, data + i * group_total, group_total);
		}
		storage_write(dev, i * all_total, all_total, buf);
	}
	actual_oram->blk_size = blk_size;
	actual_oram->group_size = group_size;
	actual_oram->group_count = group_count;
	actual_oram->dev = dev;
	actual_oram->virtual_count = virtual_cnt;
	oram_sort(actual_oram, compare_hash, 0, group_count + count_sqrt);
	actual_oram->shelter_count = 0;
	actual_oram->dummy_count = 0;
	return actual_oram;
}

static int oram_return(struct oram *oram) {
	const int total = oram->group_size * oram->blk_size + sizeof(struct group_info);
	const int group_count = oram->group_count;
	const int count_sqrt = (int)sqrt(group_count);
	oram_sort(oram, compare_restore, 0, group_count + 2 * count_sqrt);
	for (int i = 0; i < group_count + count_sqrt; i++) {
		struct group_info g_info;	
		storage_read(oram->dev, i * total, sizeof(struct group_info), &g_info);
		struct oracle *const oracle = &oram->oracles[i];
		crypto_hash(i, &oracle->sha_ctx, g_info.hash_val);
		memcpy(oracle->hash_val, g_info.hash_val, HASH_LEN);
		g_info.idx = i;
		g_info.state = UNCHANGED;
		storage_write(oram->dev, i * total, sizeof(struct group_info), &g_info);
	}
	for (int i = group_count + count_sqrt; i < group_count + count_sqrt * 2; i++) {
		struct group_info g_info;
		storage_read(oram->dev, i * total, sizeof(struct group_info), &g_info);
		g_info.state = UNCHANGED;
		storage_write(oram->dev, i * total, sizeof(struct group_info), &g_info);
	}
	oram_sort(oram, compare_hash, 0, group_count + count_sqrt);
	oram->dummy_count = 0;
	oram->shelter_count = 0;
	return 0;
}

static int binary_search(struct oram *oram, int idx, uint8_t *hash_val, int left, int right) {
	while (left <= right) {
		const int total = sizeof(struct group_info) + oram->blk_size * oram->group_size;
		const int m = left + ((right - left) >> 1);
		struct group_info g_info;
		storage_read(oram->dev, m * total, sizeof(struct group_info), &g_info);
		uint8_t *hash_val_temp = g_info.hash_val;
		if (memcmp(hash_val, hash_val_temp, HASH_LEN) == 0) {
			if (g_info.idx == idx) {
				return m;
			}
			int l = m;
			int r = m;
			while (l - 1 >= left) {
				l--;
				storage_read(oram->dev, l * total, sizeof(struct group_info), &g_info);
				hash_val = g_info.hash_val;
				if (memcmp(hash_val, hash_val_temp, HASH_LEN)) {
					break;
				} else {
					storage_read(oram->dev, l * total, sizeof(struct group_info), &g_info);
					if (g_info.idx == idx) {
						return l;
					}
				}
			}
			while (r + 1 <= right) {
				r++;
				storage_read(oram->dev, r * total, sizeof(struct group_info), &g_info);
				hash_val = g_info.hash_val;
				if (memcmp(hash_val, hash_val_temp, HASH_LEN)) {
					break;
				} else {
					storage_read(oram->dev, r * total, sizeof(struct group_info), &g_info);
					if (g_info.idx == idx) {
						return r;
					}
				}
			}
		} else if (memcmp(hash_val, hash_val_temp, HASH_LEN) < 0) {
			right = m - 1;
		} else {
			left = m + 1;
		}
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
			if (g_info->idx == idx && g_info->state == NEW) {
				memcpy(buffer, buf + sizeof(struct group_info), group_total);
				tag_found = 1;
			}
		}
		uint8_t buf2[total];
		if (tag_found == 1) {
			uint8_t *const hash_val_dummy = oram->oracles[group_count + oram->dummy_count].hash_val;
			int location = binary_search(oram, group_count + oram->dummy_count, hash_val_dummy, 0, group_count + count_sqrt - 1);
			if (location != -1) {
				storage_read(oram->dev, location * total, total, buf2);
			}
		} else {
			uint8_t *const hash_val = oram->oracles[idx].hash_val;
			int left = 0;
			int right = group_count + count_sqrt - 1;
			int dest_location = binary_search(oram, idx, hash_val, left, right);
			if (dest_location != -1) {
				storage_read(oram->dev, dest_location * total, total, buf2);
				memcpy(buffer, buf2 + sizeof(struct group_info), group_total);
				struct group_info *const g_info = (struct group_info *)buf2;
				g_info->state = INVALID;
				storage_write(oram->dev, dest_location * total, sizeof(struct group_info), g_info);
				g_info->state = NEW;
				storage_write(oram->dev, m_total + (count_sqrt + oram->shelter_count) * total, total, buf2);
				oram->shelter_count += 1;
			}
		}
		oram->dummy_count += 1;
		if (oram->dummy_count == count_sqrt) { 
			oram_return(oram);
		}
	} else if (code == WRITE) {
		uint8_t buf[total];
		int tag_found = 0;
		for (int i = group_count + count_sqrt; i < group_count + 2 * count_sqrt; i++) {
			storage_read(oram->dev, i * total, total, buf);
			struct group_info *g_info = (struct group_info *)buf;
			if (g_info->idx == idx && g_info->state == NEW) {
				storage_write(oram->dev, idx * total + sizeof(struct group_info), group_total, buffer);
				tag_found = 1;
			}
		}
		uint8_t buf2[total];
		if (tag_found == 1) {
			uint8_t *hash_val_dummy = oram->oracles[group_count + oram->dummy_count].hash_val;
			int location = binary_search(oram, group_count + oram->dummy_count, hash_val_dummy, 0, group_count + count_sqrt - 1);
			if (location != -1) {
				storage_read(oram->dev, location * total, total, buf2);
			}
		} else {
			uint8_t *const hash_val = oram->oracles[idx].hash_val;
			int dest_location = binary_search(oram, idx, hash_val, 0, group_count + count_sqrt - 1);
			if (dest_location != -1) {
				storage_read(oram->dev, dest_location * total, total, buf2);
				memcpy(buf2 + sizeof(struct group_info), buffer, group_total);
				struct group_info *const g_info = (struct group_info *)buf2;
				g_info->state = INVALID;
				storage_write(oram->dev, dest_location * total, sizeof(struct group_info), g_info);
				g_info->state = NEW;
				storage_write(oram->dev, m_total + (count_sqrt + oram->shelter_count) * total, total, buf2);
				oram->shelter_count += 1;
			}
		}
		oram->dummy_count += 1;
		if (oram->dummy_count == count_sqrt) { 
			oram_return(oram);
		}
	}
	/*
	uint8_t buf[total];
	for (int i = 0; i < 8; i++) {
		storage_read(oram->dev, i * total, total, buf);
		printf("group %d:\n", i);
		hex_dump(buf, total);
	}
	printf("\n");
	*/
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
