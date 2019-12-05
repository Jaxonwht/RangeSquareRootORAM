#include <stdio.h>
#include <math.h>
#include <instruction.h>
#include <storage.h>


struct oram *oram_init(const int blk_size, const int group_size, const int group_count) {
	storage_init(group_size * group_count + 2(sqrt(blk_size * group_size * group_count)));
}


int oram_access(const struct oram *oram, const int idx, const enum code, void *buffer) {
	if (code.equals(opcode.read)) {
		storage_read(oram->*dev, idx*(oram->group_size * oram->blk_size + sizeof(group_info)), oram->group_size * oram->blk_size, buffer);
	}
	if (code.equals(opcode.write)) {
		storage_write(oram->*dev, idx*(oram->group_size * oram->blk_size + sizeof(group_info)), oram->group_size * oram->blk_size, buffer);
	}
}

void client_access


