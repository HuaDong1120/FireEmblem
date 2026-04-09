#include "terrain.hpp"
#include <array>

// 地形数据表：顺序与 TerrainType 枚举一致
// move_cost=99 表示不可通行
static constexpr std::array<TerrainData, 15> TERRAIN_TABLE = {{
    { TerrainType::Plain,    "平原", 1,  0,  0,  0  },
    { TerrainType::Forest,   "树林", 2,  1, 20,  0  },
    { TerrainType::Mountain, "山岳", 4,  3, 30,  0  },
    { TerrainType::Sea,      "海洋", 99, 0,  0,  0  },
    { TerrainType::River,    "河流", 5,  0,  0,  0  },
    { TerrainType::Desert,   "沙漠", 4,  0,  0,  0  },
    { TerrainType::Bridge,   "桥梁", 1,  0,  0,  0  },
    { TerrainType::Road,     "道路", 1,  0,  0,  0  },
    { TerrainType::Village,  "村庄", 1,  0,  0,  0  },
    { TerrainType::Fortress, "要塞", 2,  2, 20, 20  },
    { TerrainType::Gate,     "城门", 1,  2, 20,  0  },
    { TerrainType::Throne,   "王座", 1,  4, 30, 30  },
    { TerrainType::Wall,     "城墙", 99, 0,  0,  0  },
    { TerrainType::Cliff,    "断崖", 99, 0,  0,  0  },
    { TerrainType::Unknown,  "未知", 1,  0,  0,  0  },
}};

const TerrainData& GetTerrainData(TerrainType t) {
    for (const auto& d : TERRAIN_TABLE) {
        if (d.type == t) return d;
    }
    return TERRAIN_TABLE.back();
}

TerrainType ParseTerrainType(const std::string& s) {
    if (s == "Plain")    return TerrainType::Plain;
    if (s == "Forest")   return TerrainType::Forest;
    if (s == "Mountain") return TerrainType::Mountain;
    if (s == "Sea")      return TerrainType::Sea;
    if (s == "River")    return TerrainType::River;
    if (s == "Desert")   return TerrainType::Desert;
    if (s == "Bridge")   return TerrainType::Bridge;
    if (s == "Road")     return TerrainType::Road;
    if (s == "Village")  return TerrainType::Village;
    if (s == "Fortress") return TerrainType::Fortress;
    if (s == "Gate")     return TerrainType::Gate;
    if (s == "Throne")   return TerrainType::Throne;
    if (s == "Wall")     return TerrainType::Wall;
    if (s == "Cliff")    return TerrainType::Cliff;
    return TerrainType::Unknown;
}
