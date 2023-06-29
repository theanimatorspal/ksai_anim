#include <SDL2/SDL.h>
#include <backend/vulkan/init.h>
#define NO_OF_TOP_MENUS 6
#define NO_OF_POPUP_MENUS 10

int draw_window(char title[100], int rows, vec2 pos, float aspect, vk_rsrs *rsrs, SDL_Event *event, bool *move);
int draw_label_window(char text[100], vec2 pos, vk_rsrs *rsrs, float aspect, float row);
int draw_selector_window(char select[MAX_SELECTOR_SIZE][KSAI_SMALL_STRING_LENGTH], int count, float aspect, vec2 pos, vk_rsrs *rsrs, float row, int *selection);
int draw_selector_integer(int smallest, int highest, float aspect, vec2 pos, vk_rsrs *rsrs, float row, int *selection);
ivec2s draw_file_menu(char ch[NO_OF_TOP_MENUS][NO_OF_POPUP_MENUS][KSAI_SMALL_STRING_LENGTH], int tmenu_count, int popmenu_count[NO_OF_TOP_MENUS], float aspect, vk_rsrs *rsrs, int *running);