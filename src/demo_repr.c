#include "repr.h"

#include <stdio.h>
#include <stdint.h>

static void print_bits(uint32_t bits) {
    char binary[33];
    char hex[11];
    int index;
    repr_format_binary_u32(bits, binary, sizeof binary);
    repr_format_hex_u32(bits, hex, sizeof hex);
    printf("  bits %s  ", hex);
    for (index = 0; index < 32; ++index) {
        if (index > 0 && index % 4 == 0) {
            putchar(' ');
        }
        putchar(binary[index]);
    }
    printf("\n  two's complement int32: %d\n", repr_as_i32(bits));
}

static void show_signed(int32_t left, int32_t right) {
    char text[384];
    int32_t sum = 0;
    int status = repr_add_i32(left, right, &sum);
    repr_explain_i32_add(left, right, text, sizeof text);
    printf("%s\n", text);
    if (status == 0) {
        printf("  checked result: %d\n", sum);
    }
}

static void show_unsigned(uint32_t left, uint32_t right) {
    char text[384];
    uint32_t sum = 0;
    int status = repr_add_u32(left, right, &sum);
    repr_explain_u32_add(left, right, text, sizeof text);
    printf("%s\n", text);
    if (status == 0) {
        printf("  checked result: %u\n", sum);
    }
}

int main(void) {
    char hex[11];
    uint32_t parsed = 0;

    puts("Integer representation");
    puts("Decimal is base 10, binary is base 2, and hexadecimal is base 16.");
    puts("One hex digit is exactly four bits.");
    puts("");
    puts("42 as an unsigned 32-bit pattern:");
    print_bits(42u);
    puts("-42 stored as two's complement:");
    print_bits(repr_as_u32(-42));
    puts("INT32_MIN, the sign bit alone:");
    print_bits(repr_as_u32(INT32_MIN));
    puts("");
    puts("Checked addition never executes a signed overflow in C.");
    show_signed(20, 22);
    show_signed(INT32_MAX, 1);
    show_signed(INT32_MIN, -1);
    show_unsigned(100u, 23u);
    show_unsigned(UINT32_MAX, 1u);

    if (repr_format_hex_u32(0xA5A5A5A5u, hex, sizeof hex) != 10u ||
        repr_parse_hex_u32(hex, &parsed) != 0 || parsed != 0xA5A5A5A5u) {
        fputs("representation demo sanity check failed\n", stderr);
        return 1;
    }
    return 0;
}
