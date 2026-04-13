

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_system.h>
#include <spdlog/spdlog.h>

#define SDL_MAIN_USE_CALLBACKS
#include "SDL3/SDL.h"
#include "log.hpp"
#include "SDL3/SDL_main.h"
#include "context.hpp"
SDL_AppResult SDL_AppInit(void **appstate,int argc,char **argv)
{
    Logger::instance().enableFile(true);
    Logger::instance().initialize("logs", "FireEmblem");
    LOG_INFO("game start");
    Context::Init();
   
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{

    auto& ctx = Context::GetInst();
    if(ctx.ShouldExit())
    {
        return  SDL_APP_SUCCESS;
    }
    Context::GetInst().update();
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate,SDL_Event*event)
{
    Context::GetInst().HandleEvents(*event);
    return SDL_APP_CONTINUE;
}
void SDL_AppQuit(void *appstate,SDL_AppResult result)
{
    Context::Destroy();
    LOG_INFO("game exits");
}
