#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <ksai/ksai_memory.h>
#include "font.h"

static ksai_Arena global_arena;
int get_n_pair(int n, char* string_in)
{
	int equal_to_sign_index = 0;
	int count_eq = 0;
	for (int i = 0; string_in[i] != '\n' && string_in[i] != '\0'; i++)
	{
		if (string_in[i] == '=')
		{
			count_eq++;
			if (count_eq == n)
			{
				int j = i + 1;
				char dummy[1 << 5];
				int x;
				for (x = 0; string_in[j] != ' ' && string_in[j] != '\t'; x++)
				{
					dummy[x] = string_in[j];
					j++;
				}
				dummy[x] = '\0';
				int out;
				sscanf(dummy, "%d", &out);
				return out;
			}
		}
	}
}

void prs_bm_fnt(const char* path, fnt_d *font_desp)
{
	static char file_buffer[1 << 25];

	FILE* file = fopen(path, "r");
	fgets(file_buffer, 1 << 25, file);
	fgets(file_buffer, 1 << 25, file);
	/* fnt features extract */
	{
		font_desp->features.c_lineheight = get_n_pair(1, file_buffer);
		font_desp->features.base = get_n_pair(2, file_buffer);
		font_desp->features.scale_w = get_n_pair(3, file_buffer);
		font_desp->features.scale_h = get_n_pair(4, file_buffer);
		font_desp->features.pages = get_n_pair(5, file_buffer);
		font_desp->features.packed = get_n_pair(6, file_buffer);
		fgets(file_buffer, 1 << 25, file);
		font_desp->features.page_id = get_n_pair(1, file_buffer);
	}
	fgets(file_buffer, 1 << 25, file);
	int chars_count = get_n_pair(1, file_buffer);

	for (int i = 0; i < chars_count; i++)
	{
		fgets(file_buffer, 1 << 25, file);
		font_desp->fchars[i].id = get_n_pair(1, file_buffer);
		font_desp->fchars[i].x = get_n_pair(2, file_buffer);
		font_desp->fchars[i].y = get_n_pair(3, file_buffer);
		font_desp->fchars[i].width = get_n_pair(4, file_buffer);
		font_desp->fchars[i].height = get_n_pair(5, file_buffer);
		font_desp->fchars[i].xoffset = get_n_pair(6, file_buffer);
		font_desp->fchars[i].yoffset = get_n_pair(7, file_buffer);
		font_desp->fchars[i].xadvance = get_n_pair(8, file_buffer);
		font_desp->fchars[i].page = get_n_pair(9, file_buffer);
	}
	fclose(file);
}

void gn_txt(const char* text, const int size, fnt_d* font_desp)
{
	static bool first_time = true;
	if(first_time == true)
		ksai_Arena_init(sizeof(l_vbs_vrtx) * 4 * size * KSAI_STRING_ARENA_MEMORY, &global_arena);
	first_time = false;

	if (font_desp->ppln.vertices_count < 4 * size || font_desp->ppln.indices_count < 6 * size)
	{
		font_desp->ppln.vertices = (l_vbs_vrtx*)ksai_Arena_allocate(sizeof(l_vbs_vrtx) * 4 * size, &global_arena);
		font_desp->ppln.indcs = (uint32_t*)ksai_Arena_allocate(sizeof(uint32_t) * 6 * size, &global_arena);
	}
	font_desp->ppln.vertices_count = 4 * size;
	font_desp->ppln.indices_count = 6 * size;
	font_desp->max_h = 0;
	font_desp->max_w = 0;

	mat4 orth_proj;
	glm_ortho(-1, 1, 1, -1, 0, 1, orth_proj);

	uint32_t index_offset = 0;
	float w = font_desp->features.scale_w; // texture fnt sdf width

	float posx = 0.0f;
	float posy = 0.0f;

	for (uint32_t i = 0; i < size; i++)
	{
		bmchar char_info = font_desp->fchars[(int)text[i]];

		for (int j = 0; j < 255; j++)
		{
			if (font_desp->fchars[j].id == text[i])
			{
				char_info = font_desp->fchars[j];
			}
		}


		if (char_info.width == 0)
		{
			char_info.width = 36.0f;
		}

		float charw = ((char_info.width)) / 36.0f;
		float dimx = 1.0f * charw;
		float charh = ((char_info.height)) / 36.0f;
		float dimy = 1.0f * charh;

		float us = char_info.x / w;
		float ue = (char_info.x + char_info.width) / w;
		float ts = char_info.y / w;
		float te = (char_info.y + char_info.height) / w;

		float xo = char_info.xoffset / 36.0f;
		float yo = char_info.yoffset / 36.0f;
		posy = yo;

		font_desp->ppln.vertices[i * 4 + 0] = (l_vbs_vrtx){
			.pos = {posx + dimx + xo, posy + dimy, 0.0f},
			.tex_coords = {ue, te},
			.normal = {1, 0, 0}
		};


		font_desp->ppln.vertices[i * 4 + 1] = (l_vbs_vrtx){
			.pos = {posx + xo, posy + dimy, 0.0f},
			.tex_coords = {us, te},
			.normal = {1, 0, 0}
		};

		font_desp->ppln.vertices[i * 4 + 2] = (l_vbs_vrtx){
			.pos = {posx + xo, posy, 0.0f},
			.tex_coords = {us, ts},
			.normal = {1, 0, 0}
		};

		font_desp->ppln.vertices[i * 4 + 3] = (l_vbs_vrtx){
			.pos = {posx + dimx + xo, posy, 0.0f},
			.tex_coords = {ue, ts},
			.normal = {1, 0, 0}
		};

		font_desp->ppln.indcs[i * 6 + 0] = i * 4 + 0;
		font_desp->ppln.indcs[i * 6 + 1] = i * 4 + 1;
		font_desp->ppln.indcs[i * 6 + 2] = i * 4 + 2;
		font_desp->ppln.indcs[i * 6 + 3] = i * 4 + 2;
		font_desp->ppln.indcs[i * 6 + 4] = i * 4 + 3;
		font_desp->ppln.indcs[i * 6 + 5] = i * 4 + 0;

		font_desp->max_w += charw;
		if (charh > font_desp->max_h)
		{
			font_desp->max_h = charh;
		}

		float advance = ((float)(char_info.xadvance) / 36.0f);
		posx += advance;
	}


	for (int i = 0; i < font_desp->ppln.vertices_count; i++)
	{
		font_desp->ppln.vertices[i].pos[0] -= posx / 2.0f;
		font_desp->ppln.vertices[i].pos[1] -= 0.5f;
	}
}

void fnt_free()
{
	ksai_Area_free(&global_arena);
}
