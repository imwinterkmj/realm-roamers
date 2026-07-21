#pragma once

#include <string>

class Enemy;
class Player;

enum class CardType {
    Attack,
    Block
};

class Card {
public:
    Card(std::string name, CardType type, int energyCost, int value);

    std::string getName() const;
    CardType getType() const;
    int getEnergyCost() const;
    int getValue() const;
    // 消耗玩家能量后结算卡牌效果；能量不足时不改变战斗状态。
    bool play(Player& player, Enemy& enemy) const;

private:
    std::string name_;
    CardType type_;
    int energyCost_;
    int value_;
};
