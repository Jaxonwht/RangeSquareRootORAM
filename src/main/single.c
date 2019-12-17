#include <stdio.h>
#include <bits/types/struct_timeval.h>
#include <oram.h>
#include <stdio.h>
#include <sys/time.h>
#include <utils.h>

/* Instruction files that only contain read accesses do not specify a block size, use 4 bytes in that case. */
static const int DEFAULT_BLK_SIZE = 4;

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
		log_fp = fopen(argv[3], "w");
		gettimeofday(&tv1, NULL);
		instruct = parse_instruction(argv[2], &blk_size, &blk_count);
		gettimeofday(&tv2, NULL);
		timediff = timediffusec(&tv1, &tv2);
		fprintf(log_fp, "ORAM type: square root\n");
		fprintf(log_fp, "Instruction file: %s\n", argv[2]);
		fprintf(log_fp, "Parsing instruction file: %ld microseconds\n", timediff);
		if (blk_size != -1) {
			gettimeofday(&tv1, NULL);
			oram = oram_init(blk_size, 1, blk_count, argv[4]);
			gettimeofday(&tv2, NULL);
			timediff = timediffusec(&tv1, &tv2);
			fprintf(log_fp, "Block size: %d bytes\nBlock count: %d\n", blk_size, blk_count);
			fprintf(log_fp, "Initializing oram: %ld microseconds\n", timediff);
		} else {
			gettimeofday(&tv1, NULL);
			oram = oram_init(DEFAULT_BLK_SIZE, 1, blk_count, argv[4]);
			gettimeofday(&tv2, NULL);
			timediff = timediffusec(&tv1, &tv2);
			fprintf(log_fp, "Block size: %d bytes\nBlock count: %d\n", DEFAULT_BLK_SIZE, blk_count);
			fprintf(log_fp, "Initializing oram: %ld microseconds\n", timediff);
		}
	} else if (!strcmp(argv[1], "generate") && argc == 11) {
		int num_access;
		int max_range;
		log_fp = fopen(argv[9], "w");
		fprintf(log_fp, "ORAM type: square root\n");
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
		fprintf(log_fp, "Instruction file: %s\n", argv[8]);
		fprintf(log_fp, "Parsing instruction file: %ld microseconds\n", timediff);
		gettimeofday(&tv1, NULL);
		oram = oram_init(blk_size, 1, blk_count, argv[10]);
		gettimeofday(&tv2, NULL);
		timediff = timediffusec(&tv1, &tv2);
		fprintf(log_fp, "Block size: %d bytes\nBlock count: %d\n", blk_size, blk_count);
		fprintf(log_fp, "Initializing oram: %ld microseconds\n", timediff);
	} else {
		printf("Urecognized command.\n");
		return 1;
	}
	fprintf(log_fp, "Storage used: %d bytes\n", oram_used_memory(oram));
	fclose(log_fp);
	oram_destroy(oram);
	instruction_free(instruct);
	return 0;
}
