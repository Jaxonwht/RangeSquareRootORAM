#ifndef STORAGE_H
#define STORAGE_H

struct storage {
	int size;
	void *handler;
};

struct storage *storage_init(const int size);

int storage_read(const struct storage *dev, const int offset, const int size, void *buf);

int storage_write(const struct storage *dev, const int offset, const int size, const void *buf);

int storage_destroy(struct storage *dev);

#endif
