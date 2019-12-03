#include <stdio.h>
#include <instruction.h>
#include <sys/stat.h>
#include <errno.h>

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
 * @return a struct instruction * on success.
 * @return NULL on failure.
 */
struct instruction *parse_instruction(const char *file_name)
{
	return NULL;
}
