#ifndef CS2521_REPR_H
#define CS2521_REPR_H

#include <stddef.h>
#include <stdint.h>

/*
 * Formatting and checked arithmetic for the representation chapter.
 * Signed results are computed in a wider type so this module never executes
 * a C signed overflow. Unsigned results are computed in uint64_t, then range-checked.
 *
 * Successful add functions write *sum and return 0.
 * Overflow leaves *sum unchanged and returns -1.
 * A null output pointer returns -1.
 */

size_t repr_format_binary_u32(uint32_t value, char *out, size_t out_size);
size_t repr_format_hex_u32(uint32_t value, char *out, size_t out_size);

/* Accept an optional 0b/0x prefix and underscores. Return 0 on success, -1 otherwise. */
int repr_parse_binary_u32(const char *text, uint32_t *out);
int repr_parse_hex_u32(const char *text, uint32_t *out);

int32_t repr_as_i32(uint32_t bits);
uint32_t repr_as_u32(int32_t value);

int repr_add_i32(int32_t left, int32_t right, int32_t *sum);
int repr_add_u32(uint32_t left, uint32_t right, uint32_t *sum);

void repr_explain_i32_add(int32_t left, int32_t right, char *out, size_t out_size);
void repr_explain_u32_add(uint32_t left, uint32_t right, char *out, size_t out_size);

#endif
