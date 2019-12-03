#include <stdlib.h>

/*
 * Initialize a storage object.
 *
 * @param size: size in bytes
 *
 * @return a handler of the storage device.
 */
struct storage *storage_init(const int size)
{
	return NULL;
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
int storage_read(const struct storage *dev, const int offset, const int size, void *buf)
{
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
int storage_write(const struct storage *dev, const int offset, const int size, const void *buf)
{
	return 0;
}
