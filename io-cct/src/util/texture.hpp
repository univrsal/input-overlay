/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <universailp@web.de>.
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

#pragma once

#include <SDL.h>

class texture {
public:
    texture(const char* path, SDL_Renderer* renderer);

    texture(const char* path, SDL_Renderer* renderer, uint8_t* scale);

    texture();

    ~texture();

    bool load(const char* path, SDL_Renderer* renderer);

    void free();

    SDL_Rect get_dim() const;

    void draw(SDL_Renderer* renderer) const;

    void draw_tiling(SDL_Renderer* renderer, const SDL_Rect* target, int scale_f) const;

    void draw_tiling_scaled(SDL_Renderer* renderer, const SDL_Rect* target) const;

    void draw(SDL_Renderer* renderer, int x, int y) const;

    void draw(SDL_Renderer* renderer, const SDL_Point* p) const;

    void draw(SDL_Renderer* renderer, const SDL_Point* p, int scaled_offset_x, int scaled_offset_y) const;

    void draw(SDL_Renderer* renderer, const SDL_Rect* target_dim, const SDL_Rect* cutout) const;

    void draw(SDL_Renderer* renderer, int x, int y, uint8_t alpha) const;

    void draw(SDL_Renderer* renderer, const SDL_Rect* target_dim, const SDL_Rect* cutout, uint8_t alpha) const;

private:
    SDL_Texture* m_sdl_texture;
    SDL_Rect m_dimensions = {};
    uint8_t* m_scale = nullptr;
};
