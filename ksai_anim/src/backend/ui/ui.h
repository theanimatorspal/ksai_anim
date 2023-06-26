#pragma once
#include <string.h>
#include <vendor/cglm/cglm.h>
#include <backend/vulkan/init.h>
#include <backend/vulkan/pipelines.h>
#include "font.h"

#define MAX_BUFFER_SIZE 512


typedef enum lbl_typ
{
	BUTTON,
	LABEL
} lbl_typ;

typedef enum bttn_stt_typ
{
	lbl_st_UNSELECTED,
	lbl_st_SELECTED,
	lbl_st_HOVERING,
} bttn_stt_typ;

typedef struct global_ui_base_pipeline
{
	pipeline_vk		ppln;
	fnt_d			fnt;
} global_ui_base_pipeline;

typedef struct ui_label
{
	bttn_stt_typ st_typ;
	lbl_typ typ;
	struct			l_vbs_vrtx vrtcs[4];
	uint32_t		indcs[6];
	vec3			slctd_clr;
	vec3			hvrd_clr;
	vec3			nrml_clr;
	vec3			txt_clr;
	vec2			ps;
	vec3			scale;
	char			text[100];
} ui_label;



void ui_init(int size_factor, vk_rsrs *_rsrs);
bool ui_draw_button(ui_label lbl, SDL_Window *_wndw, SDL_Event *event);
void ui_events(SDL_Window *_wndw, SDL_Event *event);
void ui_update(int *_current_frame);
void ui_render(int *_crrnt_frm, vk_rsrs *_rsrs);
void ui_destroy(vk_rsrs *_rsrs);

extern pipeline_vk *pplns_[MAX_BUFFER_SIZE];
extern uint32_t vlkn_ppln_cnt;
