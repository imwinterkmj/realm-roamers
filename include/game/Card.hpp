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
    bool play(Player& player, Enemy& enemy) const;

private:
    std::string name_;
    CardType type_;
    int energyCost_;
    int value_;
};
