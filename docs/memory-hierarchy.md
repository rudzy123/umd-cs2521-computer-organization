# Memory hierarchy

The CPU can use a value only after that value has been brought into a register. Everything else is a larger, slower place to keep bytes. The hierarchy exists because a big memory is slow and a fast memory is small.

```mermaid
flowchart LR
    regs[Registers] --> l1[L1 cache]
    l1 --> l2[L2 cache]
    l2 --> l3[L3 cache]
    l3 --> dram[DRAM]
    dram --> disk[Disk or SSD]
```

| Store | What it is | How this lab treats it |
| --- | --- | --- |
| Registers | A few named words inside the CPU | `%rax` and the other registers in `assembly/`, and `r0` through `r7` in the simulator |
| Cache | Small SRAM that holds recent memory lines | Not programmed directly. The locality demo makes its effect visible in elapsed time |
| DRAM, the RAM | The main byte array the operating system gives a process | The matrix in `cs2521_locality_demo`, and the simulator's data array as a model of a memory |
| Disk or SSD | Storage that survives after the process exits | Out of scope. It is on the diagram because a page fault can make a load wait on it |

Latency grows quickly as you move down the diagram. A register access is on the order of one cycle. An L1 hit is a handful of cycles. A miss that has to go to DRAM is often tens to hundreds of nanoseconds. Those are orders of magnitude, not a number you should memorize or assert in a test. Frequency, cache size, and other running programs all move the measurement.

## Cache lines and locality

Caches do not store individual bytes. They store a line. On current x86-64 processors a line is commonly 64 bytes. Eight consecutive `int64_t` values fill one line. When the program reads the first of those eight, the cache usually fetches the whole line.

Two habits make that useful:

- **Spatial locality.** The next access is near this address, so it is probably already in the line that just arrived.
- **Temporal locality.** The program uses the same address again before the cache has to reuse that slot for something else.

`src/locality.c` stores an `int64_t` matrix in row-major order. Element `(row, col)` lives at `matrix[row * cols + col]`.

The row-major loop walks `col` inside `row`. Addresses increase by one element, 8 bytes. A 64-byte line supplies eight iterations, then the next line is the next address the loop wants.

The column-major loop walks `row` inside `col`. The address increases by one whole row. For the 768 by 768 demonstration that stride is `768 * 8 = 6144` bytes, far larger than a cache line. Each access tends to pull in a line and then use only one of its eight values. The rest of the line is dead weight, and the lines from the previous rows are no longer the ones the next access needs.

Both loops add the same integers. Integer addition is associative, so the checksum does not depend on the visit order. The demo also adds the values once while it fills the matrix and requires both traversals to match that total. A mismatch is a bug. A particular speed ratio is not.

## How to read a timing run

```sh
./build/cs2521_locality_demo
```

The program says this before it prints seconds:

> Elapsed time varies with this machine, its cache sizes, the compiler optimization level, and other programs running right now. Record the numbers you observe. Do not expect a fixed speed ratio.

`src/locality.c` is compiled at `-O2` so the inner loops are not dominated by an unoptimized branch. The first repetition can also be slower than the later ones because the matrix is moving into cache for the first time. Write down every run, including that warmup. Compare the two means on your machine, and explain the difference with the stride and the cache line. Leave the explanation there. A classmate with a different cache, or the same machine under a different load, can see a different gap and still be right.

The unit test uses a tiny matrix and checks the sum. It accepts any non-negative duration, including a duration so small that it rounds to zero.
