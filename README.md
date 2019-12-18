# RangeSquareRootORAM

This is a tree range ORAM implementation based on the square-root ORAM.

## Build

```bash
make
```

The resulting binary files will be put in `bin/`.
The resulting test binary files will be put in `test/`.

## Run

```bash
./run_single.sh
./run_range.sh
```
To run examples of single and range oram.

```bash
./run.sh
```
This runs a large collection of examples. It'de be best to call `make clean` before calling `./run.sh`.

The log files will be placed in `log/`.

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
