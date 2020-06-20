/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <uni@vrsal.cf>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/

#include "texture.hpp"
#include "constants.hpp"
#include <SDL_image.h>
#include <cstdio>

texture::texture()
{
	m_sdl_texture = nullptr;
}

texture::texture(const char *path, SDL_Renderer *renderer)
{
	m_sdl_texture = nullptr;
	m_scale = nullptr;
	load(path, renderer);
}

texture::texture(const char *path, SDL_Renderer *renderer, uint8_t *scale)
{
	m_sdl_texture = nullptr;
	load(path, renderer);
	m_scale = scale;
}

texture::~texture()
{
	free();
	m_dimensions = {0, 0, 0, 0};
	m_scale = nullptr;
}

bool texture::load(const char *path, SDL_Renderer *renderer)
{
	free();
	if (renderer == nullptr)
		return false;

	const auto surface = IMG_Load(path);

	if (surface == nullptr) {
		printf(SDL_IMAGE_TO_SURFACE, path, SDL_GetError());
		return false;
	}

	m_sdl_texture = SDL_CreateTextureFromSurface(renderer, surface);

	if (m_sdl_texture == nullptr) {
		printf(SDL_SURFACE_TO_TEXTURE_FAILED, path, SDL_GetError());
		return false;
	}

	m_dimensions.w = surface->w;
	m_dimensions.h = surface->h;
	m_dimensions.x = 0;
	m_dimensions.y = 0;

	SDL_FreeSurface(surface);
	return true;
}

void texture::free()
{
	if (m_sdl_texture != nullptr)
		SDL_DestroyTexture(m_sdl_texture);
	m_sdl_texture = nullptr;
}

SDL_Rect texture::get_dim() const
{
	return m_dimensions;
}

void texture::draw(SDL_Renderer *renderer) const
{
	SDL_RenderCopy(renderer, m_sdl_texture, nullptr, nullptr);
}

void texture::draw_tiling(SDL_Renderer *renderer, const SDL_Rect *target, const int scale_f) const
{
	int x_tiles, y_tiles;
	const SDL_Rect dim_temp = {0, 0, m_dimensions.w * scale_f, m_dimensions.h * scale_f};

	if (target->w % m_dimensions.w > 0)
		x_tiles = (target->w / dim_temp.w) + 1;
	else
		x_tiles = target->w / dim_temp.w;

	if (target->h % dim_temp.h > 0)
		y_tiles = (target->h / dim_temp.h) + 1;
	else
		y_tiles = target->h / dim_temp.h;

	for (auto x = 0; x < x_tiles; x++) {
		for (auto y = 0; y < y_tiles; y++) {
			draw(renderer, target->x + x * dim_temp.w, target->y + y * dim_temp.h);
		}
	}
}

void texture::draw_tiling_scaled(SDL_Renderer *renderer, const SDL_Rect *target) const
{
	draw_tiling(renderer, target, *m_scale);
}

void texture::draw(SDL_Renderer *renderer, const int x, const int y) const
{
	SDL_Rect temp_rect = {x, y, m_dimensions.w, m_dimensions.h};

	if (m_scale != nullptr) {
		temp_rect.w *= static_cast<int>(*m_scale);
		temp_rect.h *= static_cast<int>(*m_scale);
	}

	SDL_RenderCopy(renderer, m_sdl_texture, nullptr, &temp_rect);
}

void texture::draw(SDL_Renderer *renderer, const SDL_Point *p) const
{
	SDL_Rect temp_rect = {p->x, p->y, m_dimensions.w, m_dimensions.h};

	if (m_scale != nullptr) {
		temp_rect.w *= static_cast<int>(*m_scale);
		temp_rect.h *= static_cast<int>(*m_scale);
	}

	SDL_RenderCopy(renderer, m_sdl_texture, nullptr, &temp_rect);
}

void texture::draw(SDL_Renderer *renderer, const SDL_Point *p, const int scaled_offset_x,
				   const int scaled_offset_y) const
{
	SDL_Rect temp_rect = {p->x, p->y, m_dimensions.w, m_dimensions.h};

	if (m_scale != nullptr) {
		temp_rect.w *= static_cast<int>(*m_scale);
		temp_rect.h *= static_cast<int>(*m_scale);
		temp_rect.x += scaled_offset_x * static_cast<int>(*m_scale);
		temp_rect.y += scaled_offset_y * static_cast<int>(*m_scale);
	}

	SDL_RenderCopy(renderer, m_sdl_texture, nullptr, &temp_rect);
}

void texture::draw(SDL_Renderer *renderer, const SDL_Rect *target_dim, const SDL_Rect *cutout) const
{
	SDL_RenderCopy(renderer, m_sdl_texture, cutout, target_dim);
}

void texture::draw(SDL_Renderer *renderer, const int x, const int y, const uint8_t alpha) const
{
	SDL_SetTextureAlphaMod(m_sdl_texture, alpha);
	draw(renderer, x, y);
	SDL_SetTextureAlphaMod(m_sdl_texture, 255);
}

void texture::draw(SDL_Renderer *renderer, const SDL_Rect *target_dim, const SDL_Rect *cutout,
				   const uint8_t alpha) const
{
	if (alpha < 255) {
		SDL_SetTextureAlphaMod(m_sdl_texture, alpha);
		draw(renderer, target_dim, cutout);
		SDL_SetTextureAlphaMod(m_sdl_texture, 255);
	} else {
		draw(renderer, target_dim, cutout);
	}
}
