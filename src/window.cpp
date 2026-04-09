#include "window.hpp"
#include "math.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_video.h>
#include "sdl_call.hpp"

Window::Window(const std::string&title,int w,int h)
{
    SDL_CALL_PTR(m_window,SDL_CreateWindow(title.c_str(), w, h, SDL_WINDOW_RESIZABLE));
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
    SDL_CALL(SDL_GetWindowSize(m_window, &w, &h));
    return Vec2(w,h);
}
