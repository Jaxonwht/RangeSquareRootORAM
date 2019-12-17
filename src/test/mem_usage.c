#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <utils.h>
#include <stdlib.h>

/*
 * This file tests the memory usage function in utils.h
 */
int main(int argc, char *argv[])
{
	for (int i = 0; i < 10; i++) {
		int *dummy = malloc(10000000 * sizeof(*dummy));
		printf("Total memory usage of the current process is %lu KiB (1024 bytes).\n", get_memory_usage());
	}
	return 0;
}
