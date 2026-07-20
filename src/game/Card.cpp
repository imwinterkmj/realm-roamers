#include "game/Card.hpp"
#include "game/Enemy.hpp"
#include "game/Player.hpp"

#include <utility>

Card::Card(std::string name, CardType type, int energyCost, int value)
    : name_(std::move(name)),
      type_(type),
      energyCost_(energyCost),
      value_(value) {
}

std::string Card::getName() const {
    return name_;
}

CardType Card::getType() const {
    return type_;
}

int Card::getEnergyCost() const {
    return energyCost_;
}

int Card::getValue() const {
    return value_;
}

bool Card::play(Player& player, Enemy& enemy) const {
    if (!player.spendEnergy(energyCost_)) {
        return false;
    }

    switch (type_) {
    case CardType::Attack:
        enemy.takeDamage(value_);
        return true;
    case CardType::Block:
        player.gainBlock(value_);
        return true;
    }

    return false;
}
