#include <oram.h>
#include <utils.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include <storage.h>

enum dir {
	ASCENDING,
	DESCENDING
};

static void compareAndSwap(int i, int j, enum dir dir, const struct oram *oram, group_comparator compare)
{
	struct group_info info_i;
	struct group_info info_j;

	const int group_size = oram->group_size;
	const int blk_size = oram->blk_size;
	const int data_size = group_size * blk_size;
	const int myconst = data_size + sizeof(struct group_info);

	uint8_t data_i[data_size];
	uint8_t data_j[data_size];

	storage_read(oram->dev, i * myconst, sizeof(struct group_info), &info_i);
	storage_read(oram->dev, j * myconst, sizeof(struct group_info), &info_j);
	storage_read(oram->dev, i * myconst + sizeof(struct group_info), data_size, data_i);
	storage_read(oram->dev, j * myconst + sizeof(struct group_info), data_size, data_j);

	const int compare_res = compare(&info_i, &info_j);
	if ((dir == ASCENDING && compare_res > 0) || (dir == DESCENDING && compare_res < 0)) {
		storage_write(oram->dev, i * myconst, sizeof(struct group_info), &info_j);
		storage_write(oram->dev, j * myconst, sizeof(struct group_info), &info_i);

		storage_write(oram->dev, i * myconst + sizeof(struct group_info), data_size, data_j);
		storage_write(oram->dev, j * myconst + sizeof(struct group_info), data_size, data_i);
	}
}

static void bitonicMerge(int lo, int cnt, const enum dir dir, const struct oram *oram, group_comparator compare)
{
	if (cnt > 1) {
		const int k = cnt >> 1;
		for (int i= lo; i < lo + k; i++) {
			compareAndSwap(i, i + k, dir, oram, compare);
		}
		bitonicMerge(lo, k, dir, oram, compare);
		bitonicMerge(lo + k, k, dir, oram, compare);
	}
}

static void bitonicSort(int lo, int cnt, const enum dir dir, const struct oram *oram, group_comparator compare)
{
	if (cnt > 1) {
		const int k = cnt >> 1;
		bitonicSort(lo, k, ASCENDING, oram, compare);
		bitonicSort(lo + k, k, DESCENDING, oram, compare);
		bitonicMerge(lo, cnt, dir, oram, compare);
	}
}

/*
 * Oblivious sort.
 *
 * @param oram: hanlder of the oram.
 * @param compare: a comparator of struct group_info.
 *				   < 0 if a < b,
 *				   0 if a == b,
 *				   > 0 if a > b.
 * @param start_group: starting group index to be sorted, inclusive.
 * @param group_count: number of groups starting from start_group to be sorted.
 *
 * @return 0 on success.
 * @return -1 on failure.
 */
int oram_sort(const struct oram *oram, group_comparator compare, int start_group, int group_count)
{
	struct group_info g_info;
	for (int i = 0; i < oram->virtual_count; i++) {
		storage_read(oram->dev, i * (oram->group_size * oram->blk_size + sizeof(struct group_info)), sizeof(struct group_info), &g_info);
		if (i >= start_group && i < start_group + group_count) {
			g_info.sort_tag = valid;
		} else {
			g_info.sort_tag = dummy;
		}
		storage_write(oram->dev, i * (oram->group_size * oram->blk_size + sizeof(struct group_info)), sizeof(struct group_info), &g_info);
	}
	bitonicSort(0, oram->virtual_count, ASCENDING, oram, compare);
	return 0;
}

/*
 * Improved oblivious sort.
 *
 * @param oram: hanlder of the oram.
 * @param compare: a comparator of struct group_info.
 *				   < 0 if a < b,
 *				   0 if a == b,
 *				   > 0 if a > b.
 * @param start_group: starting group index to be sorted.
 * @param group_count: number of groups starting from start_group to be sorted.
 *
 * @return 0 on success.
 * @return -1 on failure.
 */
int oram_sort_improved(const struct oram *oram, group_comparator compare, int start_group, int group_count)
{
	return 0;
}

/*
 * Comparator that compares by hash_value byte by byte.
 *
 * @param a: group_info a.
 * @param b: group_info b.
 *
 * @return < 0 if a < b,
 *		   = 0 if a == b,
 *		   > 0 if a > b.
 */
int compare_hash(const struct group_info *a, const struct group_info *b)
{
	if (a->sort_tag == dummy && b->sort_tag == dummy) {
		return 0;
	}
	if (a->sort_tag == dummy) {
		return 1;
	}
	if (b->sort_tag == dummy) {
		return -1;
	}
	return memcmp(a->hash_val, b->hash_val, HASH_LEN);
}

/*
 * Comparator that compares by original index. If the two info have the same original index, compare their states.
 *
 * @param a: group_info a.
 * @param b: group_info b.
 *
 * @return < 0 if a < b,
 *		   = 0 if a == b,
 *		   > 0 if a > b.
 */
int compare_restore(const struct group_info *a, const struct group_info *b)
{

	if (a->sort_tag == dummy && b->sort_tag == dummy) {
		return 0;
	}
	if (a->sort_tag == dummy) {
		return 1;
	}
	if (b->sort_tag == dummy) {
		return -1;
	}
	if (a->idx != b->idx) {
		return a->idx - b->idx;
	}
	if (a->state == old && b->state == updated) {
		return 1;
	}
	if (a->state == updated && b->state == old) {
		return -1;
	}
	return 0;
}
