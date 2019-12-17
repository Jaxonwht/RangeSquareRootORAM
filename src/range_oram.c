#include <instruction.h>
#include <stdio.h>
#include <oram.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <string.h>

struct range_oram {
	int blk_size;
	int blk_count;
	int depth;
	struct oram *oram_tree[];
};

/*
 * Calculate the number of ORAMs needed. One oram for each depth.
 * 1 -> 1
 * 2 -> 2
 * 3-4 -> 3
 *
 * @param blk_count: total number of blocks
 *
 * @return number of depths/orams needed.
 */
static int calculate_depth(int blk_count)
{
	int carry = 0;
	int count = 0;
	while (blk_count > 0) {
		if ((blk_count & 1) && (blk_count != 1)) {
			carry = 1;
		}
		count++;
		blk_count >>= 1;
	}
	return count + carry;
}

/*
 * Calculate the index of the ORAM to access for a contiguous group of blk_count blocks.
 * 1 -> 0
 * 2 -> 1
 * 3-4 -> 2
 * 5-8 -> 3
 *
 * @param blk_count: number of contiguous blocks accessed.
 *
 * @return index of the accessed oram.
 */
static int active_oram_idx(int blk_count)
{
	int carry = 0;
	int count = -1;
	while (blk_count > 0) {
		if ((blk_count & 1) && (blk_count != 1)) {
			carry = 1;
		}
		count++;
		blk_count >>= 1;
	}
	return count + carry;
}

/*
 * Initialize a tree-based read only range ORAM based on the square-root ORAM implemented for oram.h.
 *
 * @param blk_size: size of a block is bytes.
 * @param blk_count: total number of blocks.
 * @param storage_folder: path to the storage folder.
 *
 * @return pointer to the initialized range_oram.
 */
struct range_oram *range_oram_init(int blk_size, int blk_count, const char *storage_folder)
{
	mkdir_force(storage_folder);
	char buf[strlen(storage_folder) + 1];
	strcpy(buf, storage_folder);
	if (buf[strlen(storage_folder) - 1] == '/') {
		buf[strlen(storage_folder) - 1] = '\0';
	}
	const int depth = calculate_depth(blk_count);
	struct range_oram *const range_oram = malloc(sizeof(*range_oram) + depth * sizeof(struct oram *));
	range_oram->blk_size = blk_size;
	range_oram->blk_count = blk_count;
	range_oram->depth = depth;
	char path_buf[255];
	for (int i = 0; i < depth; i++) {
		sprintf(path_buf, "%s/%09d.img", buf, i);
		range_oram->oram_tree[i] = oram_init(blk_size, 1 << i, 1 << (depth - 1 - i), path_buf);
	}
	return range_oram;
}

/* Access a block in the range ORAM. Currently only supports READ operation.
 *
 * @param range_oram: ptr to the range_oram handler.
 * @param idx: index of the starting block to be accessed.
 * @param blk_range: number of contiguous blocks to be accessed.
 * @param op: opcode of the operation. Currently only support READ.
 * @param buffer: memory pointer to receive the information to read to or write from.
 *
 * @return 0 on success and other numbers on failure.
 */
int range_oram_access(const struct range_oram *range_oram, int idx, int blk_range, enum opcode op, void *buffer)
{
	/* Range oram does not support write operations */
	if (op == WRITE) {
		return -1;
	}
	if (idx + blk_range > range_oram->blk_count) {
		return ENOMEM;
	}
	const int active_idx = active_oram_idx(blk_range);
	struct oram *const oram_active = range_oram->oram_tree[active_idx];
	const int group_size = oram_active->group_size;
	const int blk_size = range_oram->blk_size;
	const int group_size_byte = group_size * blk_size;
	const int quotient_first = idx / group_size;
	const int remainder_first = idx % group_size;
	const int quotient_last = (idx + blk_range - 1) / group_size;
	const int remainder_last = (idx + blk_range - 1) % group_size;
	const int padding = remainder_first * blk_size;
	if (quotient_first == quotient_last) {
		uint8_t tmp_buf[group_size_byte];
		oram_access(oram_active, quotient_first, op, tmp_buf);
		memcpy(buffer, tmp_buf + padding, blk_range * blk_size);
	} else {
		uint8_t tmp_buf[group_size_byte];
		oram_access(oram_active, quotient_first, op, tmp_buf);
		memcpy(buffer, tmp_buf + padding, group_size_byte - padding);
		oram_access(oram_active, quotient_last, op, tmp_buf);
		memcpy(buffer + group_size_byte - padding, tmp_buf, (remainder_last + 1) * blk_size);
	}
	return 0;
}

/*
 * Destroy the resources allocated for the range_oram.
 *
 * @param range_oram: ptr to the range ORAM.
 *
 * @return 0 on success and 1 on failure.
 */
int range_oram_destroy(struct range_oram *range_oram)
{
	for (int i = 0; i < range_oram->depth; i++) {
		oram_destroy(range_oram->oram_tree[i]);
	}
	free(range_oram);
	return 0;
}

/*
 * Get the number of bytes of disk usage.
 *
 * @param range_oram: range_oram handler.
 *
 * @return size in bytes.
 */
unsigned long range_oram_used_memory(const struct range_oram *range_oram)
{
	unsigned long sum = 0;
	for (int i = 0; i < range_oram->depth; i++) {
		sum += oram_used_memory(range_oram->oram_tree[i]);
	}
	return sum;
}
