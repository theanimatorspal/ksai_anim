#include "app_ui.h"
#include <backend/ui/ui.h>
#include <backend/ui/latex_colors.h>



int draw_file_menu(char ch[NO_OF_TOP_MENUS][NO_OF_POPUP_MENUS][KSAI_SMALL_STRING_LENGTH], int tmenu_count, int popmenu_count[NO_OF_TOP_MENUS], float aspect, vk_rsrs *rsrs, SDL_Event *event)
{
	/* Decoration */
	static vec3 menu_bar_color;
	static vec3 menu_hover_color;
	static vec3 menu_text_color;
	static vec3 menu_selected_color;
	static int first_time = true;
	if (first_time == true)
	{
		glm_vec3_copy(color_DARKTAUPE, menu_bar_color);
		glm_vec3_copy(color_PALEBROWN, menu_hover_color);
		glm_vec3_copy(color_CREAM, menu_text_color);
		glm_vec3_copy(color_RED, menu_selected_color);
	}

	float scale_factor = 0.8;
	ui_label lbl = (ui_label){
		.typ = BUTTON,
		.ps = {0, -0.975},
		.scale = {2, 0.045 * scale_factor * aspect},
		.st_typ = lbl_st_UNSELECTED,
		.text = "MENU BAR"
	};
	glm_vec3_copy(menu_bar_color, lbl.hvrd_clr);
	glm_vec3_copy(menu_bar_color, lbl.nrml_clr);
	glm_vec3_copy(menu_bar_color, lbl.slctd_clr);
	glm_vec3_copy(menu_bar_color, lbl.txt_clr);
	ui_draw_button(lbl, rsrs->window, NULL);

	static int selected_menu = -1;
	float prev_length = 0;
	glm_vec3_copy(menu_text_color, lbl.txt_clr);
	glm_vec3_copy(menu_hover_color, lbl.hvrd_clr);
	glm_vec3_copy(menu_selected_color, lbl.slctd_clr);
	for (int i = 0; i < tmenu_count; i++)
	{
		strcpy_s(lbl.text, sizeof(lbl.text) * sizeof(char), ch[i][0]);
		glm_vec2_copy((vec2) { -0.95 + prev_length, -0.97 }, lbl.ps);
		glm_vec2_copy((vec2) { 0.05 / 1.2, 0.035 / 1.15 * scale_factor * aspect }, lbl.scale);
		if (ui_draw_button(lbl, rsrs->window, event) == true)
		{
			if(selected_menu == i)
				selected_menu = -1;
			else
				selected_menu = i;
		}
		prev_length += (float)strlen(ch[i][0]) / 100 * 3;
	}

	if (selected_menu != -1)
	{
		printf("Selected: %d\n", selected_menu);
		selected_menu = -1;
	}
}
