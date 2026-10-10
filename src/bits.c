#include "bits.h"

#include <string.h>

uint32_t bits_set(uint32_t value, unsigned index) {
    if (index >= 32u) {
        return value;
    }
    return value | (UINT32_C(1) << index);
}

uint32_t bits_clear(uint32_t value, unsigned index) {
    if (index >= 32u) {
        return value;
    }
    return value & ~(UINT32_C(1) << index);
}

uint32_t bits_toggle(uint32_t value, unsigned index) {
    if (index >= 32u) {
        return value;
    }
    return value ^ (UINT32_C(1) << index);
}

int bits_inspect(uint32_t value, unsigned index) {
    if (index >= 32u) {
        return -1;
    }
    return (int)((value >> index) & 1u);
}

uint32_t bits_count(uint32_t value) {
    uint32_t count = 0;
    while (value != 0u) {
        count += value & 1u;
        value >>= 1u;
    }
    return count;
}

uint32_t bits_rotate_left(uint32_t value, unsigned amount) {
    unsigned shift = amount % 32u;
    if (shift == 0u) {
        return value;
    }
    return (value << shift) | (value >> (32u - shift));
}

uint32_t bits_rotate_right(uint32_t value, unsigned amount) {
    unsigned shift = amount % 32u;
    if (shift == 0u) {
        return value;
    }
    return (value >> shift) | (value << (32u - shift));
}

uint32_t bits_extract(uint32_t value, unsigned lsb, unsigned width) {
    if (width == 0u || lsb >= 32u || width > 32u - lsb) {
        return 0u;
    }
    uint32_t field = value >> lsb;
    if (width == 32u) {
        return field;
    }
    uint32_t mask = (UINT32_C(1) << width) - 1u;
    return field & mask;
}

int bits_host_endianness(void) {
    uint32_t probe = 0x01020304u;
    unsigned char bytes[sizeof probe];
    memcpy(bytes, &probe, sizeof probe);
    if (bytes[0] == 0x04u) {
        return BITS_ENDIAN_LITTLE;
    }
    if (bytes[0] == 0x01u) {
        return BITS_ENDIAN_BIG;
    }
    return BITS_ENDIAN_UNKNOWN;
}
