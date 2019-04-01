/**
 * Created by univrsal on 14.07.2017.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "Texture.hpp"

Texture::Texture()
{
    m_sdl_texture = nullptr;
}

Texture::Texture(const char* path, SDL_Renderer* renderer)
{
    m_sdl_texture = nullptr;
    m_scale = nullptr;
    load(path, renderer);
}

Texture::Texture(const char* path, SDL_Renderer* renderer, uint8_t* scale)
{
    m_sdl_texture = nullptr;
    load(path, renderer);
    m_scale = scale;
}

Texture::~Texture()
{
    free();
    m_dimensions = {0, 0, 0, 0};
    m_scale = nullptr;
}

bool Texture::load(const char* path, SDL_Renderer* renderer)
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

void Texture::free()
{
    if (m_sdl_texture != nullptr)
        SDL_DestroyTexture(m_sdl_texture);
    m_sdl_texture = nullptr;
}

SDL_Rect Texture::get_dim() const
{
    return m_dimensions;
}

void Texture::draw(SDL_Renderer* renderer) const
{
    SDL_RenderCopy(renderer, m_sdl_texture, nullptr, nullptr);
}

void Texture::draw_tiling(SDL_Renderer* renderer, const SDL_Rect* target, const int scale_f) const
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

void Texture::draw_tiling_scaled(SDL_Renderer* renderer, const SDL_Rect* target) const
{
    draw_tiling(renderer, target, *m_scale);
}

void Texture::draw(SDL_Renderer* renderer, const int x, const int y) const
{
    SDL_Rect temp_rect = {x, y, m_dimensions.w, m_dimensions.h};

    if (m_scale != nullptr) {
        temp_rect.w *= static_cast<int>(*m_scale);
        temp_rect.h *= static_cast<int>(*m_scale);
    }

    SDL_RenderCopy(renderer, m_sdl_texture, nullptr, &temp_rect);
}

void Texture::draw(SDL_Renderer* renderer, const SDL_Point* p) const
{
    SDL_Rect temp_rect = {p->x, p->y, m_dimensions.w, m_dimensions.h};

    if (m_scale != nullptr) {
        temp_rect.w *= static_cast<int>(*m_scale);
        temp_rect.h *= static_cast<int>(*m_scale);
    }

    SDL_RenderCopy(renderer, m_sdl_texture, nullptr, &temp_rect);
}

void
Texture::draw(SDL_Renderer* renderer, const SDL_Point* p, const int scaled_offset_x, const int scaled_offset_y) const
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

void Texture::draw(SDL_Renderer* renderer, const SDL_Rect* target_dim, const SDL_Rect* cutout) const
{
    SDL_RenderCopy(renderer, m_sdl_texture, cutout, target_dim);
}

void Texture::draw(SDL_Renderer* renderer, const int x, const int y, const uint8_t alpha) const
{
    SDL_SetTextureAlphaMod(m_sdl_texture, alpha);
    draw(renderer, x, y);
    SDL_SetTextureAlphaMod(m_sdl_texture, 255);
}

void
Texture::draw(SDL_Renderer* renderer, const SDL_Rect* target_dim, const SDL_Rect* cutout, const uint8_t alpha) const
{
    if (alpha < 255) {
        SDL_SetTextureAlphaMod(m_sdl_texture, alpha);
        draw(renderer, target_dim, cutout);
        SDL_SetTextureAlphaMod(m_sdl_texture, 255);
    } else {
        draw(renderer, target_dim, cutout);
    }
}
