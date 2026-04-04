#pragma  once

#include <SDL3/SDL_video.h>
#include <string>
#include "SDL3/SDL.h"
#include "math.hpp"
class Window{
public:
    Window(const std::string&title,int w,int h);
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    Vec2 GetWindowSize() const;


    SDL_Window* GetWindow();

private:
    SDL_Window* m_window{};


};
