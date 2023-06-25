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

typedef struct lu_lbl_bs
{
	l_vlkn_ppln		ppln;
	fnt_d			fnt;
} lu_lbl_bs;

typedef struct lu_lbl
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
} lu_lbl;


void lu_null_lbls_cnt();

void lu_int(int size_factor, vk_rsrs *_rsrs);
bool lu_drw_lbl(lu_lbl lbl, SDL_Window *_wndw, SDL_Event *event);
void lu_updt(int *_current_frame);
void lu_rndr(int *_crrnt_frm, vk_rsrs *_rsrs);

extern l_vlkn_ppln *pplns_[MAX_BUFFER_SIZE];
extern uint32_t vlkn_ppln_cnt;
