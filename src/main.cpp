#include "game/Battle.hpp"
#include "game/Card.hpp"
#include "game/Enemy.hpp"
#include "game/Player.hpp"
#include "ui/GameApp.hpp"

#include <vector>

int main() {
    std::vector<Card> starterDeck;
    for (int index = 0; index < 5; ++index) {
        starterDeck.emplace_back("打击", CardType::Attack, 1, 6);
        starterDeck.emplace_back("防御", CardType::Block, 1, 5);
    }

    Player player{"勇者", 80};
    Enemy enemy{"训练木桩", 30, 6};
    Battle battle{player, enemy, starterDeck, 42};
    battle.startPlayerTurn();

    GameApp app{player, enemy, battle};
    app.run();
    return 0;
}
