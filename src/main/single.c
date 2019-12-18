#include <stdio.h>
#include <bits/types/struct_timeval.h>
#include <stdbool.h>
#include <oram.h>
#include <stdio.h>
#include <sys/time.h>
#include <utils.h>

/*
 * Process all the instructions. Read instructions are read into dummy buffers.
 *
 * @param oram: oram handler.
 * @param instruct: instruction array.
 * @param blk_size: block size in bytes.
 */
static void oram_process_instruction(struct oram *oram, const struct instruction *instruct, int blk_size)
{
	char buf[blk_size];
	while (instruct) {
		if (instruct->op == READ) {
			for (int i = 0; i < instruct->size; i++) {
				oram_access(oram, instruct->idx + i, READ, buf);
			}
		} else if (instruct->op == WRITE) {
			char data[instruct->size * blk_size];
			for (int i = 0; i < instruct->size; i++) {
				oram_access(oram, instruct->idx + i, WRITE, data + i * blk_size);
			}
		}
		instruct = instruct->next;
	}
}

/*
 * Main function to run a single square-root ORAM
 *
 * @return 0 on sucess
 * @return other numbers on failure
 */
int main(int argc, char *argv[])
{
	if (argc != 5 && argc != 11) {
		printf("Call the binary as ./single parse instructionfile logfile storagefile\n");
		printf("Or... ./single generate <num_access> <blk_size> <blk_count> <max_range> <r | w | rw> <rand | seq> instructionfile logfile storagefile\n");
		return 1;
	}
	int blk_size;
	int blk_count;
	struct instruction *instruct;
	struct oram *oram;
	struct timeval tv1;
	struct timeval tv2;
	long timediff;
	FILE *log_fp;
	if (!strcmp(argv[1], "parse") && argc == 5) {
		log_fp = fopen(argv[3], "r");
		if (log_fp == NULL) {
			log_fp = fopen(argv[3], "a");
			fprintf(log_fp, "Range\tInstruction_file\tInstruction_parsing_time/usec\tBlock_size/B\tBlock_count\tOram_initialization_time/usec\tClient_memory/KiB\tDisk_storage/B\tRunning_time_per_block/usec\n");
		} else {
			fclose(log_fp);
			log_fp = fopen(argv[3], "a");
		}
		gettimeofday(&tv1, NULL);
		instruct = parse_instruction(argv[2], &blk_size, &blk_count);
		gettimeofday(&tv2, NULL);
		timediff = timediffusec(&tv1, &tv2);
		fprintf(log_fp, "%d\t", 0);
		fprintf(log_fp, "%s\t", argv[2]);
		fprintf(log_fp, "%ld\t", timediff);
		gettimeofday(&tv1, NULL);
		oram = oram_init(blk_size, 1, blk_count, argv[4], NULL);
		gettimeofday(&tv2, NULL);
		timediff = timediffusec(&tv1, &tv2);
		fprintf(log_fp, "%d\t", blk_size);
		fprintf(log_fp, "%d\t", blk_count);
		fprintf(log_fp, "%ld\t", timediff);
	} else if (!strcmp(argv[1], "generate") && argc == 11) {
		int num_access;
		int max_range;
		log_fp = fopen(argv[9], "r");
		if (log_fp == NULL) {
			log_fp = fopen(argv[9], "a");
			fprintf(log_fp, "Range\tInstruction_file\tInstruction_parsing_time/usec\tBlock_size/B\tBlock_count\tOram_initialization_time/usec\tClient_memory/KiB\tDisk_storage/B\tRunning_time_per_block/usec\n");
		} else {
			fclose(log_fp);
			log_fp = fopen(argv[9], "a");
		}
		fprintf(log_fp, "%d\t", 0);
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
				generate_rand_read(argv[8], num_access, blk_count, max_range, blk_size);
			} else if (!strcmp(argv[7], "seq")) {
				generate_seq_read(argv[8], blk_count, max_range, blk_size);
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
		oram = oram_init(blk_size, 1, blk_count, argv[10], NULL);
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
	fprintf(log_fp, "%lu\t", oram_used_memory(oram));
	gettimeofday(&tv1, NULL);
	oram_process_instruction(oram, instruct, blk_size);
	gettimeofday(&tv2, NULL);
	timediff = timediffusec(&tv1, &tv2);
	fprintf(log_fp, "%ld\n", timediff / get_num_blk_accessed(instruct));
	fclose(log_fp);
	oram_destroy(oram);
	instruction_free(instruct);
	return 0;
}
