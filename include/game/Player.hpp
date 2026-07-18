#pragma once

#include <string>

class Player {
public:
    Player(std::string name, int maxHealth);

    void takeDamage(int damage);

    int getHealth() const;
    bool isDead() const;

private:
    std::string name_;
    int health_;
    int maxHealth_;
};