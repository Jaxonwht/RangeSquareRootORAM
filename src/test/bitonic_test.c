#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <utils.h>

enum dir {
	ASCENDING,
	DESCENDING
};

enum sort_tag {
	DUMMY,
	VALID
};

static void compareAndSwap_test(int i, int j, enum dir dir, int dummy[], enum sort_tag tags[])
{
	const enum sort_tag tag_i = tags[i];
	const enum sort_tag tag_j = tags[j];

	if ((tag_i == DUMMY && tag_j == DUMMY) || (dir == ASCENDING && tag_j == DUMMY) || (dir == DESCENDING && tag_i == DUMMY)) {
		return;
	}

	int a = dummy[i];
	int b = dummy[j];

	if ((dir == ASCENDING && (a > b || tag_i == DUMMY)) || (dir == DESCENDING && (a < b || tag_j == DUMMY))) {
		tags[i] = tag_j;
		tags[j] = tag_i;
		dummy[i] = b;
		dummy[j] = a;
	}
}

static void bitonicMerge_test(int lo, int cnt, enum dir dir, int dummy[], enum sort_tag tags[])
{
	if (cnt > 1) {
		const int k = (cnt >> 1);
		for (int i = lo; i < lo + k; i++) {
			compareAndSwap_test(i, i + k, dir, dummy, tags);
		}
		bitonicMerge_test(lo, k, dir, dummy, tags);
		bitonicMerge_test(lo + k, k, dir, dummy, tags);
	}
}

static void bitonicSort_test(int lo, int cnt, enum dir dir, int dummy[], enum sort_tag tags[])
{
	if (cnt > 1) {
		const int k = (cnt >> 1);
		bitonicSort_test(lo, k, ASCENDING, dummy, tags);
		bitonicSort_test(lo + k, k, DESCENDING, dummy, tags);
		bitonicMerge_test(lo, cnt, dir, dummy, tags);
	}
}

static int oram_sort_test()
{
	const int test[] = {2, 3, 1, 19, 10, 5, 8, 17, 9, 4, 7, 6, 6, 1, 3, 2, 10, 0, 0, 7};
	const int size = sizeof(test) / sizeof(test[0]);
	const int virtual_size = two_power_ceiling(size);
	int dummy[virtual_size];
	enum sort_tag tags[virtual_size];
	for (int i = 0; i < size; i++) {
		dummy[i] = test[i];
		tags[i] = VALID;
	}
	for (int i = size; i < virtual_size; i++) {
		tags[i] = DUMMY;
	}
	for (int q = 0; q < size; q++) {
		printf("%d ", test[q]);
	}
	printf("\n");
	bitonicSort_test(0, virtual_size, ASCENDING, dummy, tags);

	for (int q = 0; q < size; q++){
		printf("%d, %d\t", dummy[q], tags[q]);
	}
	printf("\n");
	return 0;
}

int main(void)
{
	oram_sort_test();
	return 0;
}
