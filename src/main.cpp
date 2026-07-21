#include "game/Battle.hpp"
#include "game/Card.hpp"
#include "game/Enemy.hpp"
#include "game/GameSession.hpp"

#include <iostream>
#include <vector>

namespace {

enum class BattleOutcome { Victory, Defeat, InputClosed };

const char* getCardTypeName(CardType type) {
    switch (type) {
    case CardType::Attack: return "攻击";
    case CardType::Block: return "防御";
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

void printBattleState(const Battle& battle, const Player& player, const Enemy& enemy,
                      std::size_t permanentDeckSize) {
    std::cout << "\n[玩家回合] " << player.getName()
              << " HP: " << player.getHealth()
              << " | 格挡: " << player.getBlock()
              << " | 能量: " << player.getEnergy() << '\n';
    std::cout << enemy.getName() << " HP: " << enemy.getHealth()
              << " | 永久卡组: " << permanentDeckSize
              << " | 抽牌堆: " << battle.getDrawPileSize()
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

BattleOutcome runBattle(GameSession& session, Enemy enemy, std::uint32_t seed) {
    Battle battle{session.getPlayer(), enemy, session.getDeck(), seed};
    std::cout << "\n=== 遭遇：" << enemy.getName() << " ===\n";

    while (!battle.isOver()) {
        battle.startPlayerTurn();
        bool playerEndedTurn = false;

        while (!battle.isOver() && !playerEndedTurn) {
            printBattleState(battle, session.getPlayer(), enemy, session.getDeckSize());
            std::cout << "输入手牌编号出牌，输入 0 结束回合： ";

            int choice = 0;
            if (!(std::cin >> choice)) return BattleOutcome::InputClosed;
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
            std::cout << "\n[敌人回合] " << enemy.getName() << " 攻击，造成 "
                      << enemy.getAttackDamage() << " 点伤害。\n";
            battle.endPlayerTurn();
        }
    }

    if (session.getPlayer().isDead()) {
        std::cout << "\n" << session.getPlayer().getName() << " 被击败了。\n";
        return BattleOutcome::Defeat;
    }

    std::cout << "\n" << enemy.getName() << " 被击败，战斗胜利！\n";
    return BattleOutcome::Victory;
}

bool chooseReward(GameSession& session) {
    const std::vector<Card> rewards = session.createRewardOptions();
    std::cout << "\n=== 战斗奖励：选择 1 张卡加入永久卡组 ===\n";
    for (std::size_t index = 0; index < rewards.size(); ++index) {
        const Card& card = rewards[index];
        std::cout << index + 1 << ". " << card.getName()
                  << "（" << getCardTypeName(card.getType())
                  << "，能量 " << card.getEnergyCost()
                  << "，数值 " << card.getValue() << "）\n";
    }

    while (true) {
        std::cout << "输入奖励编号： ";
        int choice = 0;
        if (!(std::cin >> choice)) return false;
        if (choice < 1 || static_cast<std::size_t>(choice) > rewards.size()) {
            std::cout << "无效的奖励编号。\n";
            continue;
        }

        const Card chosenCard = rewards[static_cast<std::size_t>(choice - 1)];
        session.addCard(chosenCard);
        std::cout << "获得 " << chosenCard.getName()
                  << "！永久卡组现在有 " << session.getDeckSize() << " 张卡。\n";
        return true;
    }
}

} // namespace

int main() {
    GameSession session{"勇者", 80, createStarterDeck(), 1337};
    std::cout << "=== C++ Card Roguelike：两场战斗演示 ===\n";

    const BattleOutcome firstBattle = runBattle(session, Enemy{"雾壳虫", 30, 6}, 42);
    if (firstBattle != BattleOutcome::Victory || !chooseReward(session)) return 0;

    std::cout << "\n带着 " << session.getDeckSize() << " 张卡进入下一场战斗。\n";
    runBattle(session, Enemy{"锈甲卫", 42, 8}, 2026);
    return 0;
}
