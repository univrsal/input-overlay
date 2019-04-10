/**
 * Created by univrsal on 14.07.2017.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include "constants.hpp"

class texture
{
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
