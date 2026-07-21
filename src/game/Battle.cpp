#include "game/Battle.hpp"

#include "game/Enemy.hpp"
#include "game/Player.hpp"

#include <algorithm>
#include <iterator>
#include <utility>

Battle::Battle(Player& player, Enemy& enemy, std::vector<Card> deck, std::uint32_t seed)
    : player_(player),
      enemy_(enemy),
      drawPile_(std::move(deck)),
      randomEngine_(seed) {
    std::shuffle(drawPile_.begin(), drawPile_.end(), randomEngine_);
}

void Battle::startPlayerTurn() {
    player_.startTurn();
    drawCards(5);
}

bool Battle::playCard(std::size_t handIndex) {
    if (handIndex >= hand_.size()) {
        return false;
    }
    if (!hand_[handIndex].play(player_, enemy_)) {
        return false;
    }

    discardPile_.push_back(std::move(hand_[handIndex]));
    hand_.erase(hand_.begin() + handIndex);
    return true;
}

void Battle::endPlayerTurn() {
    discardHand();

    if (!isOver()) {
        player_.takeDamage(enemy_.getAttackDamage());
    }
}

bool Battle::isOver() const {
    return player_.isDead() || enemy_.isDead();
}

const std::vector<Card>& Battle::getHand() const {
    return hand_;
}

const std::vector<Card>& Battle::getDrawPile() const {
    return drawPile_;
}

const std::vector<Card>& Battle::getDiscardPile() const {
    return discardPile_;
}

std::size_t Battle::getDrawPileSize() const {
    return drawPile_.size();
}

std::size_t Battle::getDiscardPileSize() const {
    return discardPile_.size();
}

void Battle::drawCards(std::size_t count) {
    while (count-- > 0) {
        if (drawPile_.empty()) {
            shuffleDiscardIntoDrawPile();
            if (drawPile_.empty()) {
                return;
            }
        }

        hand_.push_back(std::move(drawPile_.back()));
        drawPile_.pop_back();
    }
}

void Battle::shuffleDiscardIntoDrawPile() {
    drawPile_.insert(
        drawPile_.end(),
        std::make_move_iterator(discardPile_.begin()),
        std::make_move_iterator(discardPile_.end())
    );
    discardPile_.clear();
    std::shuffle(drawPile_.begin(), drawPile_.end(), randomEngine_);
}

void Battle::discardHand() {
    discardPile_.insert(
        discardPile_.end(),
        std::make_move_iterator(hand_.begin()),
        std::make_move_iterator(hand_.end())
    );
    hand_.clear();
}
