#ifndef STORAGE_H
#define STORAGE_H

struct storage;

struct storage *storage_init(unsigned long size, const char *path);

int storage_read(const struct storage *dev, int offset, int size, void *buf);

int storage_write(const struct storage *dev, int offset, int size, const void *buf);

int storage_destroy(struct storage *dev);

unsigned long get_size(const struct storage *dev);

#endif
