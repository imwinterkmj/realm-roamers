#include "game/Player.hpp"

Player::Player(std::string name, int maxHealth)
    : name_(name),
      health_(maxHealth),
      maxHealth_(maxHealth) {
}

void Player::takeDamage(int damage) {
    health_ -= damage;
    if (health_ < 0) {
        health_ = 0;
    }
}

int Player::getHealth() const {
    return health_;
}

bool Player::isDead() const {
    return health_ <= 0;
}

std::string Player::getName() const {
    return name_;
}