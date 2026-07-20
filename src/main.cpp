#include "game/Card.hpp"
#include "game/Player.hpp"
#include "game/Enemy.hpp"

#include <iostream>
#include <vector>

namespace {

std::string getCardTypeName(CardType type) {
    switch (type) {
    case CardType::Attack:
        return "攻击";
    case CardType::Block:
        return "防御";
    }

    return "未知";
}

void printDeck(const std::vector<Card>& deck) {
    std::cout << "卡组：\n";
    for (const Card& card : deck) {
        std::cout << "- " << card.getName()
                  << " | " << getCardTypeName(card.getType())
                  << " | 能量: " << card.getEnergyCost()
                  << " | 数值: " << card.getValue() << '\n';
    }
    std::cout << '\n';
}

} // namespace

int main() {
    const std::vector<Card> deck{
        {"打击", CardType::Attack, 1, 6},
        {"防御", CardType::Block, 1, 5}
    };
    printDeck(deck);

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
