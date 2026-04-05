#pragma once
#include "SDL3/SDL.h"
#include "math.hpp"
#include "path.hpp"
#include <memory>
#include <unordered_map>
class Renderer;

class Image {
public:
    Image() = default;
    Image(Renderer& renderer, SDL_Surface* surface);
    Image(Renderer& renderer, const Path& filename);
    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;
    Image(Image&&) noexcept;
    Image& operator=(Image&&) noexcept;
    ~Image();

    Vec2 GetSize() const;

    [[nodiscard]] SDL_Texture* GetTexture() const;
    void ChangeColorMask(const Color& color);

private:
    SDL_Texture* m_texture{};
};
class ImageManager{
public:
    explicit ImageManager(Renderer&renderer);
    Image* load(const Path&filename);
    Image* Find(const Path&filename);
    bool isExists(const Path&filename);

private:
    std::unordered_map<Path, std::unique_ptr<Image>>m_images;
    Renderer&m_renderer;

};
