#ifndef CS2521_BITS_H
#define CS2521_BITS_H

#include <stdint.h>

#define BITS_ENDIAN_BIG 0
#define BITS_ENDIAN_LITTLE 1
#define BITS_ENDIAN_UNKNOWN (-1)

/*
 * Bit utilities for a 32-bit word.
 * Index 0 is the least significant bit. An index greater than 31 is rejected:
 * set, clear, and toggle return the original word, and inspect returns -1.
 */

uint32_t bits_set(uint32_t value, unsigned index);
uint32_t bits_clear(uint32_t value, unsigned index);
uint32_t bits_toggle(uint32_t value, unsigned index);
int bits_inspect(uint32_t value, unsigned index);
uint32_t bits_count(uint32_t value);
uint32_t bits_rotate_left(uint32_t value, unsigned amount);
uint32_t bits_rotate_right(uint32_t value, unsigned amount);

/*
 * Keep `width` bits starting at `lsb`. Returns 0 when the field does not fit
 * in the word, including a zero width. A width of 32 at lsb 0 returns `value`.
 */
uint32_t bits_extract(uint32_t value, unsigned lsb, unsigned width);

/* 1 little-endian, 0 big-endian, -1 if the first byte matches neither layout. */
int bits_host_endianness(void);

#endif
