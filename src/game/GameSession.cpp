#include "game/GameSession.hpp"

#include <algorithm>
#include <utility>

GameSession::GameSession(std::string playerName, int maxHealth, std::vector<Card> startingDeck,
                         std::uint32_t seed)
    : player_(std::move(playerName), maxHealth),
      deck_(std::move(startingDeck)),
      randomEngine_(seed) {
}

Player& GameSession::getPlayer() {
    return player_;
}

const std::vector<Card>& GameSession::getDeck() const {
    return deck_;
}

std::size_t GameSession::getDeckSize() const {
    return deck_.size();
}

void GameSession::addCard(Card card) {
    deck_.push_back(std::move(card));
}

std::vector<Card> GameSession::createRewardOptions() {
    std::vector<Card> rewardPool{
        {"重击", CardType::Attack, 2, 14},
        {"连刺", CardType::Attack, 1, 8},
        {"壁垒", CardType::Block, 1, 9},
        {"蓄力斩", CardType::Attack, 2, 18},
        {"稳固", CardType::Block, 1, 12}
    };

    std::shuffle(rewardPool.begin(), rewardPool.end(), randomEngine_);
    rewardPool.erase(rewardPool.begin() + 3, rewardPool.end());
    return rewardPool;
}
