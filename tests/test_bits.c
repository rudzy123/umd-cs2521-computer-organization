#include "bits.h"
#include "test_harness.h"

#include <stdint.h>
#include <string.h>

static void test_set_clear_toggle_inspect(void) {
    uint32_t word = 0;
    word = bits_set(word, 0);
    EXPECT(word == 1u);
    EXPECT(bits_inspect(word, 0) == 1);
    word = bits_set(word, 31);
    EXPECT(word == 0x80000001u);
    EXPECT(bits_inspect(word, 31) == 1);
    EXPECT(bits_inspect(word, 1) == 0);

    word = bits_clear(word, 0);
    EXPECT(word == 0x80000000u);
    EXPECT(bits_inspect(word, 0) == 0);

    word = bits_toggle(word, 31);
    EXPECT(word == 0u);
    word = bits_toggle(0xAu, 1);
    EXPECT(word == 0x8u);
    word = bits_toggle(word, 1);
    EXPECT(word == 0xAu);

    EXPECT(bits_set(0x1234u, 32) == 0x1234u);
    EXPECT(bits_set(0x1234u, 1000) == 0x1234u);
    EXPECT(bits_clear(0x1234u, 32) == 0x1234u);
    EXPECT(bits_toggle(0x1234u, 32) == 0x1234u);
    EXPECT(bits_inspect(0x1u, 32) == -1);
    EXPECT(bits_inspect(0x1u, 31) == 0);
    EXPECT(bits_clear(1u, 0) == 0u);
}

static void test_count(void) {
    EXPECT(bits_count(0u) == 0u);
    EXPECT(bits_count(1u) == 1u);
    EXPECT(bits_count(0x80000000u) == 1u);
    EXPECT(bits_count(0xF0F0F0F0u) == 16u);
    EXPECT(bits_count(0xFFFFFFFFu) == 32u);
}

static void test_rotate(void) {
    uint32_t value = 0x80000001u;
    unsigned amount;
    EXPECT(bits_rotate_left(value, 1) == 0x00000003u);
    EXPECT(bits_rotate_right(value, 1) == 0xC0000000u);
    EXPECT(bits_rotate_left(value, 0) == value);
    EXPECT(bits_rotate_right(value, 0) == value);
    EXPECT(bits_rotate_left(value, 32) == value);
    EXPECT(bits_rotate_right(value, 32) == value);
    EXPECT(bits_rotate_left(value, 64) == value);
    EXPECT(bits_rotate_right(bits_rotate_left(0xA5A5A5A5u, 7), 7) == 0xA5A5A5A5u);
    for (amount = 0; amount < 40u; ++amount) {
        uint32_t turned = bits_rotate_left(0x13579BDFu, amount);
        EXPECT(bits_rotate_right(turned, amount) == 0x13579BDFu);
    }
}

static void test_extract(void) {
    EXPECT(bits_extract(0xC2u, 4, 4) == 12u);
    EXPECT(bits_extract(0xC2u, 0, 8) == 0xC2u);
    EXPECT(bits_extract(0x80000000u, 31, 1) == 1u);
    EXPECT(bits_extract(0x80000000u, 31, 2) == 0u);
    EXPECT(bits_extract(0xFFFFFFFFu, 0, 0) == 0u);
    EXPECT(bits_extract(0xA5A5A5A5u, 0, 32) == 0xA5A5A5A5u);
    EXPECT(bits_extract(0xA5A5A5A5u, 1, 32) == 0u);
    EXPECT(bits_extract(0xA5A5A5A5u, 40, 1) == 0u);
    EXPECT(bits_extract(0x00F00000u, 20, 4) == 0xFu);
}

static void test_endianness(void) {
    uint32_t probe = 0x01020304u;
    unsigned char bytes[sizeof probe];
    memcpy(bytes, &probe, sizeof probe);
    EXPECT(bits_host_endianness() == BITS_ENDIAN_LITTLE);
    EXPECT(bytes[0] == 0x04u);
    EXPECT(bytes[1] == 0x03u);
    EXPECT(bytes[2] == 0x02u);
    EXPECT(bytes[3] == 0x01u);
}

int main(void) {
    test_set_clear_toggle_inspect();
    test_count();
    test_rotate();
    test_extract();
    test_endianness();
    return test_finish();
}
