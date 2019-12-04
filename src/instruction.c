#include <stdio.h>
#include <instruction.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

/*
 * mkdir -p to create the data directory.
 *
 * @return 0 on success.
 * @return -1 on failure.
 */
int mkdir_force(const char *dir_name)
{
	int ret = mkdir(dir_name, 0755);
	if (ret != 0 && ret != EEXIST){
		return -1;
	}
	else{
		return 0;
	}
}

/*
 * Generate random accesses.
 *
 * @param file_name: file name of the instruction file.
 * @param num_accesses: total number of block accesses.
 * @param range: total number of blocks n, the blocks are numbered 0 to n-1.
 * @param max_len: maximum number of contiguous blocks accessed in a single instruction.
 *
 * @return 0 on success.
 * @return -1 on failure.
 */
int generate_rand(const char *file_name, const int num_access, const int range, const int max_len)
{
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
int generate_rand_read(const char *file_name, const int num_read, const int range, const int max_len)
{
	return 0;
}

/*
 * Generate random writes.
 *
 * @param file_name: file name of the instruction file.
 * @param num_write: total number of block writes.
 * @param range: total number of blocks n, the blocks are numbered 0 to n-1.
 * @param max_len: maximum number of contiguous blocks written in a single instruction.
 *
 * @return 0 on success.
 * @return -1 on failure.
 */
int generate_rand_write(const char *file_name, const int num_write, const int range, const int max_len)
{
	return 0;
}

/*
 * Generate sequential accesses.
 *
 * @param file_name: file name of the instruction file.
 * @param num_accesses: total number of block accesses.
 * @param range: total number of blocks n, the blocks are numbered 0 to n-1.
 * @param max_len: maximum number of contiguous blocks accessed in a single instruction.
 *
 * @return 0 on success.
 * @return -1 on failure.
 */
int generate_seq(const char *file_name, const int num_access, const int range, const int max_len)
{
	return 0;
}

/*
 * Generate sequential reads.
 *
 * @param file_name: file name of the instruction file.
 * @param num_read: total number of block reads.
 * @param range: total number of blocks n, the blocks are numbered 0 to n-1.
 * @param max_len: maximum number of contiguous blocks read in a single instruction.
 *
 * @return 0 on success.
 * @return -1 on failure.
 */
int generate_seq_read(const char *file_name, const int num_read, const int range, const int max_len)
{
	return 0;
}

/*
 * Generate sequential writes.
 *
 * @param file_name: file name of the instruction file.
 * @param num_write: total number of block writes.
 * @param range: total number of blocks n, the blocks are numbered 0 to n-1.
 * @param max_len: maximum number of contiguous blocks written in a single instruction.
 *
 * @return 0 on success.
 * @return -1 on failure.
 */
int generate_seq_write(const char *file_name, const int num_write, const int range, const int max_len)
{
	return 0;
}

/*
 * Generate an array of instructions from file.
 *
 * @param file_name: file name of the instruction file
 * 
 * @return a struct instruction_arr * on success.
 * @return NULL on failure.
 */
struct instruction_arr *parse_instruction(const char *file_name)
{
	const char WRITE_STR[] = "write";
	const char READ_STR[] = "read";
	const int WRITE_STR_LEN = strlen(WRITE_STR);
	const int READ_STR_LEN = strlen(READ_STR);
	FILE *fp = fopen(file_name, "r");
	struct instruction_arr *const arr_ptr = malloc(sizeof(*arr_ptr));
	int count = 0;
	char buf[WRITE_STR_LEN + 1];
	while (fgets(buf, WRITE_STR_LEN + 1, fp) != NULL) {
		if (strncmp(buf, WRITE_STR, WRITE_STR_LEN) == 0 || strncmp(buf, READ_STR, READ_STR_LEN) == 0) {
			count++;
		}
	}
	arr_ptr->count = count;
	arr_ptr->instruction = malloc(count * sizeof(*arr_ptr->instruction));
	fclose(fp);
	fp = fopen(file_name, "r");
	count = 0;
	char op_buf[6];
	int idx;
	int size;
	while (fscanf(fp, "%s %d %d\n", op_buf, &idx, &size) == 3) {
		struct instruction *const instruct = &arr_ptr->instruction[count++];
		if (strncmp(op_buf, WRITE_STR, WRITE_STR_LEN) == 0) {
			instruct->op = WRITE;
		} else if (strncmp(op_buf, READ_STR, READ_STR_LEN) == 0){
			instruct->op = READ;
		} else {
			count--;
			continue;
		}
		instruct->idx = idx;
		instruct->size = size;
	}
	fclose(fp);
	return arr_ptr;
}

/*
 * Free the array of instructions.
 *
 * @param arr: array of instructions.
 *
 * @return 0 on success.
 * @return -1 on failure.
 */
int instruction_arr_free(struct instruction_arr *arr)
{
	free(arr->instruction);
	free(arr);
	return 0;
}
