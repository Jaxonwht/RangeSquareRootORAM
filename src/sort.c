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

static void compareAndSwap(const int i, const int j, const enum dir dir, const struct oram *oram, group_comparator compare)
{
	struct group_info info_i;
	struct group_info info_j;
	const int group_size = oram->group_size;
	const int blk_size = oram->blk_size;
	const int myconst = group_size * blk_size + sizeof(struct group_info);
	uint8_t buf[sizeof(struct group_info)];
	storage_read(oram->dev, i * myconst, sizeof(struct group_info), buf);
	decrypt(buf, sizeof(struct group_info), &info_i);
	storage_read(oram->dev, j * myconst, sizeof(struct group_info), buf);
	decrypt(buf, sizeof(struct group_info), &info_j);
	const int compare_res = compare(&info_i, &info_j);
	if ((dir == ASCENDING && compare_res > 0) || (dir == DESCENDING && compare_res <= 0)) {
		encrypt(&info_j, sizeof(struct group_info), buf);
		storage_write(oram->dev, i * myconst, sizeof(struct group_info), buf);
		encrypt(&info_i, sizeof(struct group_info), buf);
		storage_write(oram->dev, j * myconst, sizeof(struct group_info), buf);
	}
}

static void bitonicMerge(const int lo, const int cnt, const enum dir dir, const struct oram *oram, group_comparator compare)
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

static void bitonicSort(const int lo, const int cnt, const enum dir dir, const struct oram *oram, group_comparator compare)
{
	if (cnt > 1) {
		const int k = cnt >> 2;
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
 * @param start_group: starting group index to be sorted.
 * @param end_group: ending group index to be sorted.
 * 
 * @return 0 on success.
 * @return -1 on failure.
 */
int oram_sort(const struct oram *oram, group_comparator compare, const int start_group, const int end_group)
{
    bitonicSort(start_group, end_group, ASCENDING, oram, compare);
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
 * @param end_group: ending group index to be sorted.
 * 
 * @return 0 on success.
 * @return -1 on failure.
 */
int oram_sort_improved(const struct oram *oram, group_comparator compare, const int start_group, const int end_group)
{
	return 0;
}
