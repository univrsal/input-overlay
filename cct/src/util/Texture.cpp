/**
 * Created by univrsal on 14.07.2017.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "Texture.hpp"

Texture::Texture()
{
	m_sdl_texture = NULL;
}


Texture::Texture(const char *path, SDL_Renderer *renderer)
{
	m_sdl_texture = NULL;
	m_scale = NULL;
	load(path, renderer);
}

Texture::Texture(const char *path, SDL_Renderer *renderer, uint8_t *scale)
{
	m_sdl_texture = NULL;
	load(path, renderer);
	m_scale = scale;
}

Texture::~Texture()
{
	free();
	m_dimensions = { 0, 0, 0, 0 };
	m_scale = NULL;
}

bool Texture::load(const char *path, SDL_Renderer *renderer)
{
	free();
	if (renderer == NULL)
		return false;

	SDL_Surface *surface = IMG_Load(path);

	if (surface == NULL) {
		printf("ERROR: Couldn't load image %s! SDL_Error: %s\n", path, SDL_GetError());
		return false;
	}

	m_sdl_texture = SDL_CreateTextureFromSurface(renderer, surface);

	if (m_sdl_texture == NULL)
	{
		printf("ERROR: Couldn't load image %s! SDL_Error: %s\n", path, SDL_GetError());
		return false;
	}

	m_dimensions.w = surface->w;
	m_dimensions.h = surface->h;
	m_dimensions.x = 0;
	m_dimensions.y = 0;

	SDL_FreeSurface(surface);
	return true;
}

void Texture::free()
{
	if (m_sdl_texture != NULL)
		SDL_DestroyTexture(m_sdl_texture);
	m_sdl_texture = NULL;
}

SDL_Rect Texture::get_dim(void)
{
	return m_dimensions;
}

void Texture::draw(SDL_Renderer *renderer)
{
	SDL_RenderCopy(renderer, m_sdl_texture, NULL, NULL);
}

void Texture::draw_tiling(SDL_Renderer *renderer, const SDL_Rect *target, int scale_f)
{
	int x_tiles, y_tiles;
	SDL_Rect dim_temp = { 0, 0, m_dimensions.w * scale_f, m_dimensions.h * scale_f };

	if (target->w % m_dimensions.w > 0)
		x_tiles = (target->w / dim_temp.w) + 1;
	else
		x_tiles = target->w / dim_temp.w;

	if (target->h % dim_temp.h > 0)
		y_tiles = (target->h / dim_temp.h) + 1;
	else
		y_tiles = target->h / dim_temp.h;

	for (int x = 0; x < x_tiles; x++) {
		for (int y = 0; y < y_tiles; y++) {
			draw(renderer, target->x + x * dim_temp.w, target->y + y * dim_temp.h);
		}
	}
}

void Texture::draw_tiling_scaled(SDL_Renderer *renderer, const SDL_Rect *target)
{
	draw_tiling(renderer, target, *m_scale);
}

void Texture::draw(SDL_Renderer *renderer, int x, int y)
{
	SDL_Rect temp_rect = { x, y, m_dimensions.w, m_dimensions.h };

	if (m_scale != NULL) {
		temp_rect.w *= (int)(*m_scale);
		temp_rect.h *= (int)(*m_scale);
	}

	SDL_RenderCopy(renderer, m_sdl_texture, NULL, &temp_rect);
}

void Texture::draw(SDL_Renderer *renderer, const SDL_Point *p)
{
	SDL_Rect temp_rect = { p->x, p->y, m_dimensions.w, m_dimensions.h };

	if (m_scale != NULL) {
		temp_rect.w *= (int)(*m_scale);
		temp_rect.h *= (int)(*m_scale);
	}

	SDL_RenderCopy(renderer, m_sdl_texture, NULL, &temp_rect);
}

void Texture::draw(SDL_Renderer *renderer, const SDL_Point *p, int scaled_offset_x, int scaled_offset_y)
{
	SDL_Rect temp_rect = { p->x, p->y, m_dimensions.w, m_dimensions.h };

	if (m_scale != NULL) {
		temp_rect.w *= (int)(*m_scale);
		temp_rect.h *= (int)(*m_scale);
		temp_rect.x += scaled_offset_x * (int)(*m_scale);
		temp_rect.y += scaled_offset_y * (int)(*m_scale);
	}

	SDL_RenderCopy(renderer, m_sdl_texture, NULL, &temp_rect);
}

void Texture::draw(SDL_Renderer *renderer, const SDL_Rect *target_dim, const SDL_Rect *cutout)
{
	SDL_RenderCopy(renderer, m_sdl_texture, cutout, target_dim);
}
