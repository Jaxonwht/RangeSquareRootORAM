#include <oram.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <storage.h>

void compare(int i, int j, int dir, *oram, compare);
void bitonicMerge(int lo, int cnt, int dir, *oram, compare);
void bitonicSort(int lo, int cnt, int dir, *oram, compare);

const int ASCENDING = 1;
const int DESCENDING = 0;

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
    int N = oram->group_count;
    bitonicSort(0, N, ASCENDING, oram, compare);
	return 0;
}

void compare(int i, int j, int dir, const struct oram *oram, group_comparator compare)
{
  struct group_info info_i;
  struct group_info info_j;
  int group_size = oram->group_size;
  int blk_size = oram->blk_size;
  int myconst = group_size * blk_size + sizeof(group_info);
  int ret_1 = storage.read(oram->dev, i * myconst, sizeof(struct group_info), &info_i);
  int ret_2 = storage.read(oram->dev, j * myconst, sizeof(struct group_info), &info_j);

  if (dir==compare(info_i,info_j)){
    storage.write(oram->dev, i * myconst, sizeof(struct group_info), &info_j);
    storage.write(oram->dev, j * myconst, sizeof(struct group_info), &info_i);
  }

}

void bitonicSort(int lo, int cnt, int dir, const struct oram *oram, group_comparator compare)
{
  if (cnt>1)
    {
      int k=cnt/2;
      bitonicSort(lo, k, ASCENDING, oram, compare);
      bitonicSort(lo+k, k, DESCENDING, oram, compare);
      bitonicMerge(lo, cnt, dir, oram, compare);
    }
}

void bitonicMerge(int lo, int cnt, int dir, const struct oram *oram, group_comparator compare)
{
  if (cnt>1)
    {
      int k=cnt/2;
      int i;
      for (i=lo; i<lo+k; i++)
	compare(i, i+k, dir, oram, compare);
      bitonicMerge(lo, k, dir, oram, compare);
      bitonicMerge(lo+k, k, dir, oram, compare);
    }
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
