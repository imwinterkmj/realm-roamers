#pragma once

#include <raylib.h>

#include <string>
#include <vector>

class Battle;
class Enemy;
class Player;

// Owns the graphical application loop and translates mouse input into battle actions.
class GameApp {
public:
    GameApp(Player& player, Enemy& enemy, Battle& battle);

    void run();

private:
    // 将当前帧的鼠标点击转换为出牌、结束回合或界面反馈。
    void handleInput();
    // 只读渲染当前 Battle 状态。
    void drawBattleState(Font uiFont) const;
    // 返回与手牌顺序完全一致的矩形；index i 对应 battle_.getHand()[i]。
    std::vector<Rectangle> getHandCardBounds() const;
    // 返回结束回合按钮的可点击区域，与实际绘制位置共用。
    Rectangle getEndTurnButtonBounds() const;
    // 保存一条临时反馈文字并重置其显示倒计时。
    void setFeedback(std::string message, Color color);

    Player& player_;
    Enemy& enemy_;
    Battle& battle_;
    int turnNumber_ = 1;
    std::string feedback_;
    Color feedbackColor_ = RAYWHITE;
    float feedbackTimer_ = 0.0F;
};
