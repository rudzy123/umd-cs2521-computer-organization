# Learning log

Use this file as the lab journal. Answer in your own words after you have run the program named in the question. Measurements belong to your machine. Copy the numbers the program printed, and say what those numbers do not prove.

## Representation

Run `./build/cs2521_repr_demo` and `python3 scripts/bit_lab.py -- 42 -42`.

1. Pick a number other than 42. Write it in decimal, binary, and hexadecimal. Show the grouping of four bits that makes the hex form obvious.
2. Build `-13` in eight bits with invert-and-add-1. Check the result against the cheat sheet, then explain why the sign bit is set.
3. Why is `INT32_MIN` not "just the negation of a positive `int32_t`"?
4. `repr_add_i32(INT32_MAX, 1, &sum)` returns `-1` and does not add the two `int32_t` values together. What C rule makes that refusal necessary, and what does the unsigned adder do with the same bits?

## Boolean operations and bits

Run `./build/cs2521_bits_demo`.

5. Starting from 0, which Boolean operation sets bit 3, and which one clears it again? Write the mask in binary.
6. Rotate `0x80000001` left by one bit on paper. Where does the bit that falls off the left end come back?
7. What did `bits_host_endianness` report, and which byte of `0x01020304` is at the lowest address?

## Assembly and stack frames

Read `docs/assembly-walkthrough.md` and `docs/generated-disassembly.txt`.

8. In `asm_add_i64`, which register holds each argument, and which register holds the return value? Why is that the System V rule rather than a choice the function made up?
9. Draw the factorial stack frame after the prologue. Label the return address, the saved `%rbp`, and the saved `%rbx`.
10. Which registers in `asm_sum_i64` is the function allowed to overwrite without saving them? Name one register it would have to save if it used it.
11. Find `asm_add_i64` in the objdump listing. What is the difference between the section offset in that listing and the address the instruction will have while the test is running?

## Instruction cycle

Run `./build/cs2521_sim_demo`.

12. For cycle 1, name the value that was fetched, the opcode that was decoded, and the register write performed by execute.
13. On which cycle does `JZ` first see a zero counter, and what is `r0` at that moment?
14. The teaching `ADD` and an x86 `addq` are not the same instruction format. State one difference in how the program counter moves, and one difference in where the operands live.

## Memory hierarchy

Run `./build/cs2521_locality_demo`. Paste the lines it prints, including the warning.

15. How many bytes is the column-major stride in that run, and how many `int64_t` values fit in a 64-byte cache line?
16. Using spatial locality, explain why the row-major loop can use a cache line more than once and the column-major loop mostly cannot.
17. Your timing note, in a form you could defend:

    On this run, row-major mean was ____ seconds and column-major mean was ____ seconds. I treat that gap as a property of this run: this matrix size, this optimization level, and this machine's caches. I do not treat the ratio as a constant of the C language or of every CPU.

A strong answer to question 17 can say that column-major was slower, that it was not, or that the two were close. The required part is the measurement and the reason you refuse to generalize it.
