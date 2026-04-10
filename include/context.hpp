#pragma once


#include "renderer.hpp"
#include "window.hpp"
#include "image.hpp"
#include "tilemap.hpp"
#include "path.hpp"
#include "inspector.hpp"
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

    // 加载 Tiled JSON 地图文件
    void LoadMap(const Path& json_path);

    // 摄像机偏移（像素），控制地图滚动
    Vec2 m_camera_offset{0.f, 0.f};

private:
    Context();
    bool m_should_exit = false;
    std::unique_ptr<Window>       m_window;
    std::unique_ptr<Renderer>     m_renderer;
    std::unique_ptr<ImageManager> m_image_manager;
    std::unique_ptr<TileMap>      m_tilemap;
    std::unique_ptr<Inspector>   m_inspector;
    static std::unique_ptr<Context> instance;

    void UpdatePose();
    void logicUpdate();
    void renderUpdate();

};
