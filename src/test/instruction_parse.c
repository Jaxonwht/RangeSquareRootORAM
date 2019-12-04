#include <instruction.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	if (argc != 2) {
		printf("Call the binary as ./instruction_test filename");
		return 1;
	}
	struct instruction_arr *instruct_arr = parse_instruction(argv[1]);
	printf("array length %d\n", instruct_arr->count);
	for (int i = 0; i < instruct_arr->count; i++) {
		const struct instruction *const instruct = &instruct_arr->instruction[i];
		printf("op is %d, idx is %d, size is %d\n", instruct->op, instruct->idx, instruct->size);
	}
	int ret = instruction_arr_free(instruct_arr);
	return ret;
}
