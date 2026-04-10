#include "inspector.hpp"

#include <cstdio>
#include <SDL3/SDL.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>
#include <imgui_internal.h>   
#include "window.hpp"
#include "renderer.hpp"
Inspector::Inspector(Window& window, Renderer& renderer):
m_window(window),m_renderer(renderer)
{


    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    // ← 加这行
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // ← 加这行
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(window.GetWindow(), renderer.GetRenderer());
    ImGui_ImplSDLRenderer3_Init(renderer.GetRenderer());

   
}
Inspector::~Inspector()
{
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

void Inspector::BeginFrame()const
{
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void Inspector::EndFrame()const
{
    ImGui::Render();
    auto& io = ImGui::GetIO();
    SDL_SetRenderScale(m_renderer.GetRenderer(), io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), m_renderer.GetRenderer());
    
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}
void Inspector::Update()
{  
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    
    ImGuiWindowFlags host_flags = 
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("DockSpace", nullptr, host_flags);
    ImGui::PopStyleVar();

    // 创建 DockSpace
    ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0, 0),ImGuiDockNodeFlags_PassthruCentralNode);  // ← 加这个;

    ImGui::End();

    // 你的面板窗口
    //ImGui::Begin("Inspector");
    //ImGui::Text("Hello");
    //ImGui::End();

    ImGui::Begin("Hierarchy");
    ImGui::Text("Hello");
    ImGui::End();
    //TODO:logic
    //ImGui::ShowDemoWindow();
}
void Inspector::HandleEvent(const SDL_Event& event)
{
    ImGui_ImplSDL3_ProcessEvent(&event);
}