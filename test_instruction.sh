#!/usr/bin/env bash

FILE=data/test_instruction

# generate an instruction file
# 10 accesses, 4 byte block, 100 total blocks, 2 maximum block range, read-write, random
# filename data/generated.txt
test/instruction_parse generate 10 4 100 2 rw rand $FILE
test/instruction_parse generate 10 4 100 2 r rand ${FILE}_read
test/instruction_parse generate 10 4 100 2 w rand ${FILE}_write

# test the generated file
test/instruction_parse parse $FILE
