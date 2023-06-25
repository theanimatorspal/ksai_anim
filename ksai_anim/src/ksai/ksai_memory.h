#pragma once
#include <stdint.h>
#include <ksai/ksai.h>

typedef struct ksai_Arena {
  uint8_t *bytes;
  size_t written;
  size_t size;
} ksai_Arena;


KSAI_INLINE KSAI_API uint8_t* ksai_Arena__current(const ksai_Arena *in_arena) {
  return in_arena->bytes + in_arena->written;
}

KSAI_INLINE KSAI_API size_t ksai_Arena__length(const ksai_Arena *in_arena) {
  return in_arena->written;
}

KSAI_API void ksai_Arena_init(size_t size, ksai_Arena *out_arena);
KSAI_API void ksai_Arena_reserve(size_t in_size, ksai_Arena *out_arena);
KSAI_API uint8_t *ksai_Arena_allocate(size_t in_size, ksai_Arena *out_arena);
KSAI_API void ksai_Area_free(ksai_Arena *out_arena);
