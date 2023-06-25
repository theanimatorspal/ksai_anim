#include "ksai_memory.h"
#include <Windows.h>


KSAI_API void ksai_Arena_init(size_t size, ksai_Arena *out_arena) {

        out_arena->bytes = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        out_arena->size = size;
        out_arena->written = 0;
}

KSAI_API void ksai_Arena_reserve(size_t in_size, ksai_Arena *out_arena) {
        out_arena->written += in_size;
}

KSAI_API uint8_t *ksai_Arena_allocate(size_t in_size, ksai_Arena *out_arena) {
        size_t size = (size_t)ksai_Arena__current(out_arena);
        if (out_arena->written + in_size > out_arena->size) 
        {
        }
        out_arena->written += in_size;
        return (uint8_t *)size;
}

KSAI_API void ksai_Area_free(ksai_Arena *out_arena)
{
       VirtualFree(out_arena->bytes, out_arena->size, MEM_DECOMMIT);
}
