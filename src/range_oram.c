#include <oram.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

/*
 * Calculate the number of ORAMs needed. One oram for each depth.
 *
 * @param blk_count: total number of blocks
 *
 * @return number of depths/orams needed.
 */
static int calculate_depth(int blk_count)
{
	bool carry = false;
	int count = 0;
    if (blk_count == 1) {
        return 1;
    }
    while (blk_count > 0) {
        if (blk_count & 1) {
            carry = true;
        }
        count++;
        blk_count >>= 1;
    }
    if (carry) {
        count++;
    }
	return count;
}

/*
 * Initialize a tree-based read only range ORAM based on the square-root ORAM implemented for oram.h.
 *
 * @param blk_size: size of a block is bytes.
 * @param blk_count: total number of blocks.
 *
 * @return pointer to the initialized range_oram.
 */
struct range_oram *range_oram_init(int blk_size, int blk_count)
{
	const int depth = calculate_depth(blk_count);
	struct range_oram *const range_oram = malloc(sizeof(*range_oram) + depth);
	range_oram->blk_size = blk_size;
	range_oram->blk_count = blk_count;
	range_oram->depth = depth;
	for (int i = 0; i < depth; i++) {
		range_oram->oram_tree[i] = oram_init(blk_size, 1 << i, 1 << (depth - 1 - i));
	}
	return range_oram;
}

/* Access a block in the range ORAM.
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
	if (idx + blk_range > range_oram->blk_count) {
		return ENOMEM;
	}
	const int depth = calculate_depth(blk_range);
	const struct oram *const oram_active = range_oram->oram_tree[depth - 1];
	const int group_size = oram_active->group_size;
	const int blk_size = range_oram->blk_size;
	const int quotient_first = idx / group_size;
	const int remainder_first = idx % group_size;
	const int quotient_last = (idx + blk_range - 1) / group_size;
	const int remainder_last = (idx + blk_range - 1) % group_size;
	if (quotient_first == quotient_last) {
		uint8_t tmp_buf[blk_size * group_size];
		oram_access(oram_active, quotient_first, op, tmp_buf);
		memcpy(buffer, tmp_buf + remainder_first, blk_range * blk_size);
	} else {
		uint8_t tmp_buf[blk_size * group_size];
		oram_access(oram_active, quotient_first, op, tmp_buf);
		memcpy(buffer, tmp_buf + remainder_first, (group_size - remainder_first) * blk_size);
		oram_access(oram_active, quotient_last, op, tmp_buf);
		memcpy(buffer, tmp_buf, (remainder_last + 1) * blk_size);
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
