#include <storage.h>
#include <libgen.h>
#include <stdio.h>
#include <stdint.h>

/*
 * Run storage tests.
 * test/storage_test file_path to start the test.
 */
int main(int argc, char *argv[])
{
	if (argc == 1) {
		printf("test/storage_test filename to start the test.\n");
		return 1;
	}
	const struct storage *const dev = storage_init(40, argv[1]);
	uint8_t buf[40];
	storage_read(dev, 0, 40, buf);
	hex_dump(buf, 10);
	uint8_t rand_buf[10];
	hex_dump(rand_buf, 10);
	storage_write(dev, 3, 10, rand_buf);
	storage_read(dev, 0, 13, buf);
	hex_dump(buf, 13);
	return 0;
}
