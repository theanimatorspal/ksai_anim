#include "app_ui.h"
#include <backend/ui/ui.h>
#include <backend/ui/latex_colors.h>

#define TOP_MENU_POSY -0.975
#define TOP_MENU_POSX 0
#define TOP_MENU_SCALEY 0.045
#define TOP_MENU_SCALEX 2
#define MENU_TEXT_SCALEX 0.05/1.2
#define MENU_TEXT_SCALEY 0.035/1.15
#define TOP_MENU_LEFT_PADDING -0.95

int draw_popup_menu(ui_label lbl, char ch[NO_OF_POPUP_MENUS][KSAI_SMALL_STRING_LENGTH], int count, float aspect, vec2 pos, vk_rsrs *rsrs)
{
	if (count != 0)
	{
		int strlengh = strlen(ch[0]);
		int largest = 0;
		for (int i = 0; i < count; i++)
		{
			if (strlengh < strlen(ch[i]))
			{
				strlengh = strlen(ch[i]);
				largest = i;
			}
		}

		ui_label back = lbl;
		glm_vec3_copy(lbl.nrml_clr, back.txt_clr);
		glm_vec3_copy(lbl.nrml_clr, back.nrml_clr);
		glm_vec3_copy(lbl.nrml_clr, back.hvrd_clr);
		glm_vec3_copy(lbl.nrml_clr, back.slctd_clr);
		strcpy_s(back.text, 100 * sizeof(char), ch[largest]);


		for (int i = 0; i < count; i++)
		{
			glm_vec2_copy((vec2) { pos[0], pos[1] + (i + 1) * 0.08 }, back.ps);
			glm_vec2_copy((vec2) { MENU_TEXT_SCALEX, 1.2 * MENU_TEXT_SCALEY* aspect * 1.02 }, back.scale);
			ui_draw_button(back, rsrs->window);
		}


		for (int i = 0; i < count; i++)
		{
			strcpy_s(lbl.text, sizeof(lbl.text) * sizeof(char), ch[i]);
			glm_vec2_copy((vec2) { pos[0], pos[1] + (i + 1) * 0.08 }, lbl.ps);
			glm_vec2_copy((vec2) { MENU_TEXT_SCALEX, MENU_TEXT_SCALEY *aspect }, lbl.scale);
			if (ui_draw_button(lbl, rsrs->window) == true)
			{
				return i;
			}
		}
	}

	return -1;
}

int draw_file_menu(char ch[NO_OF_TOP_MENUS][NO_OF_POPUP_MENUS][KSAI_SMALL_STRING_LENGTH], int tmenu_count, int popmenu_count[NO_OF_TOP_MENUS], float aspect, vk_rsrs *rsrs)
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
		.ps = {TOP_MENU_POSX, TOP_MENU_POSY},
		.scale = {TOP_MENU_SCALEX, TOP_MENU_SCALEY * scale_factor * aspect},
		.st_typ = lbl_st_UNSELECTED,
		.text = "MENU BAR"
	};
	glm_vec3_copy(menu_bar_color, lbl.hvrd_clr);
	glm_vec3_copy(menu_bar_color, lbl.nrml_clr);
	glm_vec3_copy(menu_bar_color, lbl.slctd_clr);
	glm_vec3_copy(menu_bar_color, lbl.txt_clr);
	ui_draw_button(lbl, rsrs->window);

	static int selected_menu = -1;
	float prev_length = 0;
	static float prev_length_next = 0;
	glm_vec3_copy(menu_text_color, lbl.txt_clr);
	glm_vec3_copy(menu_hover_color, lbl.hvrd_clr);
	glm_vec3_copy(menu_selected_color, lbl.slctd_clr);
	for (int i = 0; i < tmenu_count; i++)
	{
		strcpy_s(lbl.text, sizeof(lbl.text) * sizeof(char), ch[i][0]);
		glm_vec2_copy((vec2) { TOP_MENU_LEFT_PADDING + prev_length, TOP_MENU_POSY }, lbl.ps);
		glm_vec2_copy((vec2) { MENU_TEXT_SCALEX, MENU_TEXT_SCALEY *scale_factor *aspect }, lbl.scale);
		if (ui_draw_button(lbl, rsrs->window) == true)
		{
			if (selected_menu == i)
			{
				selected_menu = -1;
			}
			else
			{
				selected_menu = i;
				prev_length_next = prev_length;
			}
		}
		prev_length += (float) strlen(ch[i][0]) / 100 * 3;
	}

	if (selected_menu != -1)
	{
		ui_draw_button(lbl, rsrs->window);
		int choice = draw_popup_menu(lbl, ch[selected_menu] + 1, popmenu_count[selected_menu], aspect, (vec2) { -0.95 + prev_length_next, -0.97 }, rsrs);
		if (choice != -1)
		{
			selected_menu = -1;
		}
	}
	return 0;
}
