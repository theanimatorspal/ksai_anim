#pragma once
#include <stdint.h>
#include <vendor/cglm/cglm.h>
#include <vulkan/vulkan.h>
#include <backend/vulkan/pipelines.h>

typedef
struct fnt_cntnr
{
	vec3		ps;
	vec3		scl;
	vec3		rt;
} fnt_cntnr;

typedef
struct bmchar {
	uint32_t		id;
	uint32_t		x, y;
	uint32_t		width;
	uint32_t		height;
	int32_t			xoffset;
	int32_t			yoffset;
	int32_t			xadvance;
	uint32_t		page;
} bmchar;

typedef 
struct fnt_f 
{
	uint32_t		c_lineheight;
	uint32_t		base;
	uint32_t		scale_w;
	uint32_t		scale_h;
	uint32_t		pages;
	uint32_t		packed;
	uint32_t		page_id;
	char			file[1 << 7];
} fnt_f;

typedef
struct fnt_d
{
	VkImage				fnt_img;
	VkDeviceMemory			fnt_img_mmry;
	VkImageView				fnt_img_vw;
	VkSampler				fnt_img_smplr;
	char					fnt_img_pth[1 << 8];
	l_vlkn_ppln				ppln;
	bmchar					fchars[1 << 8];
	fnt_f					features;
	fnt_cntnr				container;
	float					max_h;
	float					max_w;
} fnt_d;


void prs_bm_fnt(const char* path, fnt_d *font_desp);
void gn_txt(const char* text, const int size, fnt_d *font_desp);
