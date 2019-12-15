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
	if (argc != 3 && argc != 9) {
		printf("Call the binary as ./instruction_gen_parse parse filename\n");
		printf("Or... ./instruction_gen_parse generate <num_access> <blk_size> <blk_count> <max_range> <r | w | rw> <rand | seq> filename\n");
		return 1;
	}
	if (!strcmp(argv[1], "parse")) {
		int blk_size;
		struct instruction *instruct_arr = parse_instruction(argv[2], &blk_size);
		struct instruction *instruct = instruct_arr;
		while (instruct != NULL) {
			printf("op is %s, idx is %d, size is %d\n", instruct->op == READ ? "read" : "write", instruct->idx, instruct->size);
			if (instruct->op == WRITE) {
				printf("data is:");
				for (int i = 0; i < instruct->size * blk_size; i++) {
					printf(" %02hhx", *(uint8_t *)(instruct->data + i));
				}
				printf("\n");
			}
			instruct = instruct->next;
		}
		int ret = instruction_free(instruct_arr);
		return ret;
	}
	if (!strcmp(argv[1], "generate")) {
		int num_access;
		int blk_size;
		int blk_count;
		int max_range;
		sscanf(argv[2], "%d", &num_access);
		sscanf(argv[3], "%d", &blk_size);
		sscanf(argv[4], "%d", &blk_count);
		sscanf(argv[5], "%d", &max_range);
		if (!strcmp(argv[6], "rw")) {
			if (!strcmp(argv[7], "rand")) {
				generate_rand(argv[8], num_access, blk_count, max_range, blk_size);
				return 0;
			}
			if (!strcmp(argv[7], "seq")) {
				generate_seq(argv[8], blk_count, max_range, blk_size);
				return 0;
			}
		}
		if (!strcmp(argv[6], "r")) {
			if (!strcmp(argv[7], "rand")) {
				generate_rand_read(argv[8], num_access, blk_count, max_range);
				return 0;
			}
			if (!strcmp(argv[7], "seq")) {
				generate_seq_read(argv[8], blk_count, max_range);
				return 0;
			}
		}
		if (!strcmp(argv[6], "w")) {
			if (!strcmp(argv[7], "rand")) {
				generate_rand_write(argv[8], num_access, blk_count, max_range, blk_size);
				return 0;
			}
			if (!strcmp(argv[7], "seq")) {
				generate_seq_write(argv[8], blk_count, max_range, blk_size);
				return 0;
			}
		}
	}
	printf("Unrecognized command %s\n", argv[1]);
	return 1;
}
