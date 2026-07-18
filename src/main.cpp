#include "game/Player.hpp"
#include "game/Enemy.hpp"

#include <iostream>

int main() {
    Player player{"勇者", 80};
    Enemy slime{"史莱姆", 40, 8};
    const int playerDamage = 20;

    // 第一回合
    std::cout << player.getName() << " 攻击 " << slime.getName()
              << "，造成 " << playerDamage << " 点伤害。\n";
    slime.takeDamage(playerDamage);
    std::cout << slime.getName() << " HP: " << slime.getHealth() << '\n';

    if (!slime.isDead()) {
        std::cout << slime.getName() << " 反击，造成 "
                  << slime.getAttackDamage() << " 点伤害。\n";
        player.takeDamage(slime.getAttackDamage());
        std::cout << player.getName() << " HP: " << player.getHealth() << "\n\n";
    }

    // 第二回合
    std::cout << player.getName() << " 攻击 " << slime.getName()
              << "，造成 " << playerDamage << " 点伤害。\n";
    slime.takeDamage(playerDamage);

    if (slime.isDead()) {
        std::cout << slime.getName() << " 被击败！\n";
    } else {
        std::cout << slime.getName() << " HP: " << slime.getHealth() << '\n';
        std::cout << slime.getName() << " 反击，造成 "
                  << slime.getAttackDamage() << " 点伤害。\n";
        player.takeDamage(slime.getAttackDamage());
        std::cout << player.getName() << " HP: " << player.getHealth() << '\n';
    }

    return 0;
}
