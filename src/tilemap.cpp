#include "tilemap.hpp"
#include "image.hpp"
#include "log.hpp"
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

// ============================================================
// TilesetInfo
// ============================================================

Region TilesetInfo::GetSrcRegion(int gid) const {
    int local_id = gid - firstgid;
    int col      = local_id % columns;
    int row      = local_id / columns;
    return Region{
        Vec2{static_cast<float>(col * tilewidth),
             static_cast<float>(row * tileheight)},
        Vec2{static_cast<float>(tilewidth),
             static_cast<float>(tileheight)}
    };
}

// ============================================================
// 内部辅助：从 JSON tileset 对象填充 TilesetInfo
// ============================================================

// 解析单个 tileset 对象（embedded 或 .tsj 文件内容）
// firstgid 由外部传入（.tsj 文件自身不含 firstgid）
static bool ParseTilesetObject(const json& ts_json,
                                int firstgid,
                                const Path& base_dir,
                                ImageManager& img_mgr,
                                TilesetInfo& out) {
    out.firstgid   = firstgid;
    out.tilewidth  = ts_json.value("tilewidth",  32);
    out.tileheight = ts_json.value("tileheight", 32);
    out.columns    = ts_json.value("columns",    1);
    out.tilecount  = ts_json.value("tilecount",  0);

    // 图片路径（相对于 JSON 文件所在目录）
    std::string img_rel = ts_json.value("image", "");
    if (img_rel.empty()) {
        LOG_ERROR("TileMap: tileset has no image field");
        return false;
    }
    Path img_path  = base_dir / img_rel;
    out.image_key  = img_path.string();
    img_mgr.load(img_path);

    // 读取每个 tile 的自定义属性 "terrain"
    if (ts_json.contains("tiles")) {
        for (const auto& tile_json : ts_json["tiles"]) {
            int local_id = tile_json.value("id", -1);
            if (local_id < 0) continue;
            if (!tile_json.contains("properties")) continue;
            for (const auto& prop : tile_json["properties"]) {
                if (prop.value("name", "") == "terrain") {
                    std::string val = prop.value("value", "");
                    out.tile_terrain[local_id] = ParseTerrainType(val);
                }
            }
        }
    }
    return true;
}

// ============================================================
// TileMap::Load
// ============================================================

bool TileMap::Load(const Path& json_path, ImageManager& img_mgr, Renderer& /*renderer*/) {
    std::ifstream file(json_path);
    if (!file.is_open()) {
        LOG_ERROR("TileMap: cannot open {}", json_path.string());
        return false;
    }

    json root;
    try {
        file >> root;
    } catch (const json::exception& e) {
        LOG_ERROR("TileMap: JSON parse error: {}", e.what());
        return false;
    }

    m_width      = root.value("width",      0);
    m_height     = root.value("height",     0);
    m_tilewidth  = root.value("tilewidth",  32);
    m_tileheight = root.value("tileheight", 32);

    if (m_width <= 0 || m_height <= 0) {
        LOG_ERROR("TileMap: invalid map size {}x{}", m_width, m_height);
        return false;
    }

    Path base_dir = json_path.parent_path();

    // ----------------------------------------------------------
    // 解析 tilesets
    // ----------------------------------------------------------
    for (const auto& ts_ref : root.value("tilesets", json::array())) {
        int firstgid = ts_ref.value("firstgid", 1);

        TilesetInfo ts_info;

        if (ts_ref.contains("source")) {
            // 外部 tileset（.tsj 文件）
            Path tsj_path = base_dir / ts_ref["source"].get<std::string>();
            std::ifstream tsj_file(tsj_path);
            if (!tsj_file.is_open()) {
                LOG_ERROR("TileMap: cannot open tileset file {}", tsj_path.string());
                return false;
            }
            json tsj_root;
            try {
                tsj_file >> tsj_root;
            } catch (const json::exception& e) {
                LOG_ERROR("TileMap: tileset JSON parse error: {}", e.what());
                return false;
            }
            // .tsj 的图片路径相对于 .tsj 文件自身所在目录
            if (!ParseTilesetObject(tsj_root, firstgid, tsj_path.parent_path(), img_mgr, ts_info))
                return false;
        } else {
            // 内嵌 tileset
            if (!ParseTilesetObject(ts_ref, firstgid, base_dir, img_mgr, ts_info))
                return false;
        }

        m_tilesets.push_back(std::move(ts_info));
    }

    // ----------------------------------------------------------
    // 解析 layers（只处理 tilelayer 类型）
    // ----------------------------------------------------------
    for (const auto& layer_json : root.value("layers", json::array())) {
        if (layer_json.value("type", "") != "tilelayer") continue;

        TileLayer layer;
        layer.name   = layer_json.value("name",   "");
        layer.width  = layer_json.value("width",  m_width);
        layer.height = layer_json.value("height", m_height);

        if (!layer_json.contains("data")) continue;
        const auto& data = layer_json["data"];

        // 仅支持默认数组格式（不支持 base64 压缩编码）
        if (!data.is_array()) {
            LOG_WARNING("TileMap: layer '{}' uses unsupported encoding, skipping", layer.name);
            continue;
        }

        layer.tiles.reserve(data.size());
        for (const auto& item : data) {
            layer.tiles.push_back(TileID{item.get<uint32_t>()});
        }

        m_layers.push_back(std::move(layer));
    }

    BuildTerrainGrid();

    m_loaded = true;
    LOG_INFO("TileMap: loaded {}x{} map, {} layers, from {}",
             m_width, m_height, m_layers.size(), json_path.string());
    return true;
}

// ============================================================
// TileMap::BuildTerrainGrid
//   遍历所有格子，从第一个有地形信息的图层取地形类型
// ============================================================

void TileMap::BuildTerrainGrid() {
    m_terrain_grid.Resize(m_width, m_height);
    for (int row = 0; row < m_height; ++row)
        for (int col = 0; col < m_width; ++col)
            m_terrain_grid.Set(TerrainType::Unknown, col, row);

    for (int row = 0; row < m_height; ++row) {
        for (int col = 0; col < m_width; ++col) {
            for (const auto& layer : m_layers) {
                if (!layer.InRange(col, row)) continue;
                TileID tid = layer.Get(col, row);
                if (tid.IsEmpty()) continue;
                const TilesetInfo* ts = FindTileset(tid.GID());
                if (!ts) continue;
                int local_id = tid.GID() - ts->firstgid;
                auto it = ts->tile_terrain.find(local_id);
                if (it != ts->tile_terrain.end()) {
                    m_terrain_grid.Set(it->second, col, row);
                    break; // 找到地形信息，跳出图层循环
                }
            }
        }
    }
}

// ============================================================
// TileMap::Render
// ============================================================

void TileMap::Render(Renderer& renderer, ImageManager& img_mgr, Vec2 camera_offset) const {
    if (!m_loaded) return;

    for (const auto& layer : m_layers) {
        for (int row = 0; row < layer.height; ++row) {
            for (int col = 0; col < layer.width; ++col) {
                TileID tid = layer.Get(col, row);
                if (tid.IsEmpty()) continue;

                const TilesetInfo* ts = FindTileset(tid.GID());
                if (!ts) continue;

                Image* img = img_mgr.Find(ts->image_key);
                if (!img) continue;

                Region src  = ts->GetSrcRegion(tid.GID());
                Vec2   pos  = GridToScreen(col, row, camera_offset);
                Region dst{ pos, Vec2{static_cast<float>(m_tilewidth),
                                      static_cast<float>(m_tileheight)} };

                // 处理翻转标志
                Flags<Flip> flip_flags{FlipNone};
                if (tid.FlipH()) flip_flags |= FlipHorizontal;
                if (tid.FlipV()) flip_flags |= FlipVertical;

                renderer.DrawImgage(*img, src, dst, 0, {0.f, 0.f}, flip_flags);
            }
        }
    }
}

// ============================================================
// TileMap：坐标转换 & 查询
// ============================================================

TerrainType TileMap::GetTerrain(int col, int row) const {
    if (col < 0 || row < 0 || col >= m_width || row >= m_height)
        return TerrainType::Unknown;
    return m_terrain_grid.Get(col, row);
}

Vec2 TileMap::GridToScreen(int col, int row, Vec2 camera_offset) const {
    return Vec2{
        static_cast<float>(col * m_tilewidth)  - camera_offset.x,
        static_cast<float>(row * m_tileheight) - camera_offset.y
    };
}

Vec2I TileMap::ScreenToGrid(Vec2 screen_pos, Vec2 camera_offset) const {
    return Vec2I{
        static_cast<int>((screen_pos.x + camera_offset.x) / static_cast<float>(m_tilewidth)),
        static_cast<int>((screen_pos.y + camera_offset.y) / static_cast<float>(m_tileheight))
    };
}

const TilesetInfo* TileMap::FindTileset(int gid) const {
    // 找 firstgid <= gid 中 firstgid 最大的 tileset（Tiled 规范）
    const TilesetInfo* result = nullptr;
    for (const auto& ts : m_tilesets) {
        if (gid >= ts.firstgid) {
            if (!result || ts.firstgid > result->firstgid)
                result = &ts;
        }
    }
    return result;
}
