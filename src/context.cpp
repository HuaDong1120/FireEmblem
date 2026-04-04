#include "context.hpp"
#include "log.hpp"
#include "render.hpp"
#include "window.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_oldnames.h>
#include <memory>
#include "SDL3/SDL.h"
std::unique_ptr<Context> Context::instance = nullptr;
void Context::Init()
{
    if(!instance)
    {
        instance.reset(new Context);
    }else{
        LOG_WARNING("inited context singlton twice!");
    }

}

void Context::Destroy()
{
    instance.reset();
}


Context& Context::GetInst()
{
    return *instance;
}
void Context::update()
{
    //TODO：
    m_renderer->Clear();

    m_renderer->Present();
}
void Context::HandleEvents(const SDL_Event&e)
{
    if(e.type == SDL_EVENT_QUIT)
    {
        m_should_exit = true;
    }
}

bool Context::ShouldExit()
{
    return m_should_exit;
}

Context::Context()
{

    SDL_Init(SDL_INIT_EVENTS|SDL_INIT_VIDEO|SDL_INIT_JOYSTICK|SDL_INIT_GAMEPAD) ;
    m_window = std::make_unique<Window>("FireEmblem",1024,720);
    m_renderer = std::make_unique<Renderer>(*m_window);
    m_renderer->SetClearColor({0.3,0.3,0.3,1});
}
Context::~Context()
{
    m_window.reset();
    m_renderer.reset();
    SDL_Quit();
}
