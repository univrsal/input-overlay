/**
 * Created by univrsal on 14.07.2017.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include "Constants.hpp"

class Texture
{
public:
	Texture(const char *path, SDL_Renderer *renderer);

	Texture(const char *path, SDL_Renderer *renderer, uint8_t *scale);

	Texture();

	~Texture();

	bool load(const char *path, SDL_Renderer *renderer);

	void free();

	SDL_Rect get_dim(void);

	void draw(SDL_Renderer *renderer);

	void draw_tiling(SDL_Renderer *renderer, const SDL_Rect *target, int scale_f);

	void draw_tiling_scaled(SDL_Renderer *renderer, const SDL_Rect *target);

	void draw(SDL_Renderer *renderer, int x, int y);

	void draw(SDL_Renderer *renderer, const SDL_Point *p);

	void draw(SDL_Renderer *renderer, const SDL_Point *p, int scaled_offset_x, int scaled_offset_y);

	void draw(SDL_Renderer *renderer, const SDL_Rect *target_dim, const SDL_Rect *cutout);

private:
	SDL_Texture * m_sdl_texture;
	SDL_Rect m_dimensions;
	uint8_t *m_scale;
};
