#!/usr/bin/env python3
"""Show binary, hexadecimal, and two's-complement forms of an integer.

This is a whiteboard for the representation chapter. The checked arithmetic
used by the C library lives in src/repr.c. Example:

    python3 scripts/bit_lab.py 42
    python3 scripts/bit_lab.py 0x2A
    python3 scripts/bit_lab.py -- -42
"""

from __future__ import annotations

import argparse
import sys

INT32_MIN = -2147483648
INT32_MAX = 2147483647
MASK32 = 0xFFFFFFFF


def grouped(pattern: str) -> str:
    """Group bits into nibbles, padding on the left so the groups match hex digits."""
    pad = (-len(pattern)) % 4
    padded = ("0" * pad) + pattern
    return " ".join(padded[index : index + 4] for index in range(0, len(padded), 4))


def hex_text(value: int) -> str:
    if value < 0:
        return "-0x" + format(-value, "X")
    return "0x" + format(value, "X")


def parse_value(text: str) -> int:
    return int(text, 0)


def show(value: int) -> None:
    print(f"decimal: {value}")
    if value < 0:
        print(f"binary:  -{grouped(format(abs(value), 'b'))}")
    else:
        print(f"binary:  {grouped(format(value, 'b'))}")
    print(f"hex:     {hex_text(value)}")

    if not INT32_MIN <= value <= INT32_MAX:
        print("This value does not fit in a signed 32-bit word.")
        return

    bits = value & MASK32
    pattern = grouped(format(bits, "032b"))
    print(f"int32 bits: {pattern}  (0x{bits:08X})")
    if value >= 0:
        print("The sign bit is 0, so the two's-complement reading is the same magnitude.")
        return

    magnitude = -value
    magnitude_bits = magnitude & MASK32
    inverted = (~magnitude_bits) & MASK32
    twos = (inverted + 1) & MASK32
    print("Two's complement, 32 bits:")
    print(f"  magnitude: {grouped(format(magnitude_bits, '032b'))}")
    print(f"  invert:    {grouped(format(inverted, '032b'))}")
    print(f"  add 1:     {grouped(format(twos, '032b'))}")
    if value == INT32_MIN:
        print("  INT32_MIN is the negative value whose magnitude does not fit in int32.")
        print("  The invert-and-add-1 steps still produce its sign-bit pattern.")


def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("values", nargs="+", help="integers in decimal, 0x hex, or 0b binary")
    args = parser.parse_args(argv[1:])
    try:
        numbers = [parse_value(text) for text in args.values]
    except ValueError as exc:
        print(f"could not parse an integer: {exc}", file=sys.stderr)
        return 1
    for index, number in enumerate(numbers):
        if index:
            print()
        show(number)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
