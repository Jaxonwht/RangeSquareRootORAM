#ifndef INSTRUCTION_H
#define INSTRUCTION_H

enum opcode {
	write,
	read
};

struct instruction {
	enum opcode op;
	int idx;
	int len;
};

int mkdir_force(const char *dir_name);

int generate_rand(const char *file_name, const int num_access, const int range, const int max_len);
int generate_rand_read(const char *file_name, const int num_read, const int range, const int max_len);
int generate_rand_write(const char *file_name, const int num_write, const int range, const int max_len);

int generate_seq(const char *file_name, const int num_access, const int range, const int max_len);
int generate_seq_read(const char *file_name, const int num_read, const int range, const int max_len);
int generate_seq_write(const char *file_name, const int num_write, const int range, const int max_len);

struct instruction *parse_instruction(const char *file_name);

#endif
