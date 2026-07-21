#pragma once

#include "game/Card.hpp"
#include "game/Player.hpp"

#include <cstdint>
#include <random>
#include <string>
#include <vector>

class GameSession {
public:
    GameSession(std::string playerName, int maxHealth, std::vector<Card> startingDeck,
                std::uint32_t seed = 1337);

    Player& getPlayer();
    const std::vector<Card>& getDeck() const;
    std::size_t getDeckSize() const;
    void addCard(Card card);
    std::vector<Card> createRewardOptions();

private:
    Player player_;
    std::vector<Card> deck_;
    std::mt19937 randomEngine_;
};
