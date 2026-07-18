#include "game/Enemy.hpp"

Enemy::Enemy(std::string name, int maxHealth, int attackPower)
    : name_(name),
      health_(maxHealth),
      maxHealth_(maxHealth),
      attackPower_(attackPower) {
}

void Enemy::takeDamage(int damage) {
    health_ -= damage;
    if (health_ < 0) {
        health_ = 0;
    }
}

int Enemy::getHealth() const {
    return health_;
}

bool Enemy::isDead() const {
    return health_ <= 0;
}

int Enemy::getAttackDamage() const {
    return attackPower_;
}

std::string Enemy::getName() const {
    return name_;
}