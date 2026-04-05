#pragma once

#include "renderer.hpp"
#include "window.hpp"
#include "image.hpp"
#include <SDL3/SDL_events.h>
#include <memory>


class Context{
public:

    static void Init();
    static void Destroy();

    static Context&GetInst();
    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;
    Context(Context&&) = delete;
    Context& operator=(Context&&) = delete;
    ~Context();

    void update();
    void HandleEvents(const SDL_Event&e);
    bool ShouldExit();
private:
    Context();
    bool m_should_exit = false;
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Renderer>m_renderer;
    std::unique_ptr<ImageManager>m_image_manager;
    static std::unique_ptr<Context> instance;


    void UpdatePose();

    void logicUpdate();
    void renderUpdate();

};
