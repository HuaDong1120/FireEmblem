set(VCPKG_TARGET_TRIPLET "x64-mingw-dynamic" CACHE STRING "")
set(CMAKE_TOOLCHAIN_FILE "D:/CodeSource/vcpkg/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

cmake_minimum_required(VERSION 3.25)
project(FireEmblem VERSION 0.0.1)


file(GLOB_RECURSE SRC ./src/*.cpp ./src/*.c)
file(GLOB_RECURSE ASSETS ./assets/*)
file(GLOB_RECURSE INCLUDE ./include/*.hpp ./include/*.h)
set(GAME_NAME ${PROJECT_NAME})

add_executable(${GAME_NAME})

target_sources(${GAME_NAME} PRIVATE ${SRC} ${ASSETS} ${INCLUDE})
target_include_directories(${GAME_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_features(${GAME_NAME} PRIVATE cxx_std_17)


find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_image REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)
find_package(imgui CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
#set_target_properties(${GAME_NAME} PROPERTIES
#    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
#)
target_link_libraries(${PROJECT_NAME} PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
        imgui::imgui
        spdlog::spdlog
        nlohmann_json::nlohmann_json
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
