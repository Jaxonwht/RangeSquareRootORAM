#include <oram.h>

/*
 * Oblivious sort.
 *
 * @param oram: hanlder of the oram.
 * @param compare: a comparator of struct group_info.
 *				   < 0 if a < b,
 *				   0 if a == b,
 *				   > 0 if a > b.
 * 
 * @return 0 on success.
 * @return -1 on failure.
 */
int oram_sort(const struct oram *oram, group_comparator compare)
{
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
 * 
 * @return 0 on success.
 * @return -1 on failure.
 */
int oram_sort_improved(const struct oram *oram, group_comparator compare)
{
	return 0;
}
