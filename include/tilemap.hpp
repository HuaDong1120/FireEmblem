#pragma once
#include "math.hpp"
#include "terrain.hpp"
#include "path.hpp"
#include "renderer.hpp"
#include <vector>
#include <string>
#include <unordered_map>

class ImageManager;

// ---------------------------------------------------------------------------
// TileID
//   封装 Tiled 的全局 Tile ID（GID）。
//   Tiled 导出 JSON 时高 3 位编码翻转/对角旋转标志。
// ---------------------------------------------------------------------------
struct TileID {
    static constexpr uint32_t FLIP_H   = 0x80000000u;
    static constexpr uint32_t FLIP_V   = 0x40000000u;
    static constexpr uint32_t FLIP_D   = 0x20000000u;
    static constexpr uint32_t GID_MASK = 0x1FFFFFFFu;

    uint32_t raw{0};

    bool IsEmpty() const { return (raw & GID_MASK) == 0; }
    int  GID()     const { return static_cast<int>(raw & GID_MASK); }
    bool FlipH()   const { return (raw & FLIP_H) != 0; }
    bool FlipV()   const { return (raw & FLIP_V) != 0; }
};

// ---------------------------------------------------------------------------
// TileLayer
//   对应 Tiled JSON 中一个 tilelayer。
//   排列方式：行优先（tiles[row * width + col]）
// ---------------------------------------------------------------------------
struct TileLayer {
    std::string         name;
    int                 width{}, height{};
    std::vector<TileID> tiles;

    TileID Get(int col, int row) const { return tiles[row * width + col]; }
    bool   InRange(int col, int row) const {
        return col >= 0 && row >= 0 && col < width && row < height;
    }
};

// ---------------------------------------------------------------------------
// TilesetInfo
//   对应 Tiled 中一个图块集（Tileset）。
// ---------------------------------------------------------------------------
struct TilesetInfo {
    int         firstgid{1};
    int         tilewidth{}, tileheight{};
    int         columns{};
    int         tilecount{};
    std::string image_key; // ImageManager 中使用的 key（绝对路径字符串）

    // local_tile_id -> TerrainType（由 Tiled 自定义属性 "terrain" 读取）
    std::unordered_map<int, TerrainType> tile_terrain;

    bool Owns(int gid) const {
        return gid >= firstgid && gid < firstgid + tilecount;
    }

    // 根据 gid 计算在 tileset 图片中的源矩形
    Region GetSrcRegion(int gid) const;
};

// ---------------------------------------------------------------------------
// TileMap
//   加载 Tiled 导出的 JSON 地图（.tmj / .json），功能：
//     - 多图层（tilelayer）渲染
//     - 内嵌或外部（.tsj）tileset
//     - 通过 tile 属性 "terrain" 自动构建地形网格
//     - 摄像机偏移（地图滚动）
//     - 网格 <-> 屏幕坐标互转
//
//   在 Tiled 中配置地形类型的方式：
//     1. 选中 tileset 中的某个 tile
//     2. 在属性面板添加自定义属性：名称 "terrain"，类型 string
//     3. 值填写：Plain / Forest / Mountain / Sea / River / Desert /
//                Bridge / Road / Village / Fortress / Gate / Throne /
//                Wall / Cliff
// ---------------------------------------------------------------------------
class TileMap {
public:
    TileMap() = default;

    // 从 Tiled JSON 文件加载地图（.tmj / .json）
    bool Load(const Path& json_path, ImageManager& img_mgr, Renderer& renderer);

    // 渲染所有 tilelayer，camera_offset 为像素级滚动偏移
    void Render(Renderer& renderer, ImageManager& img_mgr,
                Vec2 camera_offset = {0.f, 0.f}) const;

    // 查询 (col, row) 处的地形类型
    TerrainType GetTerrain(int col, int row) const;

    // 网格坐标 -> 屏幕左上角像素坐标
    Vec2  GridToScreen(int col, int row, Vec2 camera_offset = {0.f, 0.f}) const;

    // 屏幕像素坐标 -> 网格坐标
    Vec2I ScreenToGrid(Vec2 screen_pos, Vec2 camera_offset = {0.f, 0.f}) const;

    int  GetMapWidth()   const { return m_width; }
    int  GetMapHeight()  const { return m_height; }
    int  GetTileWidth()  const { return m_tilewidth; }
    int  GetTileHeight() const { return m_tileheight; }
    bool IsLoaded()      const { return m_loaded; }

private:
    int  m_width{}, m_height{};
    int  m_tilewidth{}, m_tileheight{};

    std::vector<TileLayer>   m_layers;
    std::vector<TilesetInfo> m_tilesets;
    MatStorage<TerrainType>  m_terrain_grid;

    bool m_loaded{false};

    const TilesetInfo* FindTileset(int gid) const;
    void               BuildTerrainGrid();
};
