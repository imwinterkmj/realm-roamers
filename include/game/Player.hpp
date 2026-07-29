#pragma once

#include <string>

class Player {
public:
    Player(std::string name, int maxHealth);

    void startTurn();
    void takeDamage(int damage);
    void heal(int amount);
    void gainBlock(int amount);
    bool spendEnergy(int amount);

    int getHealth() const;
    int getBlock() const;
    int getEnergy() const;
    bool isDead() const;
    std::string getName() const;

private:
    std::string name_;
    int health_;
    int maxHealth_;
    int block_ = 0;
    int energy_ = 0;

    static constexpr int kMaxEnergy = 3;
};
