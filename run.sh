#!/usr/bin/env bash

GEN=bin/instruct_gen_parse
SINGLE=bin/single
RANGE=bin/range
SINGLE_STORAGE=data/storage/dummy.img
RANGE_STORAGE=data/storage/range
INSTRUCTION_DIR=data/instruction
LOG_DIR=log

# Generate instruction files
for blk_size in 4 8 16 32 64 128 256
do
	for blk_count in 128 256 512 1024 2048 4096 8192 16384 32768 65536 131072 262144 524288
	do
		let "num_access = $blk_count / 10"
		let "max_range = $blk_count / 10"
		for mode in r rw w
		do
			for gen_mode in rand seq
			do
				$GEN generate $num_access $blk_size $blk_count $max_range $mode $gen_mode ${INSTRUCTION_DIR}/bs_${blk_size}_bc_${blk_count}_${mode}_${gen_mode}.txt
			done
		done
	done
done

# possible logs are rand rand_r rand_w seq seq_r seq_rw
for blk_size in 4 8 16 32 64 128 256
do
	for blk_count in 128 256 512 1024 2048 4096 8192 16384 32768 65536 131072 262144 524288
	do
		let "num_access = $blk_count / 10"
		let "max_range = $blk_count / 10"
		for mode in r rw w
		do
			for gen_mode in rand seq
			do
				$SINGLE parse ${INSTRUCTION_DIR}/bs_${blk_size}_bc_${blk_count}_${mode}_${gen_mode}.txt $LOG_DIR/${gen_mode}_${mode} $SINGLE_STORAGE
			done
		done
		for gen_mode in rand seq
		do
			$RANGE parse ${INSTRUCTION_DIR}/bs_${blk_size}_bc_${blk_count}_r_${gen_mode}.txt $LOG_DIR/${gen_mode}_r $RANGE_STORAGE
		done
	done
done
