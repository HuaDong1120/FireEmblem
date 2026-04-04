#include "window.hpp"
#include "log.hpp"
#include "math.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_video.h>

Window::Window(const std::string&title,int w,int h)
{
    m_window =  SDL_CreateWindow(title.c_str(), w, h, 0);
    if(!m_window)
    {
        LOG_ERROR("create SDL window failed!{}",SDL_GetError());
    }

}

Window::~Window()
{
    SDL_DestroyWindow(m_window);
}
SDL_Window* Window::GetWindow()
{
    return m_window;
}
Vec2 Window::GetWindowSize()const
{
    int w,h;
    SDL_GetWindowSize(m_window, &w, &h);
    return Vec2(w,h);
}
