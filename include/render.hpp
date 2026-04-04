#pragma  once
#include "SDL3/SDL.h"
#include "math.hpp"
#include <SDL3/SDL_render.h>
class Window;
class Renderer{
public:
    Renderer(Window&window);
    ~Renderer();
    Renderer(const Renderer*) = delete;
    Renderer& operator=(const Renderer*) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    void SetClearColor(const Color&c);

    void Present();

    void Clear();
private:
    SDL_Renderer * m_renderer{};
    SDL_Color m_clear_color;
};
