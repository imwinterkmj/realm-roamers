#include "game/Battle.hpp"
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

std::vector<Card> createStarterDeck() {
    std::vector<Card> deck;
    for (int index = 0; index < 5; ++index) {
        deck.emplace_back("打击", CardType::Attack, 1, 6);
        deck.emplace_back("防御", CardType::Block, 1, 5);
    }
    return deck;
}

void printBattleState(const Battle& battle, const Player& player, const Enemy& enemy) {
    std::cout << "\n[玩家回合] " << player.getName()
              << " HP: " << player.getHealth()
              << " | 格挡: " << player.getBlock()
              << " | 能量: " << player.getEnergy() << '\n';
    std::cout << enemy.getName() << " HP: " << enemy.getHealth() << '\n';
    std::cout << "抽牌堆: " << battle.getDrawPileSize()
              << " | 弃牌堆: " << battle.getDiscardPileSize() << "\n手牌：\n";

    const std::vector<Card>& hand = battle.getHand();
    for (std::size_t index = 0; index < hand.size(); ++index) {
        const Card& card = hand[index];
        std::cout << index + 1 << ". " << card.getName()
                  << "（" << getCardTypeName(card.getType())
                  << "，能量 " << card.getEnergyCost()
                  << "，数值 " << card.getValue() << "）\n";
    }
}

} // namespace

int main() {
    Player player{"勇者", 80};
    Enemy slime{"史莱姆", 36, 7};
    Battle battle{player, slime, createStarterDeck(), 42};

    std::cout << "=== C++ Card Roguelike：命令行战斗 ===\n";

    while (!battle.isOver()) {
        battle.startPlayerTurn();
        bool playerEndedTurn = false;

        while (!battle.isOver() && !playerEndedTurn) {
            printBattleState(battle, player, slime);
            std::cout << "输入手牌编号出牌，输入 0 结束回合： ";

            int choice = 0;
            if (!(std::cin >> choice)) {
                std::cout << "\n输入结束，战斗已退出。\n";
                return 0;
            }

            if (choice == 0) {
                playerEndedTurn = true;
                continue;
            }

            const std::vector<Card>& hand = battle.getHand();
            if (choice < 1 || static_cast<std::size_t>(choice) > hand.size()) {
                std::cout << "无效的手牌编号。\n";
                continue;
            }

            const Card playedCard = hand[static_cast<std::size_t>(choice - 1)];
            if (!battle.playCard(static_cast<std::size_t>(choice - 1))) {
                std::cout << "能量不足，无法打出 " << playedCard.getName() << "。\n";
                continue;
            }

            std::cout << "打出 " << playedCard.getName() << "。\n";
        }

        if (!battle.isOver()) {
            std::cout << "\n[敌人回合] " << slime.getName() << " 攻击，造成 "
                      << slime.getAttackDamage() << " 点伤害。\n";
            battle.endPlayerTurn();
            std::cout << player.getName() << " HP: " << player.getHealth()
                      << " | 格挡: " << player.getBlock() << '\n';
        }
    }

    if (player.isDead()) {
        std::cout << "\n勇者被击败了。\n";
    } else {
        std::cout << "\n史莱姆被击败，战斗胜利！\n";
    }

    return 0;
}
