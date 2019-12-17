#include <stdio.h>
#include <bits/types/struct_timeval.h>
#include <oram.h>
#include <stdio.h>
#include <sys/time.h>
#include <utils.h>
#include <stdbool.h>

/* Instruction files that only contain read accesses do not specify a block size, use 4 bytes in that case. */
static const int DEFAULT_BLK_SIZE = 4;

/*
 * Process all the instructions. Read instructions are read into dummy buffers.
 *
 * @param range_oram: range_oram handler.
 * @param instruct: instruction array.
 * @param blk_size: block size in bytes.
 * @param blk_count: total number of blocks spanned by the instructions.
 */
static void range_oram_process_instruction(struct range_oram *range_oram, const struct instruction *instruct, int blk_size, int blk_count)
{
	char buf[blk_size * blk_count];
	while (!instruct) {
		if (instruct->op == READ) {
			range_oram_access(range_oram, instruct->idx, instruct->size, READ, buf);
		} else if (instruct->op == WRITE) {
			range_oram_access(range_oram, instruct->idx, instruct->size, WRITE, instruct->data);
		}
		instruct = instruct->next;
	}
}

/*
 * Main function to run a range square-rootrange_oram
 *
 * @return 0 on sucess
 * @return other numbers on failure
 */
int main(int argc, char *argv[])
{
	if (argc != 5 && argc != 11) {
		printf("Call the binary as ./range parse instructionfile logfile storagefile\n");
		printf("Or... ./range generate <num_access> <blk_size> <blk_count> <max_range> <r | w | rw> <rand | seq> instructionfile logfile storagefile\n");
		return 1;
	}
	int blk_size;
	int blk_count;
	struct instruction *instruct;
	struct range_oram *range_oram;
	struct timeval tv1;
	struct timeval tv2;
	long timediff;
	FILE *log_fp;
	if (!strcmp(argv[1], "parse") && argc == 5) {
		log_fp = fopen(argv[3], "r");
		if (log_fp == NULL) {
			log_fp = fopen(argv[3], "a");
			fprintf(log_fp, "Range\tInstruction_file\tInstruction_parsing_time/usec\tBlock_size/B\tBlock_count\tOram_initialization_time/usec\tClient_memory/KiB\tDisk_storage/B\tRunning_time/usec\n");
		} else {
			fclose(log_fp);
			log_fp = fopen(argv[3], "a");
		}
		gettimeofday(&tv1, NULL);
		instruct = parse_instruction(argv[2], &blk_size, &blk_count);
		gettimeofday(&tv2, NULL);
		timediff = timediffusec(&tv1, &tv2);
		fprintf(log_fp, "%d\t", 1);
		fprintf(log_fp, "%s\t", argv[2]);
		fprintf(log_fp, "%ld\t", timediff);
		if (blk_size != -1) {
			gettimeofday(&tv1, NULL);
			range_oram = range_oram_init(blk_size, blk_count, argv[4]);
			gettimeofday(&tv2, NULL);
			timediff = timediffusec(&tv1, &tv2);
			fprintf(log_fp, "%d\t", blk_size);
			fprintf(log_fp, "%d\t", blk_count);
			fprintf(log_fp, "%ld\t", timediff);
		} else {
			blk_size = DEFAULT_BLK_SIZE;
			gettimeofday(&tv1, NULL);
			range_oram = range_oram_init(blk_size, blk_count, argv[4]);
			gettimeofday(&tv2, NULL);
			timediff = timediffusec(&tv1, &tv2);
			fprintf(log_fp, "%d\t", blk_size);
			fprintf(log_fp, "%d\t", blk_count);
			fprintf(log_fp, "%ld\t", timediff);
		}
	} else if (!strcmp(argv[1], "generate") && argc == 11) {
		int num_access;
		int max_range;
		log_fp = fopen(argv[9], "r");
		if (log_fp == NULL) {
			log_fp = fopen(argv[9], "a");
			fprintf(log_fp, "Range\tInstruction_file\tInstruction_parsing_time/usec\tBlock_size/B\tBlock_count\tOram_initialization_time/usec\tClient_memory/KiB\tDisk_storage/B\tRunning_time/usec\n");
		} else {
			fclose(log_fp);
			log_fp = fopen(argv[9], "a");
		}
		fprintf(log_fp, "%d\t", 1);
		sscanf(argv[2], "%d", &num_access);
		sscanf(argv[3], "%d", &blk_size);
		sscanf(argv[4], "%d", &blk_count);
		sscanf(argv[5], "%d", &max_range);
		if (!strcmp(argv[6], "rw")) {
			if (!strcmp(argv[7], "rand")) {
				generate_rand(argv[8], num_access, blk_count, max_range, blk_size);
			} else if (!strcmp(argv[7], "seq")) {
				generate_seq(argv[8], blk_count, max_range, blk_size);
			} else {
				printf("Urecognized command.\n");
				return 1;
			}
		} else if (!strcmp(argv[6], "r")) {
			if (!strcmp(argv[7], "rand")) {
				generate_rand_read(argv[8], num_access, blk_count, max_range);
			} else if (!strcmp(argv[7], "seq")) {
				generate_seq_read(argv[8], blk_count, max_range);
			} else {
				printf("Urecognized command.\n");
				return 1;
			}
		} else if (!strcmp(argv[6], "w")) {
			if (!strcmp(argv[7], "rand")) {
				generate_rand_write(argv[8], num_access, blk_count, max_range, blk_size);
			} else if (!strcmp(argv[7], "seq")) {
				generate_seq_write(argv[8], blk_count, max_range, blk_size);
			} else {
				printf("Urecognized command.\n");
				return 1;
			}
		} else {
			printf("Urecognized command.\n");
			return 1;
		}
		int a;
		int b;
		gettimeofday(&tv1, NULL);
		instruct = parse_instruction(argv[8], &a, &b);
		gettimeofday(&tv2, NULL);
		timediff = timediffusec(&tv1, &tv2);
		fprintf(log_fp, "%s\t", argv[8]);
		fprintf(log_fp, "%ld\t", timediff);
		gettimeofday(&tv1, NULL);
		range_oram = range_oram_init(blk_size, blk_count, argv[10]);
		gettimeofday(&tv2, NULL);
		timediff = timediffusec(&tv1, &tv2);
		fprintf(log_fp, "%d\t", blk_size);
		fprintf(log_fp, "%d\t", blk_count);
		fprintf(log_fp, "%ld\t", timediff);
	} else {
		printf("Urecognized command.\n");
		return 1;
	}
	fprintf(log_fp, "%lu\t", get_memory_usage());
	fprintf(log_fp, "%lu\t", range_oram_used_memory(range_oram));
	gettimeofday(&tv1, NULL);
	range_oram_process_instruction(range_oram, instruct, blk_size, blk_count);
	gettimeofday(&tv2, NULL);
	timediff = timediffusec(&tv1, &tv2);
	fprintf(log_fp, "%ld\n", timediff);
	fclose(log_fp);
	range_oram_destroy(range_oram);
	instruction_free(instruct);
	return 0;
}
