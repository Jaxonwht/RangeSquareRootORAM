#ifndef INSTRUCTION_H
#define INSTRUCTION_H

enum opcode {
	WRITE,
	READ
};

struct instruction {
	enum opcode op;
	int idx;
	int size;
	/*
	void *data;
	*/
	struct instruction *next;
};

int generate_rand(const char *file_name, int num_access, int range, int max_len);
int generate_rand_read(const char *file_name, int num_read, int range, int max_len);
int generate_rand_write(const char *file_name, int num_write, int range, int max_len);

int generate_seq(const char *file_name, int range, int max_len);
int generate_seq_read(const char *file_name, int range, int max_len);
int generate_seq_write(const char *file_name, int range, int max_len);

struct instruction *parse_instruction(const char *file_name, int *blk_count);

int get_num_blk_accessed(struct instruction *instruction);

int instruction_free(struct instruction *instruct);

#endif
