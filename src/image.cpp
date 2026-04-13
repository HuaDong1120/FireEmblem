#include "image.hpp"
#include "log.hpp"
#include "renderer.hpp"
#include "sdl_call.hpp"
#include <SDL3/SDL_blendmode.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <fmt/chrono.h>
#include <vector>
#include <memory>
#include <SDL3/SDL_storage.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
Image::Image(Renderer& renderer, SDL_Surface* surface) {
    m_texture = SDL_CreateTextureFromSurface(
        renderer.GetRenderer(), surface);
    if (!surface) {
        LOG_INFO("create texture from surface failed");
        return;
    }
    SDL_DestroySurface(surface);
}

Image::Image(Renderer& renderer,const Path& filename)
{
    int w, h;
    SDL_Storage* storage = SDL_OpenFileStorage(".");
    if (!storage)
    {
        LOG_ERROR("create storage failed");
    }
    while (!SDL_StorageReady(storage))
    {
        SDL_Delay(10);
    }
    Uint64 file_size = 0;
    std::string filepath = filename.generic_string();
    SDL_CALL(SDL_GetStorageFileSize(storage,filepath.c_str(),&file_size));
    
    LOG_INFO("loading image: [{}]", filepath);
    if (file_size == 0) {
        LOG_ERROR("file not found or size=0: {}", filepath);
        SDL_CloseStorage(storage);
        return;
    }
    
    std::vector<char> content(file_size);
    SDL_CALL(SDL_ReadStorageFile(storage,filepath.c_str(),content.data(),file_size));
    
    SDL_CloseStorage(storage);
    
    stbi_uc* data =stbi_load_from_memory((const stbi_uc*)content.data(),content.size(),&w,&h,nullptr,STBI_rgb_alpha);

    if(!data)
    {
        LOG_ERROR("load image {} failed",filename);
        return ;
    }

    SDL_Renderer* sdl_renderer = renderer.GetRenderer();
    SDL_Surface* surface =
        SDL_CreateSurfaceFrom(w, h, SDL_PIXELFORMAT_RGBA32, data, w * 4);
    if (!surface) {
        LOG_INFO("create SDL surface from {} failed: {}", filename.string(), SDL_GetError());
        stbi_image_free(data);
        return;
    }
    m_texture = SDL_CreateTextureFromSurface(sdl_renderer, surface);
    if (!m_texture) {
        LOG_INFO("create SDL texture from {} failed: {}", filename.string(), SDL_GetError());
        stbi_image_free(data);
        SDL_DestroySurface(surface);
        return;
    }

    SDL_DestroySurface(surface);
    stbi_image_free(data);

    SDL_CALL(SDL_SetTextureBlendMode(m_texture, SDL_BLENDMODE_BLEND));

    SDL_CALL(SDL_SetTextureScaleMode(m_texture, SDL_SCALEMODE_NEAREST)); //关闭线性过滤，保持像素锐利

}

Image::Image(Image&& o) noexcept : m_texture{o.m_texture} {
    o.m_texture = nullptr;
}

Image& Image::operator=(Image&& o) noexcept {
    if (&o != this) {
        m_texture = o.m_texture;
        o.m_texture = nullptr;
    }
    return *this;
}

Image::~Image() {
    SDL_DestroyTexture(m_texture);
}

Vec2 Image::GetSize() const {
    Vec2 size;
    if (!m_texture) {
        return {};
    }
    SDL_CALL(SDL_GetTextureSize(m_texture, &size.w, &size.h));
    return size;
}

SDL_Texture* Image::GetTexture() const {
    return m_texture;
}

void Image::ChangeColorMask(const Color& color) {
    SDL_SetTextureColorMod(m_texture, color.r * 255, color.g * 255, color.b * 255);
    SDL_SetTextureAlphaMod(m_texture, color.a * 255);
}



ImageManager::ImageManager(Renderer&render):m_renderer(render)
{

}

Image* ImageManager::load(const Path&filename)
{
    if(auto it = m_images.find(filename);it!=m_images.end())
    {
        LOG_WARNING("image{}already loaded",filename.string());
        return it->second.get();
    }
    auto result = m_images.emplace(filename,std::make_unique<Image>(m_renderer,filename));
    if(!result.second)
    {
        LOG_ERROR("emplace image failed!");
        return nullptr;
    }
    return result.first->second.get();
}

Image* ImageManager::Find(const Path&filename)
{
    if(auto it = m_images.find(filename);it!=m_images.end())
    {
        return it->second.get();
    }
    return nullptr;
}

bool ImageManager::isExists(const Path&filename)
{
    return Find(filename);
}
