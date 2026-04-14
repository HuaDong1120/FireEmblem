#include "renderer.hpp"
#include "image.hpp"
#include "math.hpp"
#include "window.hpp"
#include <SDL3/SDL_blendmode.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include "sdl_call.hpp"

Renderer::Renderer(Window& window) {
    SDL_CALL_PTR(m_renderer, SDL_CreateRenderer(window.GetWindow(), nullptr));
    SDL_CALL(SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND));
}

Renderer::~Renderer() {
    SDL_DestroyRenderer(m_renderer);
}

void Renderer::Present() {
    SDL_CALL(SDL_RenderPresent(m_renderer));
}

void Renderer::Clear() {
    SDL_SetRenderDrawColor(m_renderer,
        m_clear_color.r, m_clear_color.g,
        m_clear_color.b, m_clear_color.a);
    SDL_RenderClear(m_renderer);
}

void Renderer::SetClearColor(const Color& c) {
    m_clear_color.r = static_cast<Uint8>(c.r * 255);
    m_clear_color.g = static_cast<Uint8>(c.g * 255);
    m_clear_color.b = static_cast<Uint8>(c.b * 255);
    m_clear_color.a = static_cast<Uint8>(c.a * 255);
}

void Renderer::DrawLine(const Vec2& p1, const Vec2& p2, const Color& color) {
    setRenderColor(color);
    SDL_CALL(SDL_RenderLine(m_renderer, p1.x, p1.y, p2.x, p2.y));
}

void Renderer::DrawFillRect(const Region& rect, const Color& color) {
    setRenderColor(color);
    SDL_FRect sdl_rect;
    sdl_rect.x = rect.m_topleft.x;
    sdl_rect.y = rect.m_topleft.y;
    sdl_rect.w = rect.m_size.w;
    sdl_rect.h = rect.m_size.h;
    SDL_CALL(SDL_RenderFillRect(m_renderer, &sdl_rect));
}

void Renderer::DrawImgage(const Image& image, const Region& src,
                           const Region& dst, Degrees rotation,
                           const Vec2& center, Flags<Flip> flip) {
    SDL_FRect src_rect, dst_rect;
    src_rect.x = src.m_topleft.x;
    src_rect.y = src.m_topleft.y;
    src_rect.w = src.m_size.w;
    src_rect.h = src.m_size.h;

    dst_rect.x = dst.m_topleft.x;
    dst_rect.y = dst.m_topleft.y;
    dst_rect.w = dst.m_size.w;
    dst_rect.h = dst.m_size.h;

    SDL_FPoint sdl_center;
    sdl_center.x = center.x;
    sdl_center.y = center.y;

    SDL_CALL(SDL_RenderTextureRotated(m_renderer, image.GetTexture(),
        &src_rect, &dst_rect, rotation.Value(),
        &sdl_center,
        static_cast<SDL_FlipMode>(static_cast<unsigned int>(flip))));
}

void Renderer::setRenderColor(const Color& color) {
    SDL_CALL(SDL_SetRenderDrawColor(m_renderer,
        static_cast<Uint8>(color.r * 255),
        static_cast<Uint8>(color.g * 255),
        static_cast<Uint8>(color.b * 255),
        static_cast<Uint8>(color.a * 255)));
}

SDL_Renderer* Renderer::GetRenderer() const {
    return m_renderer;
}
