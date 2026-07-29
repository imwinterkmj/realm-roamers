#pragma once

#include "game/Card.hpp"

#include <string>
#include <vector>

// 一行卡牌配置对应一个可加入卡组的基础定义，不包含对局中的临时状态。
struct CardDefinition {
    std::string id;
    std::string name;
    CardType type;
    int energyCost;
    int value;
    int copies;
};

// 敌人战斗前的静态数值；战斗中的当前生命仍由 Enemy 对象管理。
struct EnemyDefinition {
    std::string id;
    std::string name;
    int maxHealth;
    int attackPower;
};

// 从 data/ 读取游戏数值。后续新增遗物、事件时可沿用同一入口。
class GameData {
public:
    static GameData load();

    const std::vector<CardDefinition>& getStarterDeck() const;
    const EnemyDefinition& getEnemy(const std::string& id) const;

private:
    std::vector<CardDefinition> starterDeck_;
    std::vector<EnemyDefinition> enemies_;
};
