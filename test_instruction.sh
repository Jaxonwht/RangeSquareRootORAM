#!/usr/bin/env bash

FILE_RAND=data/instruction/test_instruction_rand
FILE_SEQ=data/instruction/test_instruction_seq

# generate an instruction file
# 10 accesses, 4 byte block, 100 total blocks, 2 maximum block range, read-write, random
# filename data/generated.txt
bin/instruct_gen_parse generate 10 4 100 2 rw rand ${FILE_RAND}
bin/instruct_gen_parse generate 10 4 100 2 r rand ${FILE_RAND}_read
bin/instruct_gen_parse generate 10 4 100 2 w rand ${FILE_RAND}_write

# sequential versions
bin/instruct_gen_parse generate 10 4 100 2 rw seq ${FILE_SEQ}
bin/instruct_gen_parse generate 10 4 100 2 r seq ${FILE_SEQ}_read
bin/instruct_gen_parse generate 10 4 100 2 w seq ${FILE_SEQ}_write

# parse the generated file
bin/instruct_gen_parse parse ${FILE_RAND}
bin/instruct_gen_parse parse ${FILE_SEQ}
