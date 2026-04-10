#include "tilemap.hpp"
#include "image.hpp"
#include "log.hpp"
#include <nlohmann/json.hpp>
#include <tinyxml2.h>
#include <fstream>
#include <sstream>

using json = nlohmann::json;
namespace tx2 = tinyxml2;

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
// TileMap::Load  —— 按扩展名派发
// ============================================================

bool TileMap::Load(const Path& map_path, ImageManager& img_mgr, Renderer& /*renderer*/) {
    std::string ext = map_path.extension().string();
    // 转小写，兼容大小写混写
    for (auto& c : ext) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

    bool ok = false;
    if (ext == ".tmx") {
        ok = LoadTMX(map_path, img_mgr);
    } else if (ext == ".tmj" || ext == ".json") {
        ok = LoadJSON(map_path, img_mgr);
    } else {
        LOG_ERROR("TileMap: unsupported map format '{}'", ext);
        return false;
    }

    if (ok) {
        BuildTerrainGrid();
        m_loaded = true;
        LOG_INFO("TileMap: loaded {}x{} map, {} layer(s), from {}",
                 m_width, m_height, m_layers.size(), map_path.string());
    }
    return ok;
}

// ============================================================
// CSV 解析：把 "1,2,3,\n4,5,6" 转成 TileID 列表
// ============================================================

static std::vector<TileID> ParseCSV(const std::string& csv_text) {
    std::vector<TileID> result;
    std::istringstream ss(csv_text);
    std::string token;
    while (std::getline(ss, token, ',')) {
        // 去掉空白和换行
        std::string trimmed;
        for (char c : token)
            if (c != ' ' && c != '\n' && c != '\r' && c != '\t')
                trimmed += c;
        if (!trimmed.empty())
            result.push_back(TileID{static_cast<uint32_t>(std::stoul(trimmed))});
    }
    return result;
}

// ============================================================
// TSX 外部 Tileset 解析（tinyxml2）
// ============================================================

static bool ParseTSX(const Path& tsx_path, int firstgid,
                     ImageManager& img_mgr, TilesetInfo& out) {
    tx2::XMLDocument doc;
    if (doc.LoadFile(tsx_path.string().c_str()) != tx2::XML_SUCCESS) {
        LOG_ERROR("TileMap: cannot parse TSX file {}: {}", tsx_path.string(), doc.ErrorStr());
        return false;
    }

    tx2::XMLElement* root = doc.FirstChildElement("tileset");
    if (!root) {
        LOG_ERROR("TileMap: TSX missing <tileset> root: {}", tsx_path.string());
        return false;
    }

    out.firstgid   = firstgid;
    out.tilewidth  = root->IntAttribute("tilewidth",  32);
    out.tileheight = root->IntAttribute("tileheight", 32);
    out.columns    = root->IntAttribute("columns",    1);
    out.tilecount  = root->IntAttribute("tilecount",  0);

    // <image source="..."/>
    tx2::XMLElement* img_elem = root->FirstChildElement("image");
    if (!img_elem) {
        LOG_ERROR("TileMap: TSX '{}' has no <image> element", tsx_path.string());
        return false;
    }
    const char* src = img_elem->Attribute("source");
    if (!src) {
        LOG_ERROR("TileMap: TSX <image> missing source attribute");
        return false;
    }

    // source 可能是绝对路径或相对于 .tsx 文件的相对路径
    Path img_path(src);
    if (img_path.is_relative())
        img_path = tsx_path.parent_path() / img_path;

    out.image_key = img_path.string();
    img_mgr.load(img_path);

    // <tile id="N"> <properties> <property name="terrain" value="..."/>
    for (auto* tile = root->FirstChildElement("tile"); tile;
         tile = tile->NextSiblingElement("tile")) {
        int local_id = tile->IntAttribute("id", -1);
        if (local_id < 0) continue;
        auto* props = tile->FirstChildElement("properties");
        if (!props) continue;
        for (auto* prop = props->FirstChildElement("property"); prop;
             prop = prop->NextSiblingElement("property")) {
            const char* name = prop->Attribute("name");
            const char* val  = prop->Attribute("value");
            if (name && val && std::string(name) == "terrain")
                out.tile_terrain[local_id] = ParseTerrainType(val);
        }
    }
    return true;
}

// ============================================================
// TileMap::LoadTMX  —— 解析 .tmx（XML）
// ============================================================

bool TileMap::LoadTMX(const Path& tmx_path, ImageManager& img_mgr) {
    tx2::XMLDocument doc;
    if (doc.LoadFile(tmx_path.string().c_str()) != tx2::XML_SUCCESS) {
        LOG_ERROR("TileMap: cannot parse TMX file {}: {}", tmx_path.string(), doc.ErrorStr());
        return false;
    }

    tx2::XMLElement* map = doc.FirstChildElement("map");
    if (!map) {
        LOG_ERROR("TileMap: TMX missing <map> root element");
        return false;
    }

    m_width      = map->IntAttribute("width",      0);
    m_height     = map->IntAttribute("height",     0);
    m_tilewidth  = map->IntAttribute("tilewidth",  32);
    m_tileheight = map->IntAttribute("tileheight", 32);

    if (m_width <= 0 || m_height <= 0) {
        LOG_ERROR("TileMap: invalid map size {}x{}", m_width, m_height);
        return false;
    }

    Path base_dir = tmx_path.parent_path();

    // --- <tileset> ---
    for (auto* ts_elem = map->FirstChildElement("tileset"); ts_elem;
         ts_elem = ts_elem->NextSiblingElement("tileset")) {
        int firstgid = ts_elem->IntAttribute("firstgid", 1);
        TilesetInfo ts_info;

        const char* source = ts_elem->Attribute("source");
        if (source) {
            // 外部 .tsx 文件
            Path tsx_path = base_dir / source;
            if (!ParseTSX(tsx_path, firstgid, img_mgr, ts_info))
                return false;
        } else {
            // 内嵌 tileset（直接写在 tmx 里）
            ts_info.firstgid   = firstgid;
            ts_info.tilewidth  = ts_elem->IntAttribute("tilewidth",  m_tilewidth);
            ts_info.tileheight = ts_elem->IntAttribute("tileheight", m_tileheight);
            ts_info.columns    = ts_elem->IntAttribute("columns",    1);
            ts_info.tilecount  = ts_elem->IntAttribute("tilecount",  0);

            auto* img_elem = ts_elem->FirstChildElement("image");
            if (img_elem) {
                const char* src = img_elem->Attribute("source");
                if (src) {
                    Path img_path(src);
                    if (img_path.is_relative())
                        img_path = base_dir / img_path;
                    ts_info.image_key = img_path.string();
                    img_mgr.load(img_path);
                }
            }
            // tile properties
            for (auto* tile = ts_elem->FirstChildElement("tile"); tile;
                 tile = tile->NextSiblingElement("tile")) {
                int local_id = tile->IntAttribute("id", -1);
                if (local_id < 0) continue;
                auto* props = tile->FirstChildElement("properties");
                if (!props) continue;
                for (auto* prop = props->FirstChildElement("property"); prop;
                     prop = prop->NextSiblingElement("property")) {
                    const char* name = prop->Attribute("name");
                    const char* val  = prop->Attribute("value");
                    if (name && val && std::string(name) == "terrain")
                        ts_info.tile_terrain[local_id] = ParseTerrainType(val);
                }
            }
        }
        m_tilesets.push_back(std::move(ts_info));
    }

    // --- <layer> ---
    for (auto* layer_elem = map->FirstChildElement("layer"); layer_elem;
         layer_elem = layer_elem->NextSiblingElement("layer")) {
        TileLayer layer;
        layer.name   = layer_elem->Attribute("name") ? layer_elem->Attribute("name") : "";
        layer.width  = layer_elem->IntAttribute("width",  m_width);
        layer.height = layer_elem->IntAttribute("height", m_height);

        auto* data_elem = layer_elem->FirstChildElement("data");
        if (!data_elem) continue;

        const char* encoding = data_elem->Attribute("encoding");
        if (!encoding || std::string(encoding) == "csv") {
            const char* text = data_elem->GetText();
            if (!text) continue;
            layer.tiles = ParseCSV(text);
        } else {
            LOG_WARNING("TileMap: layer '{}' uses unsupported encoding '{}', skipping",
                        layer.name, encoding);
            continue;
        }

        m_layers.push_back(std::move(layer));
    }
    return true;
}

// ============================================================
// TileMap::LoadJSON  —— 解析 .tmj / .json
// ============================================================

static bool ParseTilesetJSON(const json& ts_json, int firstgid,
                              const Path& base_dir, ImageManager& img_mgr,
                              TilesetInfo& out) {
    out.firstgid   = firstgid;
    out.tilewidth  = ts_json.value("tilewidth",  32);
    out.tileheight = ts_json.value("tileheight", 32);
    out.columns    = ts_json.value("columns",    1);
    out.tilecount  = ts_json.value("tilecount",  0);

    std::string img_rel = ts_json.value("image", "");
    if (img_rel.empty()) {
        LOG_ERROR("TileMap: JSON tileset has no image field");
        return false;
    }
    Path img_path = base_dir / img_rel;
    out.image_key = img_path.string();
    img_mgr.load(img_path);

    if (ts_json.contains("tiles")) {
        for (const auto& tile_json : ts_json["tiles"]) {
            int local_id = tile_json.value("id", -1);
            if (local_id < 0) continue;
            if (!tile_json.contains("properties")) continue;
            for (const auto& prop : tile_json["properties"]) {
                if (prop.value("name", "") == "terrain")
                    out.tile_terrain[local_id] = ParseTerrainType(prop.value("value", ""));
            }
        }
    }
    return true;
}

bool TileMap::LoadJSON(const Path& json_path, ImageManager& img_mgr) {
    std::ifstream file(json_path);
    if (!file.is_open()) {
        LOG_ERROR("TileMap: cannot open {}", json_path.string());
        return false;
    }
    json root;
    try { file >> root; }
    catch (const json::exception& e) {
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

    for (const auto& ts_ref : root.value("tilesets", json::array())) {
        int firstgid = ts_ref.value("firstgid", 1);
        TilesetInfo ts_info;
        if (ts_ref.contains("source")) {
            Path tsj_path = base_dir / ts_ref["source"].get<std::string>();
            std::ifstream tsj_file(tsj_path);
            if (!tsj_file.is_open()) {
                LOG_ERROR("TileMap: cannot open tileset {}", tsj_path.string());
                return false;
            }
            json tsj_root;
            try { tsj_file >> tsj_root; }
            catch (const json::exception& e) {
                LOG_ERROR("TileMap: tileset JSON error: {}", e.what());
                return false;
            }
            if (!ParseTilesetJSON(tsj_root, firstgid, tsj_path.parent_path(), img_mgr, ts_info))
                return false;
        } else {
            if (!ParseTilesetJSON(ts_ref, firstgid, base_dir, img_mgr, ts_info))
                return false;
        }
        m_tilesets.push_back(std::move(ts_info));
    }

    for (const auto& layer_json : root.value("layers", json::array())) {
        if (layer_json.value("type", "") != "tilelayer") continue;
        TileLayer layer;
        layer.name   = layer_json.value("name",   "");
        layer.width  = layer_json.value("width",  m_width);
        layer.height = layer_json.value("height", m_height);
        if (!layer_json.contains("data")) continue;
        const auto& data = layer_json["data"];
        if (!data.is_array()) {
            LOG_WARNING("TileMap: layer '{}' has unsupported encoding, skipping", layer.name);
            continue;
        }
        layer.tiles.reserve(data.size());
        for (const auto& item : data)
            layer.tiles.push_back(TileID{item.get<uint32_t>()});
        m_layers.push_back(std::move(layer));
    }
    return true;
}

// ============================================================
// TileMap::BuildTerrainGrid
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
                    break;
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

                Region src = ts->GetSrcRegion(tid.GID());
                Vec2   pos = GridToScreen(col, row, camera_offset);
                Region dst{pos, Vec2{static_cast<float>(m_tilewidth),
                                     static_cast<float>(m_tileheight)}};

                Flags<Flip> flip_flags{FlipNone};
                if (tid.FlipH()) flip_flags |= FlipHorizontal;
                if (tid.FlipV()) flip_flags |= FlipVertical;

                renderer.DrawImgage(*img, src, dst, 0, {0.f, 0.f}, flip_flags);
            }
        }
    }
}

// ============================================================
// 查询 & 坐标转换
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
    const TilesetInfo* result = nullptr;
    for (const auto& ts : m_tilesets) {
        if (gid >= ts.firstgid) {
            if (!result || ts.firstgid > result->firstgid)
                result = &ts;
        }
    }
    return result;
}
