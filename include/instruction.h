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
};

struct instruction_arr {
	int count;
	struct instruction *instruction;
};

int mkdir_force(const char *dir_name);

int generate_rand(const char *file_name, const int num_access, const int range, const int max_len);
int generate_rand_read(const char *file_name, const int num_read, const int range, const int max_len);
int generate_rand_write(const char *file_name, const int num_write, const int range, const int max_len);

int generate_seq(const char *file_name, const int num_access, const int range, const int max_len);
int generate_seq_read(const char *file_name, const int num_read, const int range, const int max_len);
int generate_seq_write(const char *file_name, const int num_write, const int range, const int max_len);

struct instruction_arr *parse_instruction(const char *file_name);

int instruction_arr_free(struct instruction_arr *arr);

#endif
