#include <stdio.h>

enum dir {
	ASCENDING,
	DESCENDING
};

int test[] = {1, 19, 10, 5, 8, 17, 9, 6, 6, 1, 3, 2, 10, 0, 0, 7};

static void compareAndSwap_test(int i, int j, enum dir dir)
{
	int a = test[i];
	int b = test[j];

	if ((dir == ASCENDING && a>b) || (dir == DESCENDING && a<b)){
		test[i] = b;
		test[j] = a;
	}
}

static void bitonicMerge_test(int lo, int cnt, const enum dir dir)
{
	if (cnt > 1) {
		const int k = (cnt >> 1);
		for (int i= lo; i < lo + k; i++) {
			compareAndSwap_test(i, i + k, dir);
		}
		bitonicMerge_test(lo, k, dir);
		bitonicMerge_test(lo + k, k, dir);
	}
}

static void bitonicSort_test(int lo, int cnt, const enum dir dir)
{
	if (cnt > 1) {
		const int k = (cnt >> 1);
		bitonicSort_test(lo, k, ASCENDING);
		bitonicSort_test(lo + k, k, DESCENDING);
		bitonicMerge_test(lo, cnt, dir);
	}
}


static int oram_sort_test()
{
	const int size = sizeof(test) / sizeof(test[0]);
	for (int q = 0; q < size; q++){
		printf("%d ", test[q]);
	}
	printf("\n");
	bitonicSort_test(0, size, ASCENDING);

	for (int q = 0; q < size; q++){
		printf("%d ", test[q]);
	}
	printf("\n");
	return 0;
}

int main(void)
{
	oram_sort_test();
	return 0;
}