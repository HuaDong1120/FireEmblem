#pragma  once
#include "SDL3/SDL.h"
#include "math.hpp"
#include "flag.hpp"
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
class Image;
class Window;

enum Flip {
    FlipNone       = SDL_FLIP_NONE,
    FlipHorizontal = SDL_FLIP_HORIZONTAL,
    FlipVertical   = SDL_FLIP_VERTICAL,
};

class Renderer {
public:
    Renderer(Window& window);
    ~Renderer();
    Renderer(const Renderer*) = delete;
    Renderer& operator=(const Renderer*) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    void SetClearColor(const Color& c);
    void Present();
    void Clear();

    void DrawLine(const Vec2& p1, const Vec2& p2, const Color& color);
    void DrawFillRect(const Region& rect, const Color& color);  
    void DrawImgage(const Image&, const Region& src,
                    const Region& dst, Degrees rotation,
                    const Vec2& center, Flags<Flip>);

    SDL_Renderer* GetRenderer() const;

private:
    SDL_Renderer* m_renderer{};
    SDL_Color     m_clear_color;

    void setRenderColor(const Color& color);
};
