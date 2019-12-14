#ifndef ORAM_H
#define ORAM_H

#include <instruction.h>
#include <string.h>
#include <utils.h>
#include <sha256.h>
#include <stdint.h>

enum state {
	old,
	updated
};

struct group_info {
	enum state state;
	int idx;
	uint8_t hash_val[HASH_LEN];
};

struct oram {
	int blk_size;
	int group_size;
	int group_count;
	int dummy_count;
	int shelter_count;
	struct storage *dev;
	SHA256_CTX sha_ctx[];
};

struct range_oram {
	int blk_size;
	int blk_count;
	int depth;
	struct oram *oram_tree[];
};


int compare_hash(const struct group_info *a, const struct group_info *b);

int compare_restore(const struct group_info *a, const struct group_info *b);

typedef int (*group_comparator)(const struct group_info *a, const struct group_info *b);

struct oram *oram_init(int blk_size, int group_size, int group_count);

struct range_oram *range_oram_init(int blk_size, int blk_count);

int oram_access(const struct oram *oram, int idx, enum opcode, void *buffer);

int oram_sort(const struct oram *oram, group_comparator compare, int start_group, int end_group);
int oram_sort_improved(const struct oram *oram, group_comparator compare, int start_group, int end_group);

int range_oram_access(const struct range_oram *range_oram, int idx, int blk_range, enum opcode op, void *buffer);

int oram_destroy(struct oram *oram);
int range_oram_destroy(struct range_oram *range_oram);

#endif
