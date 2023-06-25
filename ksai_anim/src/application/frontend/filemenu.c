#include "app_ui.h"
#include <backend/ui/ui.h>
#include <backend/ui/latex_colors.h>


int draw_file_menu(char ch[NO_OF_TOP_MENUS][NO_OF_POPUP_MENUS], int tmenu_count, int popmenu_count[NO_OF_TOP_MENUS], float aspect, vk_rsrs *rsrs, SDL_Event *event)
{
	lu_lbl lbl = (lu_lbl){
		.hvrd_clr = {color_RED[0], color_RED[1], color_RED[2]},
		.nrml_clr = {color_ALIZARIN[0], color_BLACK[1], color_BLACK[2]},
		.slctd_clr = {color_GREEN_MUNSELL_[0], color_GREEN_MUNSELL_[1], color_GREEN_MUNSELL_[2]},
		.txt_clr = {color_ALMOND[0], color_ALMOND[1], color_ALMOND[2]},
		.typ = BUTTON,
		.ps = {0, 0},
		.scale = {0.2, 0.1 * aspect},
		.st_typ = lbl_st_UNSELECTED,
		.text = "FILE"
	};
	lu_drw_lbl(lbl, rsrs->window, event);
	return 0;
}
