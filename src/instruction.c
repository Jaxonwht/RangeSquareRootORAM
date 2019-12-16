#include <stdio.h>
#include <instruction.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/*
 * Generate a random number between low and high - 1.
 *
 * @param low: lower bound.
 * @param high: upper bound + 1.
 *
 * @return a random int.
 */
static int get_rand(int low, int high)
{
	return low + rand() % (high - low);
}

/*
 * Generate random accesses.
 *
 * @param file_name: file name of the instruction file.
 * @param num_accesses: total number of block accesses.
 * @param range: total number of blocks n, the blocks are numbered 0 to n-1.
 * @param max_len: maximum number of contiguous blocks accessed in a single instruction.
 * @param blk_size: block size of the oram.
 *
 * @return 0 on success.
 * @return -1 on failure.
 */
int generate_rand(const char *file_name, int num_access, int range, int max_len, int blk_size)
{
	const char WRITE_STR[] = "write";
	const char READ_STR[] = "read";
	FILE *const fp = fopen(file_name, "w");
	fprintf(fp, "block size %d, block count %d\n", blk_size, range);
	for (int i = 0; i < num_access; i++) {
		const int size = get_rand(1, max_len + 1);
		const int offset = get_rand(0, range - size + 1);
		if (rand() & 1) {
			fprintf(fp, "%s %d %d\n", WRITE_STR, offset, size);
			for (int k = 0; k < blk_size * size; k++) {
				uint8_t rand_byte = (uint8_t)rand();
				fprintf(fp, "%02x ", rand_byte);
			}
			fprintf(fp, "\n");
		} else {
			fprintf(fp, "%s %d %d\n", READ_STR, offset, size);
		}
	}
	fclose(fp);
	return 0;
}

/*
 * Generate random reads.
 *
 * @param file_name: file name of the instruction file.
 * @param num_read: total number of block reads.
 * @param range: total number of blocks n, the blocks are numbered 0 to n-1.
 * @param max_len: maximum number of contiguous blocks read in a single instruction.
 *
 * @return 0 on success.
 * @return -1 on failure.
 */
int generate_rand_read(const char *file_name, int num_read, int range, int max_len)
{
	const char READ_STR[] = "read";
	FILE *const fp = fopen(file_name, "w");
	fprintf(fp, "block size %d, block count %d\n", -1, range);
	for (int i = 0; i < num_read; i++) {
		const int size = get_rand(1, max_len + 1);
		const int offset = get_rand(0, range - size + 1);
		fprintf(fp, "%s %d %d\n", READ_STR, offset, size);
	}
	fclose(fp);
	return 0;
}

/*
 * Generate random writes.
 *
 * @param file_name: file name of the instruction file.
 * @param num_write: total number of block writes.
 * @param range: total number of blocks n, the blocks are numbered 0 to n-1.
 * @param max_len: maximum number of contiguous blocks written in a single instruction.
 * @param blk_size: block size of the oram.
 *
 * @return 0 on success.
 * @return -1 on failure.
 */
int generate_rand_write(const char *file_name, int num_write, int range, int max_len, int blk_size)
{
	const char WRITE_STR[] = "write";
	FILE *const fp = fopen(file_name, "w");
	fprintf(fp, "block size %d, block count %d\n", blk_size, range);
	for (int i = 0; i < num_write; i++) {
		const int size = get_rand(1, max_len + 1);
		const int offset = get_rand(0, range - size + 1);
		fprintf(fp, "%s %d %d\n", WRITE_STR, offset, size);
		for (int k = 0; k < blk_size * size; k++) {
			uint8_t rand_byte;
			rand_byte = (uint8_t)get_rand(0, 0x100);
			fprintf(fp, "%02x ", rand_byte);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
	return 0;
}

/*
 * Generate sequential accesses.
 *
 * @param file_name: file name of the instruction file.
 * @param range: total number of blocks n, the blocks are numbered 0 to n-1.
 * @param max_len: maximum number of contiguous blocks accessed in a single instruction.
 * @param blk_size: block size of the oram.
 *
 * @return 0 on success.
 * @return -1 on failure.
 */
int generate_seq(const char *file_name, int range, int max_len, int blk_size)
{
	const char WRITE_STR[] = "write";
	const char READ_STR[] = "read";
	FILE *const fp = fopen(file_name, "w");
	fprintf(fp, "block size %d, block count %d\n", blk_size, range);
	int idx = 0;
	while (idx < range) {
		const int size = get_rand(1, max_len + 1);
		const int next = idx + size;
		int actual_size = range - idx;
		if (size < actual_size) {
			actual_size = size;
		}
		if (rand() & 1) {
			fprintf(fp, "%s %d %d\n", WRITE_STR, idx, actual_size);
			for (int k = 0; k < blk_size * actual_size; k++) {
				uint8_t rand_byte = (uint8_t)rand();
				fprintf(fp, "%02x ", rand_byte);
			}
			fprintf(fp, "\n");
		} else {
			fprintf(fp, "%s %d %d\n", READ_STR, idx, actual_size);
		}
		idx = next;
	}
	fclose(fp);
	return 0;
}

/*
 * Generate sequential reads.
 *
 * @param file_name: file name of the instruction file.
 * @param range: total number of blocks n, the blocks are numbered 0 to n-1.
 * @param max_len: maximum number of contiguous blocks read in a single instruction.
 *
 * @return 0 on success.
 * @return -1 on failure.
 */
int generate_seq_read(const char *file_name, int range, int max_len)
{
	const char READ_STR[] = "read";
	FILE *const fp = fopen(file_name, "w");
	fprintf(fp, "block size %d, block count %d\n", -1, range);
	int idx = 0;
	while (idx < range) {
		const int size = get_rand(1, max_len + 1);
		const int next = idx + size;
		int actual_size = range - idx;
		if (size < actual_size) {
			actual_size = size;
		}
		fprintf(fp, "%s %d %d\n", READ_STR, idx, actual_size);
		idx = next;
	}
	fclose(fp);
	return 0;
}

/*
 * Generate sequential writes.
 *
 * @param file_name: file name of the instruction file.
 * @param range: total number of blocks n, the blocks are numbered 0 to n-1.
 * @param max_len: maximum number of contiguous blocks written in a single instruction.
 * @param blk_size: block size of the oram.
 *
 * @return 0 on success.
 * @return -1 on failure.
 */
int generate_seq_write(const char *file_name, int range, int max_len, int blk_size)
{
	const char WRITE_STR[] = "write";
	FILE *const fp = fopen(file_name, "w");
	fprintf(fp, "block size %d, block count %d\n", blk_size, range);
	int idx = 0;
	while (idx < range) {
		const int size = get_rand(1, max_len + 1);
		const int next = idx + size;
		int actual_size = range - idx;
		if (size < actual_size) {
			actual_size = size;
		}
		fprintf(fp, "%s %d %d\n", WRITE_STR, idx, actual_size);
		for (int k = 0; k < blk_size * actual_size; k++) {
			uint8_t rand_byte = (uint8_t)rand();
			fprintf(fp, "%02x ", rand_byte);
		}
		fprintf(fp, "\n");
		idx = next;
	}
	fclose(fp);
	return 0;
}

/*
 * Generate an array of instructions from file.
 *
 * @param file_name: file name of the instruction file.
 * @param blk_size: store the block size used by the instruction file.
 * @param blk_count: store the total range of blocks accessed.
 *
 * @return a struct instruction * on success.
 * @return NULL on failure.
 */
struct instruction *parse_instruction(const char *file_name, int *blk_size, int *blk_count)
{
	const char WRITE_STR[] = "write";
	const char READ_STR[] = "read";
	const int WRITE_STR_LEN = strlen(WRITE_STR);
	const int READ_STR_LEN = strlen(READ_STR);
	FILE *fp = fopen(file_name, "r");
	struct instruction *const head = malloc(sizeof(*head));
	struct instruction *iter = head;
	char op_buf[WRITE_STR_LEN + 1];
	int idx;
	int size;
	fscanf(fp, "block size %d, block count %d\n", blk_size, blk_count);
	while (fscanf(fp, "%s %d %d\n", op_buf, &idx, &size) == 3) {
		iter->next = malloc(sizeof(*iter));
		iter = iter->next;
		if (!strncmp(op_buf, WRITE_STR, WRITE_STR_LEN)) {
			iter->op = WRITE;
			iter->data = malloc(size * *blk_size);
			for (int i = 0; i < size * *blk_size; i++) {
				fscanf(fp, "%02hhx ", (uint8_t *)iter->data + i);
			}
			fscanf(fp, "\n");
		} else if (!strncmp(op_buf, READ_STR, READ_STR_LEN)){
			iter->op = READ;
		}
		iter->idx = idx;
		iter->size = size;
		iter->next = NULL;
	}
	fclose(fp);
	struct instruction *const instruct = head->next;
	free(head);
	return instruct;
}

/*
 * Free the array of instructions.
 *
 * @param instruct: array of instructions.
 *
 * @return 0 on success.
 * @return -1 on failure.
 */
int instruction_free(struct instruction *instruct)
{
	struct instruction *tmp;
	while (instruct != NULL) {
		free(instruct->data);
		tmp = instruct;
		instruct = instruct->next;
		free(tmp);
	}
	return 0;
}
