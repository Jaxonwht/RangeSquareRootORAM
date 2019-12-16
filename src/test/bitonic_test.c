#include <stdio.h>
#include <math.h>
#include <stdint.h>

enum dir {
	ASCENDING,
	DESCENDING
};

static void compareAndSwap_test(int i, int j, enum dir dir, int dummy[])
{
	int a = dummy[i];
	int b = dummy[j];

	if ((dir == ASCENDING && a > b) || (dir == DESCENDING && a < b)) {
		dummy[i] = b;
		dummy[j] = a;
	}
}

static void bitonicMerge_test(int lo, int cnt, enum dir dir, int dummy[])
{
	if (cnt > 1) {
		const int k = (cnt >> 1);
		for (int i = lo; i < lo + k; i++) {
			compareAndSwap_test(i, i + k, dir, dummy);
		}
		bitonicMerge_test(lo, k, dir, dummy);
		bitonicMerge_test(lo + k, k, dir, dummy);
	}
}

static void bitonicSort_test(int lo, int cnt, enum dir dir, int dummy[])
{
	if (cnt > 1) {
		const int k = (cnt >> 1);
		bitonicSort_test(lo, k, ASCENDING, dummy);
		bitonicSort_test(lo + k, k, DESCENDING, dummy);
		bitonicMerge_test(lo, cnt, dir, dummy);
	}
}

static int get_virtual_size(int n) {
	int count = -1;
	int carry = 0;
	while (n > 0) {
		if ((n & 1) && (n != 1)) {
			carry = 1;
		}
		n >>= 1;
		count++;
	}
	return 1 << (count + carry);
}

static int oram_sort_test()
{
	const int test[] = {2, 3, 1, 19, 10, 5, 8, 17, 9, 4, 7, 6, 6, 1, 3, 2, 10, 0, 0, 7};
	const int size = sizeof(test) / sizeof(test[0]);
	const int virtual_size = get_virtual_size(size);
	int dummy[virtual_size];
	for (int i = 0; i < size; i++) {
		dummy[i] = test[i];
	}
	for (int i = size; i < virtual_size; i++) {
		dummy[i] = INT32_MAX;
	}
	for (int q = 0; q < size; q++) {
		printf("%d ", test[q]);
	}
	printf("\n");
	bitonicSort_test(0, virtual_size, ASCENDING, dummy);

	for (int q = 0; q < size; q++){
		printf("%d ", dummy[q]);
	}
	printf("\n");
	return 0;
}

int main(void)
{
	oram_sort_test();
	return 0;
}
