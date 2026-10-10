# Binary cheat sheet

## Base 2

A binary digit, a bit, is 0 or 1. Moving one place to the left multiplies by two, just as moving one decimal place to the left multiplies by ten.

| Place | 128 | 64 | 32 | 16 | 8 | 4 | 2 | 1 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Bit | 1 | 0 | 1 | 0 | 1 | 1 | 0 | 0 |

`10101100` is `128 + 32 + 8 + 4 = 172`.

Powers of two show up everywhere in this lab: word sizes, immediate ranges, and cache lines.

| Power | Value |
| --- | --- |
| 2^0 | 1 |
| 2^3 | 8 |
| 2^4 | 16 |
| 2^8 | 256 |
| 2^10 | 1024 |
| 2^16 | 65536 |
| 2^31 | 2147483648 |
| 2^32 | 4294967296 |

Index 0 in `include/bits.h` is the least significant bit, the ones place. Bit 31 of a `uint32_t` is the 2^31 place.

## Base 16

Hexadecimal groups bits by fours, because 2^4 is 16. Each hex digit is one nibble.

| Hex | Binary | Decimal |
| --- | --- | --- |
| 0 | 0000 | 0 |
| 1 | 0001 | 1 |
| 9 | 1001 | 9 |
| A | 1010 | 10 |
| F | 1111 | 15 |

`172` is `0xAC`: `A` is 1010 and `C` is 1100, so `0xAC` is `10101100`. The other direction is the same grouping. `0x2A` is `0010 1010`, which is `32 + 8 + 2 = 42`.

`repr_format_binary_u32` writes all 32 bits. `repr_format_hex_u32` writes `0x` and eight digits. The parsers accept an optional `0b` or `0x` prefix and underscores between digits. A binary literal may contain at most 32 bits. A hex literal may contain at most eight digits.

`scripts/bit_lab.py` prints the same three views from the command line.

## Boolean operations

Boolean operations combine one bit with another bit. In C they are the operators behind every mask in `src/bits.c`.

| A | B | A AND B | A OR B | A XOR B | NOT A |
| --- | --- | --- | --- | --- | --- |
| 0 | 0 | 0 | 0 | 0 | 1 |
| 0 | 1 | 0 | 1 | 1 | 1 |
| 1 | 0 | 0 | 1 | 1 | 0 |
| 1 | 1 | 1 | 1 | 0 | 0 |

AND keeps a bit only when both inputs have it. OR turns a bit on when either input has it. XOR turns a bit on when the inputs differ. NOT flips every bit.

Those four operations are the whole bit API:

| Goal | Boolean operation | C expression |
| --- | --- | --- |
| Set bit `n` | OR with a 1 in that place | `value \| (1u << n)` |
| Clear bit `n` | AND with a 0 in that place | `value & ~(1u << n)` |
| Toggle bit `n` | XOR with a 1 in that place | `value ^ (1u << n)` |
| Inspect bit `n` | shift, then AND with 1 | `(value >> n) & 1u` |

An index of 32 or more is not a bit in the word. Set, clear, and toggle return the original word. Inspect returns `-1`.

`bits_count` walks the word and adds the least significant bit, then shifts. `0xFFFFFFFF` has 32 set bits. `0` has none.

`bits_extract` shifts the field down to bit 0 and masks it. The field must lie inside the word. A request for bits that would pass bit 31 returns 0. The implementation never shifts a 32-bit value by 32, which would be undefined in C.

A rotate moves bits off one end and feeds them back in the other end. Rotate-left of `0x80000001` by one is `0x00000003`: the old sign bit becomes the new ones bit. Rotating by 0 or by a multiple of 32 is the identity. Rotate-left and rotate-right undo each other.

## Two's complement

Unsigned integers use every bit as magnitude. Signed integers in this lab use two's complement. The high bit is the sign bit. A pattern with the sign bit clear is non-negative and has the same value as the unsigned reading. A pattern with the sign bit set is negative.

To encode a negative number:

1. Write the positive magnitude in binary.
2. Invert every bit.
3. Add 1.

In eight bits, `-13` is built from `13 = 00001101`:

```text
magnitude  00001101
invert     11110010
add 1      11110011
```

`11110011` is `0xF3`. Interpreted as a signed byte, it is `-13`.

The same steps in 32 bits produce the patterns `repr_as_i32` and `repr_as_u32` convert. The conversion copies bits or rebuilds the value with unsigned arithmetic. It does not cast a negative bit pattern through an out-of-range signed conversion.

`INT32_MIN` is the awkward value. Its magnitude, 2147483648, does not fit in a positive `int32_t`. The bit pattern is the sign bit alone: `0x80000000`. Negating that pattern in signed C is undefined, so the code treats this one pattern as a special case.

A 32-bit two's-complement word holds `-2147483648` through `2147483647`. A 32-bit unsigned word holds `0` through `4294967295`.

## Overflow, without undefined behavior

Unsigned overflow is defined. The sum is computed modulo 2^32. `UINT32_MAX + 1` wraps to 0.

Signed overflow is undefined behavior in C. The compiler is allowed to assume it never happens. `repr_add_i32` therefore adds in `int64_t`, which can hold every sum of two `int32_t` values, and rejects a sum outside the `int32_t` range. The original operands are never added with a signed 32-bit operation that would overflow.

The explanation string still reports the low 32 bits a hardware adder would produce. `INT32_MAX + 1` has the mathematical sum `2147483648`. The wrapped two's-complement result is `INT32_MIN`. That wrapped pattern is built from unsigned bits, then interpreted. The signed add itself is not executed.

`repr_add_u32` adds in `uint64_t` and reports a sum above `UINT32_MAX` the same way. On overflow, both functions leave the caller's result variable unchanged.

## Endianness

Endianness is the order of bytes in memory, not the order of values in arithmetic.

For the 32-bit pattern `0x01020304`:

```text
little-endian addresses: 04 03 02 01
big-endian addresses:    01 02 03 04
```

x86-64 is little-endian. `bits_host_endianness` copies a `uint32_t` into an array of bytes with `memcpy` and looks at the first byte. The test does that copy again on its own, so the check is the observed byte order rather than a hardcoded address.
