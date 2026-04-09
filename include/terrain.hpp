#pragma once
#include <string>

// Fire Emblem 地形类型
// 参考 GBA 系列（烈火/圣魔）的地形数据
enum class TerrainType {
    Plain,      // 平原
    Forest,     // 树林
    Mountain,   // 山岳
    Sea,        // 海洋（大多数兵种不可通行）
    River,      // 河流
    Desert,     // 沙漠
    Bridge,     // 桥梁
    Road,       // 道路
    Village,    // 村庄
    Fortress,   // 要塞（每回合回复 HP）
    Gate,       // 城门
    Throne,     // 王座（回复更多 HP）
    Wall,       // 城墙（不可通行）
    Cliff,      // 断崖（不可通行）
    Unknown,    // 未知
};

// 某种地形的属性数据（仅针对步兵，骑兵/飞行兵种另算）
struct TerrainData {
    TerrainType  type;
    const char*  name;
    int          move_cost;   // 步兵通行费（99 = 不可通行）
    int          def_bonus;   // 防御加成（%）
    int          avo_bonus;   // 回避加成
    int          heal_rate;   // 每回合回复 HP%（0 = 不回复）
};

const TerrainData& GetTerrainData(TerrainType t);

// 从 Tiled 属性字符串解析地形类型（如 "Forest" -> TerrainType::Forest）
TerrainType ParseTerrainType(const std::string& s);
