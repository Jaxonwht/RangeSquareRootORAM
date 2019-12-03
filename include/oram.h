#ifndef ORAM_H
#define ORAM_H

#include <instruction.h>
#include <string.h>
#include <utils.h>

enum state {
	old,
	updated
};

struct group_info {
	enum state state;
	int idx;
	void *hash_val;
};

struct oram {
	int blk_size;
	int group_size;
	int group_count;
	struct group_info *group_info;
	struct storage *dev;
};

struct range_oram {
	int blk_size;
	int blk_count;
	int depth;
	struct oram *oram_tree;
};

inline int compare_hash(struct group_info *a, struct group_info *b)
{
	return memcmp(a->hash_val, b->hash_val, HASH_LEN);
}

inline int compare_restore(struct group_info *a, struct group_info *b)
{
	if (a->state == old && b->state == updated && a->idx == b->idx) {
		return 1;
	}
	if (a->state == updated && b->state == old && a->idx == b->idx) {
		return -1;
	}
	return a->idx - b->idx;
}

typedef int (*group_comparator)(const struct group_info *a, const struct group_info *b);

struct oram *oram_init(const int blk_size, const int group_size, const int group_count);

struct range_oram *range_oram_init(const int blk_size, const int blk_count);

int oram_access(const struct oram *oram, const int idx, const enum opcode, void *buffer);

int oram_sort(const struct oram *oram, group_comparator compare);
int oram_sort_improved(const struct oram *oram, group_comparator compare);

int range_oram_access(const struct range_oram *range_oram, const int idx, const int blk_range, const enum opcode, void *buffer);

#endif
