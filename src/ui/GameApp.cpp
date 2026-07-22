#include "ui/GameApp.hpp"

#include <raylib.h>
#include "game/Battle.hpp"
#include "game/Enemy.hpp"
#include "game/Player.hpp"

GameApp::GameApp(Player& player, Enemy& enemy, Battle& battle)
    : player_(player), enemy_(enemy), battle_(battle) {}

void GameApp::run() {
    constexpr int windowWidth = 1440;
    constexpr int windowHeight = 900;
    constexpr const char* fontPath = "C:/Windows/Fonts/NotoSansSC-Regular.ttf";
    constexpr const char* uiCharacters =
        "域上行者第回合玩家敌人生 命格挡能量意图攻击卡堆抽牌弃牌手牌训练木桩勇者0123456789";

    InitWindow(windowWidth, windowHeight, "域上行者");
    SetTargetFPS(60);

    int glyphCount = 0;
    int* glyphs = LoadCodepoints(uiCharacters, &glyphCount);
    Font uiFont = LoadFontEx(fontPath, 48, glyphs, glyphCount);
    UnloadCodepoints(glyphs);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(Color{20, 24, 33, 255});
        drawBattleState(uiFont);
        EndDrawing();
    }

    UnloadFont(uiFont);
    CloseWindow();
}

void GameApp::drawBattleState(Font uiFont) const {
    const Color panelColor{31, 41, 55, 255};
    const Color borderColor{71, 85, 105, 255};
    const Color primaryText{226, 232, 240, 255};
    const Color mutedText{148, 163, 184, 255};
    const Color playerColor{56, 189, 248, 255};
    const Color enemyColor{248, 113, 113, 255};
    const Color healthColor{74, 222, 128, 255};
    const Color energyColor{250, 204, 21, 255};

    const auto drawText = [uiFont](const char* text, int x, int y, int fontSize, Color color) {
        DrawTextEx(uiFont, text, Vector2{static_cast<float>(x), static_cast<float>(y)},
                   static_cast<float>(fontSize), 1.0F, color);
    };

    drawText("域上行者", 56, 40, 48, primaryText);
    drawText(TextFormat("第 %i 回合", turnNumber_), 1180, 50, 30, mutedText);
    DrawLine(56, 110, 1384, 110, borderColor);

    DrawRectangle(56, 145, 640, 310, panelColor);
    DrawRectangleLinesEx(Rectangle{56.0F, 145.0F, 640.0F, 310.0F}, 2.0F, borderColor);
    drawText("玩家", 88, 178, 28, playerColor);
    drawText(player_.getName().c_str(), 88, 225, 42, primaryText);
    drawText(TextFormat("生命  %i", player_.getHealth()), 88, 300, 32, healthColor);
    drawText(TextFormat("格挡  %i", player_.getBlock()), 88, 350, 32, primaryText);
    drawText(TextFormat("能量  %i", player_.getEnergy()), 88, 400, 32, energyColor);

    DrawRectangle(744, 145, 640, 310, panelColor);
    DrawRectangleLinesEx(Rectangle{744.0F, 145.0F, 640.0F, 310.0F}, 2.0F, borderColor);
    drawText("敌人", 776, 178, 28, enemyColor);
    drawText(enemy_.getName().c_str(), 776, 225, 42, primaryText);
    drawText(TextFormat("生命  %i", enemy_.getHealth()), 776, 300, 32, healthColor);
    drawText(TextFormat("意图  攻击 %i", enemy_.getAttackDamage()), 776, 360, 32, enemyColor);

    drawText("卡堆", 56, 510, 28, mutedText);
    DrawLine(56, 554, 1384, 554, borderColor);

    DrawRectangle(56, 590, 400, 210, panelColor);
    DrawRectangleLinesEx(Rectangle{56.0F, 590.0F, 400.0F, 210.0F}, 2.0F, borderColor);
    drawText("抽牌堆", 88, 628, 28, mutedText);
    drawText(TextFormat("%i", static_cast<int>(battle_.getDrawPileSize())), 88, 680, 60, primaryText);

    DrawRectangle(520, 590, 400, 210, panelColor);
    DrawRectangleLinesEx(Rectangle{520.0F, 590.0F, 400.0F, 210.0F}, 2.0F, borderColor);
    drawText("弃牌堆", 552, 628, 28, mutedText);
    drawText(TextFormat("%i", static_cast<int>(battle_.getDiscardPileSize())), 552, 680, 60, primaryText);

    DrawRectangle(984, 590, 400, 210, panelColor);
    DrawRectangleLinesEx(Rectangle{984.0F, 590.0F, 400.0F, 210.0F}, 2.0F, borderColor);
    drawText("手牌", 1016, 628, 28, mutedText);
    drawText(TextFormat("%i", static_cast<int>(battle_.getHand().size())), 1016, 680, 60, primaryText);
}
