#!/usr/bin/env bash

BIN=bin/range
FILE=data/instruction/range
LOG_FILE=log/range
STORAGE_FILE=data/storage/range.img

# generate an instruction file
# 10 accesses, 4 byte block, 100 total blocks, 2 maximum block range, read-write, random
# filename data/generated.txt
${BIN} generate 10 4 100 2 rw rand ${FILE}_rand.txt ${LOG_FILE}_rand.log ${STORAGE_FILE}_rand.img
${BIN} generate 10 4 100 2 r rand ${FILE}_rand_read.txt ${LOG_FILE}_rand_read.log ${STORAGE_FILE}_rand_read.img
${BIN} generate 10 4 100 2 w rand ${FILE}_rand_write.txt ${LOG_FILE}_rand_write.log ${STORAGE_FILE}_rand_write.img

# sequential versions
${BIN} generate 10 4 100 2 rw seq ${FILE}_seq.txt ${LOG_FILE}_seq.log ${STORAGE_FILE}_seq.img
${BIN} generate 10 4 100 2 r seq ${FILE}_seq_read.txt ${LOG_FILE}_seq_read.log ${STORAGE_FILE}_seq_read.img
${BIN} generate 10 4 100 2 w seq ${FILE}_seq_write.txt ${LOG_FILE}_seq_write.log ${STORAGE_FILE}_seq_write.img
