#include "render.hpp"
#include "window.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_render.h>
#include "log.hpp"
Renderer::Renderer(Window&window)
{
    m_renderer = SDL_CreateRenderer(window.GetWindow(),nullptr);
    if(!m_renderer)
    {
        LOG_ERROR("Create SDL Renderer failed:{}",SDL_GetError());
    }
}

Renderer::~Renderer()
{
    SDL_DestroyRenderer(m_renderer);

}

void Renderer::Present()
{
    bool isPresent = SDL_RenderPresent(m_renderer);
    if(!isPresent)
    {
        LOG_ERROR("failed to present:{}",SDL_GetError());
    }
}
void Renderer::Clear()
{
    SDL_SetRenderDrawColor(m_renderer,m_clear_color.r,m_clear_color.g,m_clear_color.b,m_clear_color.a);
    SDL_RenderClear(m_renderer);
}
void Renderer::SetClearColor(const Color&c)
{
    m_clear_color.r = c.r*255;
    m_clear_color.g = c.g*255;
    m_clear_color.b = c.b*255;
    m_clear_color.a = c.a*255;
}
