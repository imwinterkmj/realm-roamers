#pragma once

#include "game/Card.hpp"

#include <cstddef>
#include <cstdint>
#include <random>
#include <vector>

class Enemy;
class Player;

class Battle {
public:
    Battle(Player& player, Enemy& enemy, std::vector<Card> deck, std::uint32_t seed = 42);

    void startPlayerTurn();
    // 尝试打出指定手牌；下标无效或能量不足时返回 false。
    bool playCard(std::size_t handIndex);
    void endPlayerTurn();

    bool isOver() const;
    const std::vector<Card>& getHand() const;
    // 返回只读卡堆，避免界面代码意外修改战斗状态。
    const std::vector<Card>& getDrawPile() const;
    const std::vector<Card>& getDiscardPile() const;
    std::size_t getDrawPileSize() const;
    std::size_t getDiscardPileSize() const;

private:
    // 从抽牌堆移动卡牌到手牌；抽牌堆为空时会回收弃牌堆。
    void drawCards(std::size_t count);
    // 将弃牌堆移动回抽牌堆并使用当前随机数引擎洗牌。
    void shuffleDiscardIntoDrawPile();
    void discardHand();

    Player& player_;
    Enemy& enemy_;
    std::vector<Card> drawPile_;
    std::vector<Card> hand_;
    std::vector<Card> discardPile_;
    std::mt19937 randomEngine_;
};
