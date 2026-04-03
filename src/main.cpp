

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_oldnames.h>
#include <spdlog/spdlog.h>

#define SDL_MAIN_USE_CALLBACKS
#include "SDL3/SDL.h"
#include "log.hpp"
#include "SDL3/SDL_main.h"

SDL_AppResult SDL_AppInit(void **appstate,int argc,char **argv)
{
    SDL_Init(SDL_INIT_EVENTS|SDL_INIT_VIDEO|SDL_INIT_JOYSTICK|SDL_INIT_GAMEPAD);
    Logger::instance().enableFile(true);
    Logger::instance().initialize("logs", "FireEmblem");
    LOG_INFO("game start");
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate,SDL_Event*event)
{
    return SDL_APP_CONTINUE;
}
void SDL_AppQuit(void *appstate,SDL_AppResult result)
{
    SDL_Quit();
    LOG_INFO("game exits");
}
