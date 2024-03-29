#include <instruction.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/*
 * This function tests the reading functions provided by the instruction.h API
 *
 * @param argv[1]: file_name under data
 *
 * @return 0 on success.
 * @return 1 on failure.
 */
int main(int argc, char *argv[])
{
	if (argc != 3 && argc != 8) {
		printf("Call the binary as ./instruction_gen_parse parse filename\n");
		printf("Or... ./instruction_gen_parse generate <num_access> <blk_count> <max_range> <r | w | rw> <rand | seq> filename\n");
		return 1;
	}
	if (!strcmp(argv[1], "parse")) {
		int blk_count;
		struct instruction *instruct_arr = parse_instruction(argv[2], &blk_count);
		printf("block count is %d\n", blk_count);
		struct instruction *instruct = instruct_arr;
		while (instruct != NULL) {
			printf("op is %s, idx is %d, size is %d\n", instruct->op == READ ? "read" : "write", instruct->idx, instruct->size);
			instruct = instruct->next;
		}
		int ret = instruction_free(instruct_arr);
		return ret;
	}
	if (!strcmp(argv[1], "generate")) {
		int num_access;
		int blk_count;
		int max_range;
		sscanf(argv[2], "%d", &num_access);
		sscanf(argv[3], "%d", &blk_count);
		sscanf(argv[4], "%d", &max_range);
		if (!strcmp(argv[5], "rw")) {
			if (!strcmp(argv[6], "rand")) {
				generate_rand(argv[7], num_access, blk_count, max_range);
				return 0;
			}
			if (!strcmp(argv[6], "seq")) {
				generate_seq(argv[7], blk_count, max_range);
				return 0;
			}
		}
		if (!strcmp(argv[5], "r")) {
			if (!strcmp(argv[6], "rand")) {
				generate_rand_read(argv[7], num_access, blk_count, max_range);
				return 0;
			}
			if (!strcmp(argv[6], "seq")) {
				generate_seq_read(argv[7], blk_count, max_range);
				return 0;
			}
		}
		if (!strcmp(argv[5], "w")) {
			if (!strcmp(argv[6], "rand")) {
				generate_rand_write(argv[7], num_access, blk_count, max_range);
				return 0;
			}
			if (!strcmp(argv[6], "seq")) {
				generate_seq_write(argv[7], blk_count, max_range);
				return 0;
			}
		}
	}
	printf("Unrecognized command %s\n", argv[1]);
	return 1;
}
