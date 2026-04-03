#include "window.hpp"
#include "log.hpp"
#include <SDL3/SDL_error.h>

Window::Window(const std::string&title,int w,int h)
{
    m_window =  SDL_CreateWindow(title.c_str(), w, h, 0);
    if(!m_window)
    {
        LOG_ERROR("create SDL window failed!{}",SDL_GetError());
    }
}
