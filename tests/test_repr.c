#include "repr.h"
#include "test_harness.h"

#include <stdint.h>
#include <string.h>

static void test_format_and_parse(void) {
    char binary[40];
    char hex[16];
    char tiny[4];
    uint32_t parsed = 1u;

    EXPECT(repr_format_binary_u32(10u, binary, sizeof binary) == 32u);
    EXPECT(strcmp(binary, "00000000000000000000000000001010") == 0);
    EXPECT(repr_format_hex_u32(0xDEADBEEFu, hex, sizeof hex) == 10u);
    EXPECT(strcmp(hex, "0xDEADBEEF") == 0);
    EXPECT(repr_format_hex_u32(0u, hex, sizeof hex) == 10u);
    EXPECT(strcmp(hex, "0x00000000") == 0);

    tiny[0] = 'Z';
    EXPECT(repr_format_binary_u32(1u, tiny, sizeof tiny) == 0u);
    EXPECT(tiny[0] == '\0');
    EXPECT(repr_format_binary_u32(1u, NULL, 40) == 0u);
    EXPECT(repr_format_hex_u32(1u, hex, 10) == 0u);

    EXPECT(repr_parse_binary_u32("0b1010", &parsed) == 0);
    EXPECT(parsed == 10u);
    EXPECT(repr_parse_binary_u32("0B_0101", &parsed) == 0);
    EXPECT(parsed == 5u);
    EXPECT(repr_parse_binary_u32(binary, &parsed) == 0);
    EXPECT(parsed == 10u);
    EXPECT(repr_parse_binary_u32("0b", &parsed) == -1);
    EXPECT(repr_parse_binary_u32("", &parsed) == -1);
    EXPECT(repr_parse_binary_u32("102", &parsed) == -1);
    EXPECT(repr_parse_binary_u32("111111111111111111111111111111111", &parsed) == -1);
    EXPECT(repr_parse_binary_u32(NULL, &parsed) == -1);
    EXPECT(parsed == 10u);

    EXPECT(repr_parse_hex_u32("0xDEADBEEF", &parsed) == 0);
    EXPECT(parsed == 0xDEADBEEFu);
    EXPECT(repr_parse_hex_u32("ff", &parsed) == 0);
    EXPECT(parsed == 255u);
    EXPECT(repr_parse_hex_u32("0x0000000F", &parsed) == 0);
    EXPECT(parsed == 15u);
    EXPECT(repr_parse_hex_u32("0x1_0000_0000", &parsed) == -1);
    EXPECT(repr_parse_hex_u32("0x", &parsed) == -1);
    EXPECT(repr_parse_hex_u32("0xzz", &parsed) == -1);
    EXPECT(repr_parse_hex_u32(NULL, &parsed) == -1);
    EXPECT(parsed == 15u);
}

static void test_twos_complement_bits(void) {
    EXPECT(repr_as_i32(0u) == 0);
    EXPECT(repr_as_i32(42u) == 42);
    EXPECT(repr_as_i32(0xFFFFFFFFu) == -1);
    EXPECT(repr_as_i32(0x80000000u) == INT32_MIN);
    EXPECT(repr_as_i32(0x80000001u) == INT32_MIN + 1);
    EXPECT(repr_as_u32(0) == 0u);
    EXPECT(repr_as_u32(-1) == 0xFFFFFFFFu);
    EXPECT(repr_as_u32(INT32_MIN) == 0x80000000u);
    EXPECT(repr_as_i32(repr_as_u32(-42)) == -42);
}

static void test_checked_add(void) {
    int32_t signed_sum = 123;
    uint32_t unsigned_sum = 123u;
    char explanation[320];

    EXPECT(repr_add_i32(7, 35, &signed_sum) == 0);
    EXPECT(signed_sum == 42);
    EXPECT(repr_add_i32(-10, 3, &signed_sum) == 0);
    EXPECT(signed_sum == -7);
    EXPECT(repr_add_i32(INT32_MAX, 0, &signed_sum) == 0);
    EXPECT(signed_sum == INT32_MAX);

    signed_sum = 123;
    EXPECT(repr_add_i32(INT32_MAX, 1, &signed_sum) == -1);
    EXPECT(signed_sum == 123);
    EXPECT(repr_add_i32(INT32_MIN, -1, &signed_sum) == -1);
    EXPECT(signed_sum == 123);
    EXPECT(repr_add_i32(INT32_MIN, INT32_MIN, &signed_sum) == -1);
    EXPECT(repr_add_i32(1, 2, NULL) == -1);

    EXPECT(repr_add_u32(100u, 23u, &unsigned_sum) == 0);
    EXPECT(unsigned_sum == 123u);
    EXPECT(repr_add_u32(UINT32_MAX, 0u, &unsigned_sum) == 0);
    EXPECT(unsigned_sum == UINT32_MAX);
    unsigned_sum = 123u;
    EXPECT(repr_add_u32(UINT32_MAX, 1u, &unsigned_sum) == -1);
    EXPECT(unsigned_sum == 123u);
    EXPECT(repr_add_u32(1u, UINT32_MAX, &unsigned_sum) == -1);
    EXPECT(repr_add_u32(1u, 1u, NULL) == -1);

    repr_explain_i32_add(INT32_MAX, 1, explanation, sizeof explanation);
    EXPECT(strstr(explanation, "does not fit") != NULL);
    EXPECT(strstr(explanation, "undefined behavior") != NULL);
    repr_explain_i32_add(20, 22, explanation, sizeof explanation);
    EXPECT(strstr(explanation, "fits") != NULL);
    repr_explain_u32_add(UINT32_MAX, 1u, explanation, sizeof explanation);
    EXPECT(strstr(explanation, "modulo 2^32") != NULL);
    repr_explain_u32_add(1u, 1u, explanation, sizeof explanation);
    EXPECT(strstr(explanation, "fits") != NULL);

    explanation[0] = 'Z';
    repr_explain_i32_add(1, 1, explanation, 1);
    EXPECT(explanation[0] == '\0');
    repr_explain_i32_add(1, 1, NULL, 8);
    repr_explain_u32_add(1u, 1u, NULL, 8);
}

int main(void) {
    test_format_and_parse();
    test_twos_complement_bits();
    test_checked_add();
    return test_finish();
}
