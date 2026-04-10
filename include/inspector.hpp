#pragma once
#include <SDL3/SDL_events.h>

class Renderer;
class Window;
class Inspector
{
public:
    Inspector(Window& window, Renderer& renderer);
    ~Inspector();
    void Update();
    void BeginFrame()const;
    void EndFrame()const;
   
    void HandleEvent(const SDL_Event& event);
private:    
    Window& m_window;
    Renderer& m_renderer; 
};