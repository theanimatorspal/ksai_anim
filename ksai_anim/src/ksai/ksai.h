#pragma once
#include <stdint.h>
#include <intrin.h>
#include <vulkan/vulkan.h>
#include <SDL2/SDL.h>
#include <vendor/cglm/struct.h>
#define KSAI_API
#define KSAI_INLINE __forceinline
#define KSAI_MAX_STRING_POSSIBLE_LENGTH_COUNT 1000000
#define KSAI_SMALL_STRING_LENGTH 100
#define KSAI_STRING_ARENA_MEMORY 1 << 12
#define KSAI_FONT_MEMORY 1 << 24
#define BUFFER_MAX 32
#define MAX_FRAMES_IN_FLIGHT 2
#define MAX_BUFFER_SIZE 512
#define MAX_SELECTOR_SIZE 100


#define KSAI_VK_ASSERT(STATEMENT) if(STATEMENT != VK_SUCCESS) { printf("VKerror"); __debugbreak(); }
#define KSAI_SDL_ASSERT(STATEMENT) if(STATEMENT != SDL_TRUE) { printf("SDLerror"); __debugbreak(); }
#define KSAI_U64_MAX 0xffffffff
