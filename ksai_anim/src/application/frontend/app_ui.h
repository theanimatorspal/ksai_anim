#include <SDL2/SDL.h>
#include <backend/vulkan/init.h>
#define NO_OF_TOP_MENUS 6
#define NO_OF_POPUP_MENUS 10
int draw_file_menu(char ch[NO_OF_TOP_MENUS][NO_OF_POPUP_MENUS], int tmenu_count, int popmenu_count[NO_OF_TOP_MENUS], float aspect, vk_rsrs *rsrs, SDL_Event *event);