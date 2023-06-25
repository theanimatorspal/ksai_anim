#include <string.h>
#include <stdbool.h>
#include <ksai/ksai_memory.h>
#include "ksai_primitives.h"

static ksai_Arena global_ksai_string_arena;
static bool first_call = true;

KSAI_API void ksai_String_create(ksai_String *out_str) {
}

KSAI_API void ksai_String_create_from_carry(const char *in_str, ksai_String *out_str) {
	size_t length = strnlen(in_str, KSAI_MAX_STRING_POSSIBLE_LENGTH_COUNT);
	if (first_call == true)
	{
		ksai_Arena_init(KSAI_STRING_ARENA_MEMORY, &global_ksai_string_arena);
		first_call = false;
	}
	out_str->data = (char*)ksai_Arena_allocate(length * sizeof(char) * 2, &global_ksai_string_arena);
	out_str->length = length;
	out_str->size = length * sizeof(char) * 2;
	strcpy_s(out_str->data, sizeof(out_str->data) * length, in_str);
}

KSAI_API void ksai_String_destroy(ksai_String *out_str) {
	out_str->length = 0;	
	out_str->size = 0;
}

KSAI_API void ksai_Strings_destroy()
{
	ksai_Area_free(&global_ksai_string_arena);
}

