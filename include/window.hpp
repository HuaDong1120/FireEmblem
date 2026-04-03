#pragma  once

#include <string>
#include "SDL3/SDL.h"
class Window{
public:
    Window(const std::string&title,int w,int h);
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

private:
    SDL_Window* m_window{};


};
