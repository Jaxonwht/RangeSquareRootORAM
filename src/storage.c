#include <stdlib.h>
#include <storage.h>
#include <errno.h>
#include <string.h>
#include <utils.h>
#include <sys/stat.h>
#include <stdio.h>

struct storage {
	int size;
	char *file_name;
	FILE *fp;
};

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

/*
 * Initialize a storage object.
 *
 * @param size: size in bytes.
 * @param path: path to the data file.
 *
 * @return a handler of the storage device.
 */
struct storage *storage_init(int size, const char *path)
{
	struct storage *dev = malloc(sizeof(*dev));
	void *const zero_data = calloc(size, 1);
	FILE *fp = fopen(path, "wb");
	fwrite(zero_data, 1, size, fp);
	dev->file_name = malloc(strlen(path) + 1);
	strcpy(dev->file_name, path);
	dev->size = size;
	fclose(fp);
	dev->fp = fopen(path, "r+b");
	free(zero_data);
	return dev;
}

/*
 * Read from the storage device.
 *
 * @param dev: storage handler.
 * @param offset: offset in bytes to start reading.
 * @param size: number of bytes to read.
 * @param buf: memory address to receive data.
 *
 * @return 0 on success.
 * @return other numbers on failure.
 */
int storage_read(const struct storage *dev, int offset, int size, void *buf)
{
	if (offset + size > dev->size) {
		return ENOMEM;
	}
	fseek(dev->fp, offset, SEEK_SET);
	fread(buf, 1, size, dev->fp);
	return 0;
}

/*
 * write from the storage device.
 *
 * @param dev: storage handler.
 * @param offset: offset in bytes to start writeing.
 * @param size: number of bytes to write.
 * @param buf: memory address to receive data.
 *
 * @return 0 on success.
 * @return other numbers on failure.
 */
int storage_write(const struct storage *dev, int offset, int size, const void *buf)
{
	if (offset + size > dev->size) {
		return ENOMEM;
	}
	fseek(dev->fp, offset, SEEK_SET);
	fwrite(buf, 1, size, dev->fp);
	return 0;
}

/*
 * Destroy/Release the storage.
 *
 * @param dev: storage handler.
 *
 * @return 0 on success.
 * @return -1 on failure.
 */
int storage_destroy(struct storage *dev)
{
	fclose(dev->fp);
	free(dev->file_name);
	remove(dev->file_name);
	free(dev);
	return 0;
}
