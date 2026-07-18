#pragma once

#include <string>

class Enemy {
public:
    Enemy(std::string name, int maxHealth, int attackPower);

    void takeDamage(int damage);
    std::string getName() const;
    int getHealth() const;
    bool isDead() const;
    int getAttackDamage() const;

private:
    std::string name_;
    int health_;
    int maxHealth_;
    int attackPower_;
};