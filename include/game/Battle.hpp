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
    bool playCard(std::size_t handIndex);
    void endPlayerTurn();

    bool isOver() const;
    const std::vector<Card>& getHand() const;
    std::size_t getDrawPileSize() const;
    std::size_t getDiscardPileSize() const;

private:
    void drawCards(std::size_t count);
    void shuffleDiscardIntoDrawPile();
    void discardHand();

    Player& player_;
    Enemy& enemy_;
    std::vector<Card> drawPile_;
    std::vector<Card> hand_;
    std::vector<Card> discardPile_;
    std::mt19937 randomEngine_;
};
