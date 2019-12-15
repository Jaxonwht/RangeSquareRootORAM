# RangeSquareRootORAM

This is a tree range ORAM implementation based on the square-root ORAM.

## Build

```bash
make
```

The resulting binary files will be put in **bin/**

## Run

`./test_instruction.sh` to test instruction generation and testing functionalities. The generated instruction files are in `data/instruction/` Detailed comments are in the shell file.

## Clean

```bash
make clean
```
Clean everything.

```bash
make storage_clean
```
Clean all the files for disk storage of previous ORAM sessions.

```bash
make instruction_clean
```
Clean all the instruction files.
