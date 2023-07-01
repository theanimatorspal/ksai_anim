#include "app_ui.h"
#include <ksai/ksai.h>
#include <backend/ui/ui.h>
#include <backend/ui/latex_colors.h>

/* COLORS */
#define BUTTON_HOVER_COLOR color_RUDDYBROWN
#define MENU_BAR_COLOR color_DARKTAUPE
#define MENU_HOVER_COLOR color_PALEBROWN
#define MENU_TEXT_COLOR color_CREAM
#define MENU_SELECTED_COLOR color_RED
#define MENU_CLOSE_COLOR color_RED_PIGMENT_
#define DEBUG_TEXT_COLOR color_BLACK
#define SELECTOR_COLOR color_RUSSET


#define TOP_MENU_POSY -0.975
#define TOP_MENU_POSX 0
#define TOP_MENU_SCALEY 0.045
#define TOP_MENU_SCALEX 2
#define MENU_TEXT_SCALEX 0.05/1.2
#define MENU_TEXT_SCALEY 0.035/1.15
#define BACK_SCALEY 1.24
#define TOP_MENU_LEFT_PADDING -0.95

int draw_window(char title[KSAI_SMALL_STRING_LENGTH], int rows, vec2 pos, float aspect, vk_rsrs *rsrs, SDL_Event *event, bool *move)
{
	ui_label back;

	strcpy_s(back.text, KSAI_SMALL_STRING_LENGTH * sizeof(char), "DEBUG XXXX ");
	back.st_typ = 0;
	back.typ = BUTTON;
	if (*move == true)
	{
		int width, height;
		int x, y;
		SDL_GetWindowSize(rsrs->window, &width, &height);
		SDL_GetMouseState(&x, &y);
		float ms_x = 2 * (float) x / width - 1;
		float ms_y = (2 * (float) y / height - 1);
		pos[0] = ms_x;
		pos[1] = ms_y;
	}
	for (int i = 0; i < rows; i++)
	{

		glm_vec2_copy((vec2) { pos[0], pos[1] + (i) * 0.08 }, back.ps);
		glm_vec2_copy((vec2) { MENU_TEXT_SCALEX, BACK_SCALEY *MENU_TEXT_SCALEY *aspect }, back.scale);
		if (i == 0)
		{
			glm_vec3_copy(MENU_BAR_COLOR, back.txt_clr);
			glm_vec3_copy(MENU_BAR_COLOR, back.nrml_clr);
			glm_vec3_copy(MENU_BAR_COLOR, back.hvrd_clr);
			glm_vec3_copy(MENU_BAR_COLOR, back.slctd_clr);

		}
		else
		{
			glm_vec3_copy(MENU_HOVER_COLOR, back.txt_clr);
			glm_vec3_copy(MENU_HOVER_COLOR, back.nrml_clr);
			glm_vec3_copy(MENU_HOVER_COLOR, back.hvrd_clr);
			glm_vec3_copy(MENU_HOVER_COLOR, back.slctd_clr);

		}

		bool top = ui_draw_button(back, rsrs->window);
		if (top == true && i == 0)
		{
			*move = !*move;
		}

	}
	glm_vec3_copy(MENU_TEXT_COLOR, back.txt_clr);
	glm_vec3_copy(MENU_BAR_COLOR, back.nrml_clr);
	glm_vec3_copy(MENU_BAR_COLOR, back.hvrd_clr);
	glm_vec3_copy(MENU_BAR_COLOR, back.slctd_clr);

	glm_vec2_copy((vec2) { pos[0], pos[1] - 0.01 }, back.ps);
	glm_vec2_copy((vec2) { MENU_TEXT_SCALEX, MENU_TEXT_SCALEY *aspect * 0.9 }, back.scale);
	strcpy_s(back.text, sizeof(char) * KSAI_SMALL_STRING_LENGTH, title);
	ui_draw_button(back, rsrs->window);

	return 0;
}

int draw_label_window(char text[KSAI_SMALL_STRING_LENGTH], vec2 pos, vk_rsrs *rsrs, float aspect, float row)
{
	ui_label lbl = (ui_label){
		.typ = BUTTON,
		.scale = {MENU_TEXT_SCALEX * 0.8, MENU_TEXT_SCALEY * 0.8 * 0.8 * aspect},
		.st_typ = lbl_st_UNSELECTED,
	};
	glm_vec2_copy((vec2) { pos[0], pos[1] + row * 0.08 }, lbl.ps);
	glm_vec3_copy(MENU_HOVER_COLOR, lbl.hvrd_clr);
	glm_vec3_copy(MENU_HOVER_COLOR, lbl.nrml_clr);
	glm_vec3_copy(MENU_HOVER_COLOR, lbl.slctd_clr);
	glm_vec3_copy(DEBUG_TEXT_COLOR, lbl.txt_clr);
	strcpy_s(lbl.text, sizeof(char) * KSAI_SMALL_STRING_LENGTH, text);
	return ui_draw_button(lbl, rsrs->window);
}

int draw_button_window(char text[KSAI_SMALL_STRING_LENGTH], vec2 pos, vk_rsrs *rsrs, float aspect, float row)
{
	ui_label lbl = (ui_label){
		.typ = BUTTON,
		.scale = {MENU_TEXT_SCALEX * 0.8, MENU_TEXT_SCALEY * 0.8 * 0.8 * aspect},
		.st_typ = lbl_st_UNSELECTED,
	};
	glm_vec2_copy((vec2) { pos[0], pos[1] + row * 0.08 }, lbl.ps);
	glm_vec3_copy(BUTTON_HOVER_COLOR, lbl.hvrd_clr);
	glm_vec3_copy(MENU_BAR_COLOR, lbl.nrml_clr);
	glm_vec3_copy(MENU_SELECTED_COLOR, lbl.slctd_clr);
	glm_vec3_copy(MENU_TEXT_COLOR, lbl.txt_clr);
	strcpy_s(lbl.text, sizeof(char) * KSAI_SMALL_STRING_LENGTH, text);
	return ui_draw_button(lbl, rsrs->window);
}

int draw_selector_window(char select[MAX_SELECTOR_SIZE][KSAI_SMALL_STRING_LENGTH], int count, float aspect, vec2 pos, vk_rsrs *rsrs, float row, int *selection)
{
	int clicked = 0;
	int strlengh = strlen(select[0]);
	int largest = 0;
	for (int i = 0; i < count; i++)
	{
		if (strlengh < strlen(select[i]))
		{
			strlengh = strlen(select[i]);
			largest = i;
		}
	}

	ui_label lbl = (ui_label){
		.typ = BUTTON,
		.scale = {MENU_TEXT_SCALEX * 1.4, BACK_SCALEY * MENU_TEXT_SCALEY * 0.7 * 0.8 * aspect},
		.st_typ = lbl_st_UNSELECTED,
	};
	glm_vec2_copy((vec2) { pos[0], pos[1] + row * 0.08 }, lbl.ps);
	glm_vec3_copy(SELECTOR_COLOR, lbl.hvrd_clr);
	glm_vec3_copy(SELECTOR_COLOR, lbl.nrml_clr);
	glm_vec3_copy(SELECTOR_COLOR, lbl.slctd_clr);
	glm_vec3_copy(SELECTOR_COLOR, lbl.txt_clr);
	strcpy_s(lbl.text, sizeof(char) * KSAI_SMALL_STRING_LENGTH, select[largest]);
	ui_draw_button(lbl, rsrs->window);

	glm_vec2_copy((vec2) { MENU_TEXT_SCALEX * 0.7, MENU_TEXT_SCALEY }, lbl.scale);
	glm_vec3_copy(MENU_TEXT_COLOR, lbl.txt_clr);
	strcpy_s(lbl.text, sizeof(char) * KSAI_SMALL_STRING_LENGTH, select[*selection]);
	ui_draw_button(lbl, rsrs->window);

	glm_vec2_copy((vec2) { MENU_TEXT_SCALEX * 0.7, MENU_TEXT_SCALEY * 1.2 }, lbl.scale);
	glm_vec3_copy(MENU_HOVER_COLOR, lbl.hvrd_clr);
	glm_vec3_copy(SELECTOR_COLOR, lbl.nrml_clr);
	glm_vec3_copy(MENU_SELECTED_COLOR, lbl.slctd_clr);
	glm_vec3_copy(MENU_TEXT_COLOR, lbl.txt_clr);

	glm_vec2_copy((vec2) { pos[0] + strlen(select[largest]) * 0.01, pos[1] + row * 0.08 }, lbl.ps);
	strcpy_s(lbl.text, sizeof(char) * KSAI_SMALL_STRING_LENGTH, "->");
	if (ui_draw_button(lbl, rsrs->window) && (*selection) < count - 1)
	{
		(*selection)++;
		clicked = 1;
	}

	glm_vec2_copy((vec2) { pos[0] - strlen(select[largest]) * 0.01, pos[1] + row * 0.08 }, lbl.ps);
	strcpy_s(lbl.text, sizeof(char) * KSAI_SMALL_STRING_LENGTH, "<-");
	if (ui_draw_button(lbl, rsrs->window) && (*selection) > 0)
	{
		(*selection)--;
		clicked = 1;
	}
	return clicked;
}

int draw_selector_integer(int smallest, int highest, float aspect, vec2 pos, vk_rsrs *rsrs, float row, int *selection)
{
	char select[KSAI_SMALL_STRING_LENGTH][KSAI_SMALL_STRING_LENGTH];
	int j = 0;
	for (int i = smallest; i <= highest; i++)
	{
		sprintf_s(select[j], sizeof(char) * KSAI_SMALL_STRING_LENGTH, "select(%d)", i);
		j++;
	}
	int selection_for_selector = *selection - smallest;

	draw_selector_window(
		select,
		highest - smallest,
		aspect,
		pos,
		rsrs,
		row,
		&selection_for_selector
	);
	{
		*selection = selection_for_selector + smallest;
	}
}

int draw_selector_var(int *selected, float aspect, vec2 pos, vk_rsrs *rsrs, float row, int args, ...)
{
	va_list arg;
	va_start(arg, args);
	char select[MAX_SELECTOR_SIZE][KSAI_SMALL_STRING_LENGTH];

	for (int i = 0; i < args; i++)
	{
		strcpy_s(select[i], sizeof(char) * KSAI_SMALL_STRING_LENGTH, va_arg(arg, char*));
	}

	draw_selector_window(select, args, aspect, pos, rsrs, row, selected);

	va_end(arg);
}

static uint32_t time = 0.0; 
int draw_input_number(float aspect, vec2 pos, vk_rsrs *rsrs, float row, char out[KSAI_SMALL_STRING_LENGTH], bool *should_input, int *place_value)
{
	ui_label lbl = (ui_label){
		.typ = BUTTON,
		.scale = {MENU_TEXT_SCALEX * 1.4, BACK_SCALEY * MENU_TEXT_SCALEY * 0.7 * 0.8 * aspect},
		.st_typ = lbl_st_UNSELECTED,
	};
	glm_vec2_copy((vec2) { pos[0], pos[1] + row * 0.08 }, lbl.ps);

	int xx = sdl_get_button_keyboard_number();
	if (*should_input == false)
	{
		glm_vec3_copy(SELECTOR_COLOR, lbl.nrml_clr);
		glm_vec3_copy(SELECTOR_COLOR, lbl.txt_clr);
		glm_vec3_copy(SELECTOR_COLOR, lbl.hvrd_clr);
		glm_vec3_copy(SELECTOR_COLOR, lbl.slctd_clr);
	}
	else
	{
		glm_vec3_copy(MENU_BAR_COLOR, lbl.nrml_clr);
		glm_vec3_copy(MENU_BAR_COLOR, lbl.txt_clr);
		glm_vec3_copy(MENU_BAR_COLOR, lbl.hvrd_clr);
		glm_vec3_copy(MENU_BAR_COLOR, lbl.slctd_clr);
		if (time < SDL_GetTicks() - 300)
		{
			if (xx == KSAI_INT32_MAX) {
				
			} else if (xx == 10)
			{
				out[*place_value] = '\0';
				(*place_value)--;
			}
			else if (xx == 11)
			{
				out[*place_value] = '.';
				(*place_value)++;
			}
			else if (xx == 0)
			{
				out[*place_value] = '0';
				out[*place_value + 1] = '\0';
				(*place_value)++;
			}
			else
			{
				out[*place_value] = xx + '0';
				out[*place_value + 1] = '\0';
				(*place_value)++;
			}
			time = SDL_GetTicks();
		}
	}


	strcpy_s(lbl.text, sizeof(char) * KSAI_SMALL_STRING_LENGTH, "MMMMMMMM");
	if (ui_draw_button(lbl, rsrs->window))
		*should_input = !*should_input;

	glm_vec2_copy((vec2) { MENU_TEXT_SCALEX * 0.7, MENU_TEXT_SCALEY }, lbl.scale);
	char log[KSAI_SMALL_STRING_LENGTH];
	sprintf_s(log, sizeof(char) * KSAI_SMALL_STRING_LENGTH, "%s", out);
	strcpy_s(lbl.text, sizeof(char) * KSAI_SMALL_STRING_LENGTH, log);

	glm_vec2_copy((vec2) { MENU_TEXT_SCALEX * 0.7, MENU_TEXT_SCALEY * 1.2 }, lbl.scale);
	glm_vec3_copy(SELECTOR_COLOR, lbl.hvrd_clr);
	glm_vec3_copy(SELECTOR_COLOR, lbl.nrml_clr);
	glm_vec3_copy(SELECTOR_COLOR, lbl.slctd_clr);
	glm_vec3_copy(MENU_TEXT_COLOR, lbl.txt_clr);
	ui_draw_button(lbl, rsrs->window);
	int Length;
	const Uint8 *KeyboardState = SDL_GetKeyboardState(&Length);
	if (KeyboardState[SDL_SCANCODE_RETURN])
	{
		*should_input = false;
		*place_value = 0;
		return 0;
	}
	return 0;
}

int draw_popup_menu(ui_label lbl, char ch[NO_OF_POPUP_MENUS][KSAI_SMALL_STRING_LENGTH], int count, float aspect, vec2 pos, vk_rsrs *rsrs)
{
	int x = -1;
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
		strcpy_s(back.text, KSAI_SMALL_STRING_LENGTH * sizeof(char), ch[largest]);


		for (int i = 0; i < count; i++)
		{
			glm_vec2_copy((vec2) { pos[0], pos[1] + (i + 1) * 0.08 }, back.ps);
			glm_vec2_copy((vec2) { MENU_TEXT_SCALEX, BACK_SCALEY *MENU_TEXT_SCALEY *aspect }, back.scale);
			ui_draw_button(back, rsrs->window);
		}


		for (int i = 0; i < count; i++)
		{
			strcpy_s(lbl.text, sizeof(lbl.text) * sizeof(char), ch[i]);
			glm_vec2_copy((vec2) { pos[0], pos[1] + (i + 1) * 0.08 }, lbl.ps);
			glm_vec2_copy((vec2) { MENU_TEXT_SCALEX, MENU_TEXT_SCALEY *aspect }, lbl.scale);
			if (ui_draw_button(lbl, rsrs->window) == true)
			{
				x = i;
			}
		}
	}

	return x;
}

ivec2s draw_file_menu(char ch[NO_OF_TOP_MENUS][NO_OF_POPUP_MENUS][KSAI_SMALL_STRING_LENGTH], int tmenu_count, int popmenu_count[NO_OF_TOP_MENUS], float aspect, vk_rsrs *rsrs, int *running)
{
	/* Decoration */
	static vec3 menu_bar_color;
	static vec3 menu_hover_color;
	static vec3 menu_text_color;
	static vec3 menu_selected_color;
	static int first_time = true;
	if (first_time == true)
	{
		glm_vec3_copy(MENU_BAR_COLOR, menu_bar_color);
		glm_vec3_copy(MENU_HOVER_COLOR, menu_hover_color);
		glm_vec3_copy(MENU_TEXT_COLOR, menu_text_color);
		glm_vec3_copy(MENU_SELECTED_COLOR, menu_selected_color);
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
		prev_length += (float) strlen(ch[i][0]) / KSAI_SMALL_STRING_LENGTH * 3;
	}

	glm_vec3_copy(menu_text_color, lbl.txt_clr);
	glm_vec3_copy(menu_hover_color, lbl.hvrd_clr);
	glm_vec3_copy(menu_selected_color, lbl.slctd_clr);
	glm_vec3_copy(menu_bar_color, lbl.nrml_clr);


	ivec2s ret;
	if (selected_menu != -1)
	{
		//ui_draw_button(lbl, rsrs->window);
		int choice = draw_popup_menu(lbl, ch[selected_menu] + 1, popmenu_count[selected_menu], aspect, (vec2) { -0.95 + prev_length_next, -0.97 }, rsrs);
		ret = (ivec2s){ .x = selected_menu, .y = choice };
		if (choice != -1)
		{
			selected_menu = -1;
		}
	}
	else
	{
		ret = (ivec2s){ .x = 0, .y = 0 };
	}

	glm_vec3_copy(MENU_TEXT_COLOR, lbl.txt_clr);
	glm_vec3_copy(MENU_HOVER_COLOR, lbl.hvrd_clr);
	glm_vec3_copy(MENU_CLOSE_COLOR, lbl.slctd_clr);
	glm_vec3_copy(MENU_CLOSE_COLOR, lbl.nrml_clr);
	glm_vec2_copy((vec2) { -TOP_MENU_LEFT_PADDING, TOP_MENU_POSY }, lbl.ps);
	glm_vec2_copy((vec2) { MENU_TEXT_SCALEX, MENU_TEXT_SCALEY *scale_factor *aspect * 1.45 }, lbl.scale);
	strcpy_s(lbl.text, sizeof(char) * KSAI_SMALL_STRING_LENGTH, "close");
	if (ui_draw_button(lbl, rsrs->window))
	{
		*running = 0;
	}


	return ret;

}
