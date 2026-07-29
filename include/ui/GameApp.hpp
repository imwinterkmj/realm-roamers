#pragma once

#include <raylib.h>

#include <memory>
#include <string>
#include <vector>

class Battle;
class Card;
class Enemy;
class Player;

// Owns the graphical application loop and translates mouse input into battle actions.
class GameApp {
public:
    GameApp();
    ~GameApp();

    void run();

private:
    enum class GameState {
        MainMenu,
        Map,
        Battle,
        Rest,
        Victory,
        Defeat
    };

    // 将当前帧的鼠标点击转换为出牌、结束回合或界面反馈。
    void handleInput();
    void handleMainMenuInput();
    void handleMapInput();
    void handleRestInput();
    void handleResultInput();
    // 只读渲染当前 Battle 状态。
    void drawBattleState(Font uiFont) const;
    void drawMainMenu(Font uiFont) const;
    void drawMap(Font uiFont) const;
    void drawRestScreen(Font uiFont) const;
    void drawResultScreen(Font uiFont) const;
    // 返回与手牌顺序完全一致的矩形；index i 对应 battle_.getHand()[i]。
    std::vector<Rectangle> getHandCardBounds() const;
    // 返回结束回合按钮的可点击区域，与实际绘制位置共用。
    Rectangle getEndTurnButtonBounds() const;
    std::vector<Rectangle> getMainMenuButtonBounds() const;
    std::vector<Rectangle> getMapNodeBounds() const;
    Rectangle getRestButtonBounds() const;
    Rectangle getResultButtonBounds() const;
    // 保存一条临时反馈文字并重置其显示倒计时。
    void setFeedback(std::string message, Color color);
    void startNewRun();
    void startBattle(std::string enemyName, int enemyHealth, int enemyAttack);
    void finishBattle();

    GameState gameState_ = GameState::MainMenu;
    std::unique_ptr<Player> player_;
    std::unique_ptr<Enemy> enemy_;
    std::unique_ptr<Battle> battle_;
    std::vector<Card> deck_;
    int mapNodeIndex_ = 0;
    int turnNumber_ = 1;
    std::string feedback_;
    Color feedbackColor_ = RAYWHITE;
    float feedbackTimer_ = 0.0F;
    bool shouldExit_ = false;
};
