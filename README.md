# CS 2521 Computer Organization Laboratory

This repository is a small Linux lab for binary representation, CPU execution, x86-64 assembly, the System V calling convention, and the memory hierarchy. Every chapter has a program you can build and run. The tests check results. They do not depend on load addresses or on a fixed timing ratio.

There are two machines in the lab:

- The host is a real x86-64 CPU. `assembly/*.S` is GNU assembler source, called from C with the System V AMD64 ABI.
- `include/sim.h` is a teaching CPU with a fixed-size instruction and a separate data memory. Its program counter is an index, not an x86 byte address. Use it to watch fetch, decode, execute, and halt. Do not confuse its opcodes with x86 opcodes.

## Build

You need GCC, GNU binutils (`as`, `ld`, `objdump`), CMake, and Python 3. Nothing in the lab needs root.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

C sources are C17. The cache experiment in `src/locality.c` is compiled at `-O2` even in a Debug build, so the timing comparison is about memory access. A Release build is still useful for the rest of the programs:

```sh
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release
```

## Run

```sh
./build/cs2521_bits_demo
./build/cs2521_repr_demo
./build/cs2521_sim_demo
./build/cs2521_locality_demo
python3 scripts/bit_lab.py -- 42 -42
python3 scripts/disassemble.py build docs/generated-disassembly.txt
```

The locality program prints a warning before it prints numbers. Those numbers belong to the machine you ran it on.

## Tests

| Program | What it locks down |
| --- | --- |
| `test_bits` | Set, clear, toggle, inspect, popcount, rotate, extract, and host endianness, including rejected indexes |
| `test_repr` | Binary and hex conversion, two's-complement bit patterns, and overflow checks that do not perform a signed overflow |
| `test_ops` | Assembly results against the C reference, including wraparound and factorial overflow |
| `test_sim` | Each teaching-CPU instruction, faults, and the split fetch, decode, and execute functions |
| `test_locality` | Identical integer checksums for both traversals |

`test_locality` only checks that a measured duration is non-negative. It does not require column-major traversal to be slower by any particular factor.

## Repository

```text
include/     Public headers
src/         C implementations and demonstrations
assembly/    x86-64 functions in GNU assembler syntax
tests/       C tests
scripts/     Python 3 helpers for binary display and objdump
docs/        Reading for each chapter
```

## Study order

1. [Binary cheat sheet](docs/binary-cheat-sheet.md), then `cs2521_bits_demo`, `cs2521_repr_demo`, and `scripts/bit_lab.py`.
2. [Assembly walkthrough](docs/assembly-walkthrough.md) and `docs/generated-disassembly.txt`.
3. [CPU cycle](docs/cpu-cycle.md) and `cs2521_sim_demo`.
4. [Memory hierarchy](docs/memory-hierarchy.md) and `cs2521_locality_demo`.
5. [Learning log](docs/learning-log.md).

`COMMIT_MESSAGE.md` is the commit summary for this laboratory.

## License

MIT. See [LICENSE](LICENSE).
