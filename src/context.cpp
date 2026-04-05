#include "context.hpp"
#include "image.hpp"
#include "log.hpp"
#include "math.hpp"
#include "renderer.hpp"
#include "window.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_oldnames.h>
#include <complex>
#include <iterator>
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
    logicUpdate();
    renderUpdate();
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
    //m_renderer->SetClearColor({0.3,0.3,0.3,1});
    m_image_manager = std::make_unique<ImageManager>(*m_renderer);

    m_image_manager->load("assets/0302.png");
}
Context::~Context()
{
    m_image_manager.reset();
    m_window.reset();
    m_renderer.reset();
    SDL_Quit();
}

void Context::UpdatePose()
{

}

void Context::logicUpdate()
{

}
void Context::renderUpdate()
{
    m_renderer->Clear();
    Image* image = m_image_manager->Find("assets/0302.png");
    auto size =image->GetSize();
    Region src,dst;
    src.m_size = size;
    dst.m_size = size*0.1;
    dst.m_topleft = {0,0};
    m_renderer->DrawImgage(*image,src,dst,0,{0,0},Flip::FlipNone);
    m_renderer->Present();
}
