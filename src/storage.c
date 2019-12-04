#include <stdlib.h>
#include <storage.h>
#include <errno.h>
#include <string.h>

/*
 * Initialize a storage object.
 *
 * @param size: size in bytes
 *
 * @return a handler of the storage device.
 */
struct storage *storage_init(int size)
{
	struct storage *dev = malloc(sizeof(*dev));
	dev->size = size;
	dev->handler = malloc(size);
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
	memcpy(buf, dev->handler + offset, size);
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
	memcpy(dev->handler + offset, buf, size);
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
	free(dev->handler);
	free(dev);
	return 0;
}
