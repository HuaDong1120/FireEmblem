#include "context.hpp"
#include "image.hpp"
#include "log.hpp"
#include "math.hpp"
#include "renderer.hpp"
#include "tilemap.hpp"
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
    logicUpdate();
    renderUpdate();
}
void Context::HandleEvents(const SDL_Event&e)
{
    m_inspector->HandleEvent(e);
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
    SDL_Init(SDL_INIT_EVENTS|SDL_INIT_VIDEO|SDL_INIT_JOYSTICK|SDL_INIT_GAMEPAD);
#ifdef SDL_PLATFORM_ANDROID
    SDL_SetHint(SDL_HINT_ORIENTATIONS,"LandscapeLeft");
#endif
    m_window       = std::make_unique<Window>("FireEmblem", 1024, 720);
    m_renderer     = std::make_unique<Renderer>(*m_window);
    m_image_manager = std::make_unique<ImageManager>(*m_renderer);
    m_inspector    = std::make_unique<Inspector>(*m_window, *m_renderer);
    m_tilemap = std::make_unique<TileMap>();
    m_tilemap->Load("assets/Fire.tmx", *m_image_manager, *m_renderer);
}
Context::~Context()
{
    m_image_manager.reset();
    m_window.reset();
    m_renderer.reset();
    m_tilemap.reset();
    m_inspector.reset();
    SDL_Quit();
}

void Context::UpdatePose()
{

}

void Context::logicUpdate()
{

}
void Context::LoadMap(const Path& json_path)
{
    if (!m_tilemap->Load(json_path, *m_image_manager, *m_renderer)) {
        LOG_ERROR("Context: failed to load map {}", json_path.string());
    }
}

void Context::renderUpdate()
{
    m_inspector->BeginFrame();
    m_renderer->Clear();

    if (m_tilemap->IsLoaded()) {
        m_tilemap->Render(*m_renderer, *m_image_manager, m_camera_offset);
    }
    m_inspector->Update();
    m_inspector->EndFrame();
    m_renderer->Present();
}
