#include "game/Card.hpp"
#include "game/Enemy.hpp"
#include "game/Player.hpp"

#include <iostream>
#include <vector>

namespace {

const char* getCardTypeName(CardType type) {
    switch (type) {
    case CardType::Attack:
        return "攻击";
    case CardType::Block:
        return "防御";
    }

    return "未知";
}

void playCards(const std::vector<Card>& cards, Player& player, Enemy& enemy) {
    player.startTurn();
    std::cout << "\n[玩家回合] 能量恢复为 " << player.getEnergy() << "。\n";

    for (const Card& card : cards) {
        if (!card.play(player, enemy)) {
            std::cout << "能量不足，无法打出 " << card.getName() << "。\n";
            continue;
        }

        std::cout << "打出 " << card.getName()
                  << "（" << getCardTypeName(card.getType())
                  << "，消耗 " << card.getEnergyCost() << " 能量）。\n";

        if (card.getType() == CardType::Attack) {
            std::cout << enemy.getName() << " HP: " << enemy.getHealth() << '\n';
            if (enemy.isDead()) {
                std::cout << enemy.getName() << " 被击败！\n";
                return;
            }
        } else {
            std::cout << player.getName() << " 格挡: " << player.getBlock() << '\n';
        }
    }
}

void enemyTurn(Player& player, const Enemy& enemy) {
    std::cout << "\n[敌人回合] " << enemy.getName() << " 攻击，造成 "
              << enemy.getAttackDamage() << " 点伤害。\n";
    player.takeDamage(enemy.getAttackDamage());
    std::cout << player.getName() << " HP: " << player.getHealth()
              << " | 格挡: " << player.getBlock() << '\n';
}

} // namespace

int main() {
    Player player{"勇者", 80};
    Enemy slime{"史莱姆", 20, 8};

    const std::vector<Card> firstTurnCards{
        {"打击", CardType::Attack, 1, 6},
        {"打击", CardType::Attack, 1, 6},
        {"防御", CardType::Block, 1, 5}
    };
    const std::vector<Card> secondTurnCards{
        {"打击", CardType::Attack, 1, 6},
        {"打击", CardType::Attack, 1, 6}
    };

    playCards(firstTurnCards, player, slime);
    if (!player.isDead() && !slime.isDead()) {
        enemyTurn(player, slime);
    }

    if (!player.isDead() && !slime.isDead()) {
        playCards(secondTurnCards, player, slime);
    }

    return 0;
}
