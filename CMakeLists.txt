set(VCPKG_TARGET_TRIPLET "x64-mingw-dynamic" CACHE STRING "")
set(CMAKE_TOOLCHAIN_FILE "D:/CodeSource/vcpkg/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

cmake_minimum_required(VERSION 3.25)
project(FireEmblem VERSION 0.0.1)

file(GLOB_RECURSE SRC ./src/*.cpp ./src/*.c)
file(GLOB_RECURSE ASSETS ./assets/*)
file(GLOB_RECURSE INCLUDE ./include/*.hpp ./include/*.h)
set(GAME_NAME ${PROJECT_NAME})

add_executable(${GAME_NAME})

# ── imgui (本地 submodule，不用vcpkg) ──────────────────
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/3rdparty/imgui)
set(IMGUI_SOURCES
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
        ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)
add_library(imgui_local STATIC ${IMGUI_SOURCES})
target_include_directories(imgui_local PUBLIC
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
)
# imgui 需要知道 SDL3 头文件位置
find_package(SDL3 REQUIRED CONFIG)
target_link_libraries(imgui_local PUBLIC SDL3::SDL3)
# ────────────────────────────────────────────────────────

target_sources(${GAME_NAME} PRIVATE ${SRC} ${ASSETS} ${INCLUDE})
target_include_directories(${GAME_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_features(${GAME_NAME} PRIVATE cxx_std_17)

# 去掉 find_package(imgui CONFIG REQUIRED)
find_package(SDL3_image REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(tinyxml2 CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
        imgui_local          # ← 改这里
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        tinyxml2::tinyxml2
)

target_compile_definitions(${GAME_NAME} PRIVATE SPDLOG_SHARED_LIB)

if(WIN32)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_if_different
          $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
          $<TARGET_FILE_DIR:${PROJECT_NAME}>
          COMMAND_EXPAND_LISTS
  )
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(${GAME_NAME} PRIVATE -g)
endif()
