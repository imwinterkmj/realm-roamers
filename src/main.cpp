#include "game/Player.hpp"

#include <iostream>

int main() {
    Player player{"勇者", 80};

    player.takeDamage(25);
    std::cout << "HP: " << player.getHealth() << '\n';

    player.takeDamage(100);
    std::cout << "死亡了吗？" << player.isDead() << '\n';

    return 0;
}