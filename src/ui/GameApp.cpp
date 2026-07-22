#include "ui/GameApp.hpp"

#include <raylib.h>
#include "game/Card.hpp"
#include "game/Battle.hpp"
#include "game/Enemy.hpp"
#include "game/Player.hpp"

#include <algorithm>
#include <string>

GameApp::GameApp(Player& player, Enemy& enemy, Battle& battle)
    // GameApp 不拥有这些对象；它只读取 Battle 当前状态并负责绘制。
    : player_(player), enemy_(enemy), battle_(battle) {}

void GameApp::run() {
    // Day 3 的固定设计画布。后续若支持窗口缩放，drawBattleState() 已会读取实际宽度布局。
    constexpr int windowWidth = 1440;
    constexpr int windowHeight = 900;

    // 暂时使用 Windows 已安装的中文字体。发布前应将可分发的开源字体放入 assets/。
    constexpr const char* fontPath = "C:/Windows/Fonts/NotoSansSC-Regular.ttf";
    // LoadFontEx 只会生成这里列出的字形；新增界面中文时，必须把新字符也加到此字符串中，
    // 否则 Raylib 会用错误的占位字形显示它。
    constexpr const char* uiCharacters =
        "域上行者第回合玩家敌人生 命格挡能量意图攻击卡堆抽牌弃牌手牌训练木桩勇者打击防御费用伤害造成点获得"
        "结束提示点击以使用交互将在下一阶段开放不足0123456789";

    // 必须先创建窗口，再加载字体；字体加载需要图形上下文来生成字形纹理。
    InitWindow(windowWidth, windowHeight, "域上行者");
    SetTargetFPS(60);

    // 将 UTF-8 字符串拆成 Unicode 码点，再只加载本界面实际使用的中文字形。
    int glyphCount = 0;
    int* glyphs = LoadCodepoints(uiCharacters, &glyphCount);
    Font uiFont = LoadFontEx(fontPath, 48, glyphs, glyphCount);
    UnloadCodepoints(glyphs);

    while (!WindowShouldClose()) {
        BeginDrawing();
        // 全局深色背景；具体面板和卡牌会在 drawBattleState() 中叠加绘制。
        ClearBackground(Color{20, 24, 33, 255});
        drawBattleState(uiFont);
        EndDrawing();
    }

    // 与 LoadFontEx 对应，关闭窗口前释放字体纹理资源。
    UnloadFont(uiFont);
    CloseWindow();
}

void GameApp::drawBattleState(Font uiFont) const {
    // -------- 布局基础：根据当前窗口宽度计算横向可用空间 --------
    const int screenWidth = GetScreenWidth();
    constexpr int margin = 56;
    constexpr int panelGap = 48;
    const int contentWidth = screenWidth - margin * 2;
    const int statusPanelWidth = (contentWidth - panelGap) / 2;

    // -------- 视觉配色 --------
    // 面板与文字使用中性深色；玩家、敌人、生命、能量使用语义颜色，便于快速扫读。
    const Color panelColor{31, 41, 55, 255};
    const Color borderColor{71, 85, 105, 255};
    const Color primaryText{226, 232, 240, 255};
    const Color mutedText{148, 163, 184, 255};
    const Color playerColor{56, 189, 248, 255};
    const Color enemyColor{248, 113, 113, 255};
    const Color healthColor{74, 222, 128, 255};
    const Color energyColor{250, 204, 21, 255};
    const Color attackColor{117, 52, 68, 255};
    const Color blockColor{43, 83, 108, 255};
    const Color unavailableColor{55, 65, 81, 255};
    const Color buttonColor{59, 130, 246, 255};

    // -------- 文字绘制工具 --------
    // 所有中文都必须使用 uiFont 绘制，不能使用 Raylib 默认的 DrawText。
    const auto drawText = [uiFont](const char* text, int x, int y, int fontSize, Color color) {
        DrawTextEx(uiFont, text, Vector2{static_cast<float>(x), static_cast<float>(y)},
                   static_cast<float>(fontSize), 1.0F, color);
    };
    // 先测量文字宽度，再换算左边界，供卡名、费用和按钮文字水平居中使用。
    const auto drawCenteredText = [&drawText, uiFont](const char* text, int centerX, int y,
                                                       int fontSize, Color color) {
        const float textWidth = MeasureTextEx(uiFont, text, static_cast<float>(fontSize), 1.0F).x;
        drawText(text, centerX - static_cast<int>(textWidth / 2.0F), y, fontSize, color);
    };

    // -------- 顶栏：游戏标题与当前回合 --------
    drawText("域上行者", margin, 40, 48, primaryText);
    drawText(TextFormat("第 %i 回合", turnNumber_), screenWidth - 200, 50, 30, mutedText);
    DrawLine(margin, 110, screenWidth - margin, 110, borderColor);

    // -------- 左上：玩家状态面板 --------
    // 此区域直接读取 Player，不在 UI 中修改生命、格挡或能量。
    DrawRectangle(margin, 145, statusPanelWidth, 260, panelColor);
    DrawRectangleLinesEx(Rectangle{static_cast<float>(margin), 145.0F,
                                   static_cast<float>(statusPanelWidth), 260.0F},
                         2.0F, borderColor);
    drawText("玩家", 88, 178, 28, playerColor);
    drawText(player_.getName().c_str(), 88, 225, 42, primaryText);
    drawText(TextFormat("生命  %i", player_.getHealth()), 88, 300, 32, healthColor);
    drawText(TextFormat("格挡  %i", player_.getBlock()), 88, 350, 32, primaryText);
    drawText(TextFormat("能量  %i", player_.getEnergy()), 330, 350, 32, energyColor);

    // -------- 右上：敌人状态面板 --------
    // enemyPanelX 由左面板宽度和间距计算，窗口变宽时两块面板会一起扩展。
    const int enemyPanelX = margin + statusPanelWidth + panelGap;
    DrawRectangle(enemyPanelX, 145, statusPanelWidth, 260, panelColor);
    DrawRectangleLinesEx(Rectangle{static_cast<float>(enemyPanelX), 145.0F,
                                   static_cast<float>(statusPanelWidth), 260.0F},
                         2.0F, borderColor);
    drawText("敌人", enemyPanelX + 32, 178, 28, enemyColor);
    drawText(enemy_.getName().c_str(), enemyPanelX + 32, 225, 42, primaryText);
    drawText(TextFormat("生命  %i", enemy_.getHealth()), enemyPanelX + 32, 300, 32, healthColor);
    drawText(TextFormat("意图  攻击 %i", enemy_.getAttackDamage()), enemyPanelX + 32, 350, 32, enemyColor);

    // -------- 中部：手牌区域标题 --------
    drawText("手牌", margin, 445, 24, mutedText);

    // -------- 底部：抽牌堆、弃牌堆和结束回合按钮 --------
    // 抽牌堆固定在左、弃牌堆固定在右；结束回合按钮位于视觉中心。
    constexpr int pileWidth = 320;
    constexpr int endTurnButtonWidth = 240;
    constexpr int pileY = 790;
    constexpr int pileHeight = 70;
    // 复用同一套“标签 + 数量”的小面板样式，避免抽牌堆与弃牌堆的绘制代码重复。
    const auto drawPile = [&drawText, panelColor, borderColor, primaryText, mutedText, pileWidth](
                              const char* label, int count, int x) {
        DrawRectangle(x, pileY, pileWidth, pileHeight, panelColor);
        DrawRectangleLinesEx(Rectangle{static_cast<float>(x), static_cast<float>(pileY),
                                       static_cast<float>(pileWidth), static_cast<float>(pileHeight)},
                             2.0F, borderColor);
        drawText(label, x + 20, pileY + 24, 22, mutedText);
        drawText(TextFormat("%i", count), x + pileWidth - 52, pileY + 18, 34, primaryText);
    };

    drawPile("抽牌堆", static_cast<int>(battle_.getDrawPileSize()), margin);
    drawPile("弃牌堆", static_cast<int>(battle_.getDiscardPileSize()), screenWidth - margin - pileWidth);

    // Day 3 只绘制按钮外观；Day 4 才会检测点击并调用 Battle::endPlayerTurn()。
    const int endTurnButtonX = (screenWidth - endTurnButtonWidth) / 2;
    DrawRectangleRounded(Rectangle{static_cast<float>(endTurnButtonX), static_cast<float>(pileY),
                                   static_cast<float>(endTurnButtonWidth), static_cast<float>(pileHeight)},
                         0.12F, 8, buttonColor);
    drawCenteredText("结束回合", endTurnButtonX + endTurnButtonWidth / 2, pileY + 25, 28, RAYWHITE);

    // -------- 中下部：手牌卡片 --------
    // getHand() 返回只读引用，UI 仅展示卡牌，不在这里移除或打出卡牌。
    const std::vector<Card>& hand = battle_.getHand();
    constexpr int cardGap = 16;
    constexpr int maximumCardWidth = 230;
    constexpr int cardHeight = 210;
    const int cardCount = static_cast<int>(hand.size());
    // 单张卡最大 230 像素；手牌变多或窗口变窄时自动缩小，保证一排仍能放下。
    const int cardWidth = cardCount > 0
        ? std::min(maximumCardWidth, (contentWidth - cardGap * (cardCount - 1)) / cardCount)
        : maximumCardWidth;
    const int handWidth = cardCount * cardWidth + std::max(0, cardCount - 1) * cardGap;
    int cardX = (screenWidth - handWidth) / 2;
    constexpr int cardY = 485;

    for (const Card& card : hand) {
        // 费用：每张牌右上角的数字表示使用它要消耗多少能量。
        // Day 4 点击卡牌时，Battle::playCard() 会再次做同样的合法性检查。
        const bool canAfford = player_.getEnergy() >= card.getEnergyCost();
        // 类型决定卡片颜色和效果文本：攻击牌造成伤害，防御牌获得格挡。
        const bool isAttack = card.getType() == CardType::Attack;
        const Color cardColor = canAfford ? (isAttack ? attackColor : blockColor) : unavailableColor;
        const Color cardBorder = canAfford ? (isAttack ? enemyColor : playerColor) : borderColor;
        const char* cardType = isAttack ? "攻击" : "防御";
        const std::string effect = isAttack
            ? TextFormat("造成 %i 点伤害", card.getValue())
            : TextFormat("获得 %i 点格挡", card.getValue());

        // 1) 卡片底色和边框：能量不足时统一使用灰色，方便玩家识别不可用牌。
        DrawRectangleRounded(Rectangle{static_cast<float>(cardX), static_cast<float>(cardY),
                                       static_cast<float>(cardWidth), static_cast<float>(cardHeight)},
                             0.08F, 8, cardColor);
        DrawRectangleRoundedLinesEx(Rectangle{static_cast<float>(cardX), static_cast<float>(cardY),
                                              static_cast<float>(cardWidth), static_cast<float>(cardHeight)},
                                    0.08F, 8, 3.0F, cardBorder);
        // 2) 费用角标：黄色圆圈中的数值就是 energyCost，例如 1 点能量。
        DrawCircle(cardX + cardWidth - 34, cardY + 34, 21.0F, canAfford ? energyColor : mutedText);
        drawCenteredText(TextFormat("%i", card.getEnergyCost()), cardX + cardWidth - 34, cardY + 17, 26,
                         canAfford ? Color{20, 24, 33, 255} : primaryText);
        // 3) 卡片信息：类型、名称、分隔线与由 Card::getValue() 生成的效果说明。
        drawText(cardType, cardX + 20, cardY + 20, 20, primaryText);
        drawCenteredText(card.getName().c_str(), cardX + cardWidth / 2, cardY + 65, 34, RAYWHITE);
        DrawLine(cardX + 20, cardY + 112, cardX + cardWidth - 20, cardY + 112, borderColor);
        drawCenteredText(effect.c_str(), cardX + cardWidth / 2, cardY + 137, 22, primaryText);
        cardX += cardWidth + cardGap;
    }

    // 牌堆标题和分隔线放在最后绘制，保持它们位于同一区域的最上层。
    drawText("牌堆", margin, 735, 24, mutedText);
    DrawLine(margin, 770, screenWidth - margin, 770, borderColor);
}
