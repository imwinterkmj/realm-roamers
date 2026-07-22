#pragma once

#include <raylib.h>

class Battle;
class Enemy;
class Player;

// Owns the graphical application loop. Day 4 will add input that changes
// battle state; Day 2 only observes that state.
class GameApp {
public:
    GameApp(Player& player, Enemy& enemy, Battle& battle);

    void run();

private:
    void drawBattleState(Font uiFont) const;

    Player& player_;
    Enemy& enemy_;
    Battle& battle_;
    int turnNumber_ = 1;
};
