#include "game/Player.hpp"

#include <algorithm>

Player::Player(std::string name, int maxHealth)
    : name_(name),
      health_(maxHealth),
      maxHealth_(maxHealth) {
}

void Player::startTurn() {
    block_ = 0;
    energy_ = kMaxEnergy;
}

void Player::takeDamage(int damage) {
    const int actualDamage = std::max(0, damage);
    const int blockedDamage = std::min(block_, actualDamage);

    block_ -= blockedDamage;
    health_ = std::max(0, health_ - (actualDamage - blockedDamage));
}

void Player::heal(int amount) {
    health_ = std::min(maxHealth_, health_ + std::max(0, amount));
}

void Player::gainBlock(int amount) {
    block_ += std::max(0, amount);
}

bool Player::spendEnergy(int amount) {
    if (amount < 0 || energy_ < amount) {
        return false;
    }

    energy_ -= amount;
    return true;
}

int Player::getHealth() const {
    return health_;
}

int Player::getBlock() const {
    return block_;
}

int Player::getEnergy() const {
    return energy_;
}

bool Player::isDead() const {
    return health_ <= 0;
}

std::string Player::getName() const {
    return name_;
}
