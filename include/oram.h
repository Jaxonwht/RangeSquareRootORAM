#ifndef ORAM_H
#define ORAM_H

enum state {
	old,
	updated
};

enum opcode {
	write,
	read
};

struct blk_info {
	enum state state;
	int idx;
	void *hash_val;
};

struct oram {
	int blk_size;
	int group_size;
	int group_count;
	struct storage *dev;
};

struct range_oram {
	int blk_size;
	int blk_count;
	int depth;
	struct oram *oram_tree;
};

struct oram *oram_init(const int blk_size, const int group_size, const int group_count);

struct range_oram *range_oram_init(const int blk_size, const int blk_count);

int oram_access(const struct oram *oram, const int idx, const enum opcode, void *buffer);

int oram_shuffle(const struct oram *oram, int (*compare)(struct blk_info a, struct blk_info b));

int range_oram_access(const struct range_oram *range_oram, const int idx, const int blk_range, const enum opcode, void *buffer);

#endif
