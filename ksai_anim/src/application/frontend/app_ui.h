#include <SDL2/SDL.h>
#include <backend/vulkan/init.h>
#include <engine/objects/object.h>
#include <engine/renderer/scene.h>
#include <backend/vulkan/backend.h>
#include <backend/vulkan/offscreen.h>
#include <vendor/cglm/cglm.h>
#include <SDL2/SDL.h>
#define NO_OF_TOP_MENUS 6
#define NO_OF_POPUP_MENUS 10

int draw_window(char title[100], int rows, vec2 pos, float aspect, vk_rsrs *rsrs, SDL_Event *event, bool *move);
int draw_label_window(char text[100], vec2 pos, vk_rsrs *rsrs, float aspect, float row);
int draw_button_window(char text[100], vec2 pos, vk_rsrs *rsrs, float aspect, float row);
int draw_selector_window(char select[MAX_SELECTOR_SIZE][KSAI_SMALL_STRING_LENGTH], int count, float aspect, vec2 pos, vk_rsrs *rsrs, float row, int *selection);
int draw_selector_integer(int smallest, int highest, float aspect, vec2 pos, vk_rsrs *rsrs, float row, int *selection);
int draw_selector_var(int *selected, float aspect, vec2 pos, vk_rsrs *rsrs, float row, int args, ...);
int draw_input_number(float aspect, vec2 pos, vk_rsrs *rsrs, float row, char out[KSAI_SMALL_STRING_LENGTH], bool *should_input, int *place_value);
ivec2s draw_file_menu(char ch[NO_OF_TOP_MENUS][NO_OF_POPUP_MENUS][KSAI_SMALL_STRING_LENGTH], int tmenu_count, int popmenu_count[NO_OF_TOP_MENUS], float aspect, vk_rsrs *rsrs, int *running);
void draw_timeline(float aspect, vk_rsrs *rsrs, int *current_frame, int *low_range, int *high_range, kie_Scene *scene, int current_selected);


void handle_file_menu(
	ivec2s file_option,
	float aspect,
	vk_rsrs *rsrs,
	SDL_Event *event,
	kie_Scene *scene,
	renderer_backend *backend,
	int *current_selected,
	vec3 clear_color,
	int viewport_objects_count,
	bool *should_show_viewport_objects,
	kie_Camera *camera,
	int *viewport_pipeline
);
