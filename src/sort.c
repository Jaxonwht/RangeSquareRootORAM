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

enum sort_tag {
	VALID,
	DUMMY
};

static void compareAndSwap(int i, int j, enum dir dir, const struct oram *oram, group_comparator compare, enum sort_tag tags[])
{
	const enum sort_tag tag_i = tags[i];
	const enum sort_tag tag_j = tags[j];

	if ((tag_i == DUMMY && tag_j == DUMMY ) || (dir == ASCENDING && tag_j == DUMMY) || (dir == DESCENDING && tag_i == DUMMY)) {
		return;
	}

	const int group_size = oram->group_size;
	const int blk_size = oram->blk_size;
	const int data_size = group_size * blk_size;
	const int myconst = data_size + sizeof(struct group_info);

	uint8_t buf_i[myconst];
	uint8_t buf_j[myconst];

	struct group_info *info_i = (struct group_info *)buf_i;
	struct group_info *info_j = (struct group_info *)buf_j;

	uint8_t *data_i = buf_i + sizeof(struct group_info);
	uint8_t *data_j = buf_j + sizeof(struct group_info);

	storage_read(oram->dev, i * myconst, myconst, buf_i);
	storage_read(oram->dev, j * myconst, myconst, buf_j);

	const int compare_res = compare(info_i, info_j);
	if ((dir == ASCENDING && (compare_res > 0 || tag_i == DUMMY)) || (dir == DESCENDING && (compare_res < 0 || tag_j == DUMMY))) {
		tags[i] = tag_j;
		tags[j] = tag_i;
		storage_write(oram->dev, i * myconst, sizeof(struct group_info), info_j);
		storage_write(oram->dev, j * myconst, sizeof(struct group_info), info_i);
		storage_write(oram->dev, i * myconst + sizeof(struct group_info), data_size, data_j);
		storage_write(oram->dev, j * myconst + sizeof(struct group_info), data_size, data_i);
	}
}

static void bitonicMerge(int lo, int cnt, const enum dir dir, const struct oram *oram, group_comparator compare, enum sort_tag tags[])
{
	if (cnt > 1) {
		const int k = cnt >> 1;
		for (int i= lo; i < lo + k; i++) {
			compareAndSwap(i, i + k, dir, oram, compare, tags);
		}
		bitonicMerge(lo, k, dir, oram, compare, tags);
		bitonicMerge(lo + k, k, dir, oram, compare, tags);
	}
}

static void bitonicSort(int lo, int cnt, const enum dir dir, const struct oram *oram, group_comparator compare, enum sort_tag tags[])
{
	if (cnt > 1) {
		const int k = cnt >> 1;
		bitonicSort(lo, k, ASCENDING, oram, compare, tags);
		bitonicSort(lo + k, k, DESCENDING, oram, compare, tags);
		bitonicMerge(lo, cnt, dir, oram, compare, tags);
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
	enum sort_tag tags[oram->virtual_count];
	for (int i = 0; i < oram->virtual_count; i++) {
		if (i >= start_group && i < start_group + group_count) {
			tags[i] = VALID;
		} else {
			tags[i] = DUMMY;
		}
	}
	bitonicSort(0, oram->virtual_count, ASCENDING, oram, compare, tags);
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
	switch (a->state) {
	case INVALID:
		switch (b->state) {
		case INVALID:
			return 0;
		case UNCHANGED:
		case NEW:
			return 1;	
		}
	case UNCHANGED:
		switch (b->state) {
		case INVALID:
			return -1;
		case UNCHANGED:
		case NEW:
			return a->idx - b->idx;
		}
	case NEW:
		switch (b->state) {
		case INVALID:
			return -1;
		case UNCHANGED:
		case NEW:
			return a->idx - b->idx;
		}
	}
	return 0;
}
