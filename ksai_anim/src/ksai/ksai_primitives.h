#include <stdint.h>
#include <ksai/ksai.h>

typedef struct ksai_String
{
	uint32_t length;
	uint32_t size;
	char* data;
} ksai_String;

KSAI_INLINE KSAI_API size_t ksai_String__length(const ksai_String *in_str) {
	return in_str->length;
}

KSAI_API void ksai_String_create_from_carry(const char *in_str, ksai_String *out_str);
KSAI_API void ksai_String_destroy(ksai_String *out_str);
KSAI_API void ksai_Strings_destroy();
