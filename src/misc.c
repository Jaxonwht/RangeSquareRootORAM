#include <utils.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>

/*
 * Get the time difference between two timestamps in microseconds.
 *
 * @param tv1: timestamp 1.
 * @param tv2: timestamp 2.
 *
 * @return difference in microseconds tv2 - tv1.
 */
long timediffusec(struct timeval *tv1, struct timeval *tv2)
{
	return (tv2->tv_sec - tv1->tv_sec) * 1000000 + (tv2->tv_usec - tv1->tv_usec);
}

/*
 * Get the ceiling powers of 2 of the input number.
 * 1 -> 1
 * 2 -> 2
 * 3-4 -> 4
 * 5-8 -> 8
 * @param: input number.
 *
 * @return ceiling power of 2.
 */
int two_power_ceiling(int n)
{
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

/*
 * Generate a random number between low and high - 1.
 *
 * @param low: lower bound.
 * @param high: upper bound + 1.
 *
 * @return a random int.
 */
int get_rand(int low, int high)
{
	return low + rand() % (high - low);
}

/*
 * Utility function to print data, of size bytes.
 *
 * @param data: data pointer.
 * @param size: number of bytes to print.
 */
void hex_dump(const void *data, int size)
{
	printf("%02hhx", ((uint8_t *)data)[0]);
	for (int i = 1; i < size; i++) {
		printf(" %02hhx", ((uint8_t *)data)[i]);
	}
	printf("\n");
}

/*
 * mkdir -p to create the data directory.
 *
 * @return 0 on success.
 * @return -1 on failure.
 */
int mkdir_force(const char *dir_name)
{
	int ret = mkdir(dir_name, 0755);
	if (ret != 0 && ret != EEXIST) {
		return -1;
	}
	else {
		return 0;
	}
}


