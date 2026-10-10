#include "repr.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

size_t repr_format_binary_u32(uint32_t value, char *out, size_t out_size) {
    if (out == NULL || out_size < 33u) {
        if (out != NULL && out_size > 0u) {
            out[0] = '\0';
        }
        return 0u;
    }
    for (int bit = 31; bit >= 0; --bit) {
        out[31 - bit] = ((value >> (unsigned)bit) & 1u) ? '1' : '0';
    }
    out[32] = '\0';
    return 32u;
}

size_t repr_format_hex_u32(uint32_t value, char *out, size_t out_size) {
    static const char k_hex[] = "0123456789ABCDEF";
    if (out == NULL || out_size < 11u) {
        if (out != NULL && out_size > 0u) {
            out[0] = '\0';
        }
        return 0u;
    }
    out[0] = '0';
    out[1] = 'x';
    for (int nibble = 7; nibble >= 0; --nibble) {
        unsigned shift = (unsigned)nibble * 4u;
        out[2 + (7 - nibble)] = k_hex[(value >> shift) & 0xFu];
    }
    out[10] = '\0';
    return 10u;
}

int repr_parse_binary_u32(const char *text, uint32_t *out) {
    uint32_t value = 0;
    int digits = 0;
    const char *cursor;
    if (text == NULL || out == NULL) {
        return -1;
    }
    cursor = text;
    if (cursor[0] == '0' && (cursor[1] == 'b' || cursor[1] == 'B')) {
        cursor += 2;
    }
    for (; *cursor != '\0'; ++cursor) {
        if (*cursor == '_') {
            continue;
        }
        if (*cursor != '0' && *cursor != '1') {
            return -1;
        }
        if (digits >= 32) {
            return -1;
        }
        value = (value << 1u) | (uint32_t)(*cursor - '0');
        digits += 1;
    }
    if (digits == 0) {
        return -1;
    }
    *out = value;
    return 0;
}

static int hex_digit(char character) {
    if (character >= '0' && character <= '9') {
        return character - '0';
    }
    if (character >= 'a' && character <= 'f') {
        return character - 'a' + 10;
    }
    if (character >= 'A' && character <= 'F') {
        return character - 'A' + 10;
    }
    return -1;
}

int repr_parse_hex_u32(const char *text, uint32_t *out) {
    uint32_t value = 0;
    int digits = 0;
    const char *cursor;
    if (text == NULL || out == NULL) {
        return -1;
    }
    cursor = text;
    if (cursor[0] == '0' && (cursor[1] == 'x' || cursor[1] == 'X')) {
        cursor += 2;
    }
    for (; *cursor != '\0'; ++cursor) {
        int digit;
        if (*cursor == '_') {
            continue;
        }
        digit = hex_digit(*cursor);
        if (digit < 0 || digits >= 8) {
            return -1;
        }
        value = (value << 4u) | (uint32_t)digit;
        digits += 1;
    }
    if (digits == 0) {
        return -1;
    }
    *out = value;
    return 0;
}

int32_t repr_as_i32(uint32_t bits) {
    if (bits <= (uint32_t)INT32_MAX) {
        return (int32_t)bits;
    }
    if (bits == 0x80000000u) {
        return INT32_MIN;
    }
    {
        uint32_t magnitude = (~bits) + 1u;
        return -(int32_t)magnitude;
    }
}

uint32_t repr_as_u32(int32_t value) {
    uint32_t bits = 0;
    memcpy(&bits, &value, sizeof bits);
    return bits;
}

int repr_add_i32(int32_t left, int32_t right, int32_t *sum) {
    int64_t wide;
    if (sum == NULL) {
        return -1;
    }
    wide = (int64_t)left + (int64_t)right;
    if (wide > INT32_MAX || wide < INT32_MIN) {
        return -1;
    }
    *sum = (int32_t)wide;
    return 0;
}

int repr_add_u32(uint32_t left, uint32_t right, uint32_t *sum) {
    uint64_t wide;
    if (sum == NULL) {
        return -1;
    }
    wide = (uint64_t)left + (uint64_t)right;
    if (wide > UINT32_MAX) {
        return -1;
    }
    *sum = (uint32_t)wide;
    return 0;
}

void repr_explain_i32_add(int32_t left, int32_t right, char *out, size_t out_size) {
    int64_t wide;
    if (out == NULL || out_size == 0u) {
        return;
    }
    wide = (int64_t)left + (int64_t)right;
    if (wide > INT32_MAX || wide < INT32_MIN) {
        uint32_t low = (uint32_t)(uint64_t)wide;
        int32_t wrapped = repr_as_i32(low);
        snprintf(out, out_size,
                 "signed add: %" PRId32 " + %" PRId32 " = %" PRId64
                 ". The sum does not fit in int32_t. "
                 "C signed overflow is undefined behavior, so this result was not "
                 "computed with a signed add. A 32-bit two's-complement adder would "
                 "wrap to %" PRId32 ".",
                 left, right, wide, wrapped);
        return;
    }
    snprintf(out, out_size,
             "signed add: %" PRId32 " + %" PRId32 " = %" PRId32
             ". The sum fits in int32_t.",
             left, right, (int32_t)wide);
}

void repr_explain_u32_add(uint32_t left, uint32_t right, char *out, size_t out_size) {
    uint64_t wide;
    if (out == NULL || out_size == 0u) {
        return;
    }
    wide = (uint64_t)left + (uint64_t)right;
    if (wide > UINT32_MAX) {
        snprintf(out, out_size,
                 "unsigned add: %" PRIu32 " + %" PRIu32 " = %" PRIu64
                 ". The sum does not fit in uint32_t. Unsigned wrap modulo 2^32 "
                 "produces %" PRIu32 ".",
                 left, right, wide, (uint32_t)wide);
        return;
    }
    snprintf(out, out_size,
             "unsigned add: %" PRIu32 " + %" PRIu32 " = %" PRIu32
             ". The sum fits in uint32_t.",
             left, right, (uint32_t)wide);
}
