#include "bits.h"

#include <stdio.h>
#include <stdint.h>

static void print_word(const char *label, uint32_t value) {
    int bit;
    printf("%-18s 0x%08X  ", label, value);
    for (bit = 31; bit >= 0; --bit) {
        if (bit < 31 && bit % 4 == 3) {
            putchar(' ');
        }
        putchar(bits_inspect(value, (unsigned)bit) ? '1' : '0');
    }
    printf("  popcount=%u\n", bits_count(value));
}

int main(void) {
    uint32_t word = 0;
    int endian = bits_host_endianness();
    const char *endian_name = "neither a pure little-endian nor big-endian layout";

    if (endian == BITS_ENDIAN_LITTLE) {
        endian_name = "little-endian";
    } else if (endian == BITS_ENDIAN_BIG) {
        endian_name = "big-endian";
    }

    puts("Bit manipulation");
    puts("Index 0 is the least significant bit. A 32-bit word is the unit.");
    print_word("start", word);
    word = bits_set(word, 0);
    word = bits_set(word, 3);
    print_word("set 0 and 3", word);
    word = bits_toggle(word, 3);
    print_word("toggle 3", word);
    word = bits_clear(word, 0);
    print_word("clear 0", word);

    word = 0x80000001u;
    print_word("pattern", word);
    print_word("rotate left 1", bits_rotate_left(word, 1));
    print_word("rotate right 1", bits_rotate_right(word, 1));
    printf("extract bits [7:4] of 0xC2 = %u\n", bits_extract(0xC2u, 4, 4));
    printf("host byte order: %s\n", endian_name);
    puts("On x86-64 the hardware is little-endian: the low byte sits at the lowest address.");

    if (bits_count(0xFu) != 4u || bits_extract(0xC2u, 4, 4) != 12u) {
        fputs("bit demo sanity check failed\n", stderr);
        return 1;
    }
    return 0;
}
