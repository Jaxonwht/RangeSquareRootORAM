#include <oram.h>
#include <stdio.h>
#include <utils.h>
#include <stdint.h>
#include <stdlib.h>

/*
 * Actually test the read and write of oram and range_oram.
 */
int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("Usage: test/oram_test storage_path\n");
		return 1;
	}
	const int blk_size = 4;
	const int group_size = 1;
	const int group_count = 16;
	struct oram *const oram = oram_init(blk_size, group_size, group_count, argv[1], NULL, 0);
	uint8_t buf[16];
	for (int i = 0; i < 16; i++) {
		buf[i] = (uint8_t)rand();
	}
	hex_dump(buf, 16);
	for (int i = 0; i < 4; i++) {
		oram_access(oram, i, WRITE, buf + 4 * i);
	}
	uint8_t buf2[16];
	for (int i = 0; i < 4; i++) {
		oram_access(oram, i, READ, buf2 + 4 * i);
	}
	hex_dump(buf2, 16);
	printf("Read and write buffers comparison %d\n", memcmp(buf, buf2, 16));
	oram_destroy(oram);

	uint8_t data[blk_size * group_count];
	for (int i = 0; i < blk_size * group_count; i++) {
		data[i] = (uint8_t)rand();
	}
	printf("The prepopulating data of %d bytes is:\n", blk_size * group_count);
	hex_dump(data, blk_size * group_count);
	struct range_oram *range_oram = range_oram_init(blk_size, group_count, argv[1], data, blk_size * group_count);
	uint8_t buf3[10];
	for (int i = 1; i < 10; i++) {
		range_oram_access(range_oram, 3, i, READ, buf3);
		printf("First %d blocks starting from block 3 are:\n", i);
		hex_dump(buf3, i * 4);
	}
	range_oram_destroy(range_oram);
	return 0;
}
