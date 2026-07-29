#include "ui/GameApp.hpp"

#include <raylib.h>
#include "game/Card.hpp"
#include "game/Battle.hpp"
#include "game/Enemy.hpp"
#include "game/Player.hpp"

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

GameApp::GameApp() = default;
GameApp::~GameApp() = default;

void GameApp::run() {
    // Day 3 的固定设计画布。后续若支持窗口缩放，drawBattleState() 已会读取实际宽度布局。
    constexpr int windowWidth = 1440;
    constexpr int windowHeight = 900;

    // 暂时使用 Windows 已安装的中文字体。发布前应将可分发的开源字体放入 assets/。
    constexpr const char* fontPath = "C:/Windows/Fonts/NotoSansSC-Regular.ttf";
    // LoadFontEx 只会生成这里列出的字形；新增界面中文时，必须把新字符也加到此字符串中，
    // 否则 Raylib 会用错误的占位字形显示它。
    constexpr const char* uiCharacters =
        "域上行者第回合玩家敌人生 命格挡能量意图攻击卡堆抽牌弃牌手牌训练木桩勇者打击重击防御壁垒费用伤害造成点获得"
        "结束提示点击以使用交互将在下一阶段开放不足请选择或无法已出敌人行动战斗胜利失败，开始选择狌旋龟高低血攻"
        "返回菜单再来一场0123456789";

    // 必须先创建窗口，再加载字体；字体加载需要图形上下文来生成字形纹理。
    InitWindow(windowWidth, windowHeight, "域上行者");
    SetTargetFPS(60);

    // 将 UTF-8 字符串拆成 Unicode 码点，再只加载本界面实际使用的中文字形。
    int glyphCount = 0;
    int* glyphs = LoadCodepoints(uiCharacters, &glyphCount);
    Font uiFont = LoadFontEx(fontPath, 48, glyphs, glyphCount);
    UnloadCodepoints(glyphs);

    while (!WindowShouldClose()) {
        // 先响应本帧输入，再绘制，因此出牌或结束回合后的状态会立刻反映到画面。
        handleInput();
        BeginDrawing();
        // 全局深色背景；具体面板和卡牌会在 drawBattleState() 中叠加绘制。
        ClearBackground(Color{20, 24, 33, 255});
        switch (gameState_) {
        case GameState::MainMenu:
            drawMainMenu(uiFont);
            break;
        case GameState::Battle:
            drawBattleState(uiFont);
            break;
        case GameState::Victory:
        case GameState::Defeat:
            drawResultScreen(uiFont);
            break;
        }
        EndDrawing();
    }

    // 与 LoadFontEx 对应，关闭窗口前释放字体纹理资源。
    UnloadFont(uiFont);
    CloseWindow();
}

void GameApp::handleInput() {
    // 反馈显示约 2 秒；每帧递减，归零后不再绘制。
    feedbackTimer_ = std::max(0.0F, feedbackTimer_ - GetFrameTime());

    // Pressed 只在“按下的第一帧”返回 true，避免按住鼠标时连续触发多次出牌。
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        return;
    }

    if (gameState_ == GameState::MainMenu) {
        handleMainMenuInput();
        return;
    }
    if (gameState_ == GameState::Victory || gameState_ == GameState::Defeat) {
        handleResultInput();
        return;
    }

    // 战斗结束后不再允许改变 Battle，直接切换到专用结果界面。
    if (battle_->isOver()) {
        gameState_ = player_->isDead() ? GameState::Defeat : GameState::Victory;
        return;
    }

    // Raylib 返回的是窗口坐标；它与绘制、Rectangle 使用相同的坐标系。
    const Vector2 mousePosition = GetMousePosition();

    // 结束回合：Battle 会弃掉手牌、结算敌人攻击；若双方存活则立即开启下一玩家回合。
    if (CheckCollisionPointRec(mousePosition, getEndTurnButtonBounds())) {
        battle_->endPlayerTurn();
        if (battle_->isOver()) {
            gameState_ = player_->isDead() ? GameState::Defeat : GameState::Victory;
            return;
        }

        ++turnNumber_;
        battle_->startPlayerTurn();
        setFeedback("敌人行动结束", Color{148, 163, 184, 255});
        return;
    }

    // cardBounds 的第 i 个矩形和 hand 的第 i 张卡严格对应。
    // 这样命中测试得到的 index 可以直接传给 Battle::playCard(index)。
    const std::vector<Rectangle> cardBounds = getHandCardBounds();
    const std::vector<Card>& hand = battle_->getHand();
    for (std::size_t index = 0; index < cardBounds.size(); ++index) {
        if (!CheckCollisionPointRec(mousePosition, cardBounds[index])) {
            continue;
        }

        // playCard() 成功后会从手牌 vector 删除该元素，因此先复制名称用于后续反馈。
        const Card selectedCard = hand[index];
        if (!battle_->playCard(index)) {
            setFeedback("能量不足，无法使用", Color{250, 204, 21, 255});
            return;
        }

        if (battle_->isOver()) {
            gameState_ = GameState::Victory;
            return;
        }

        setFeedback(TextFormat("已使用 %s", selectedCard.getName().c_str()), Color{56, 189, 248, 255});
        return;
    }

    // 点击没有命中任何可交互区域时，给出提示而不改变战斗状态。
    setFeedback("请选择手牌或结束回合", Color{148, 163, 184, 255});
}

void GameApp::handleMainMenuInput() {
    const Vector2 mousePosition = GetMousePosition();
    const std::vector<Rectangle> enemyChoices = getEnemyChoiceBounds();

    // 两张选项卡分别启动不同数值定位的基础敌人，后续地图节点会复用 startBattle()。
    if (CheckCollisionPointRec(mousePosition, enemyChoices[0])) {
        startBattle("狌狌", 26, 8);
    } else if (CheckCollisionPointRec(mousePosition, enemyChoices[1])) {
        startBattle("旋龟", 48, 4);
    }
}

void GameApp::handleResultInput() {
    // MVP 阶段的结果界面只有一个入口：点击后返回菜单并选择下一场战斗。
    gameState_ = GameState::MainMenu;
}

void GameApp::startBattle(std::string enemyName, int enemyHealth, int enemyAttack) {
    std::vector<Card> starterDeck;
    for (int index = 0; index < 4; ++index) starterDeck.emplace_back("打击", CardType::Attack, 1, 6);
    for (int index = 0; index < 2; ++index) starterDeck.emplace_back("重击", CardType::Attack, 2, 12);
    for (int index = 0; index < 3; ++index) starterDeck.emplace_back("防御", CardType::Block, 1, 5);
    starterDeck.emplace_back("壁垒", CardType::Block, 2, 11);

    player_ = std::make_unique<Player>("勇者", 80);
    enemy_ = std::make_unique<Enemy>(std::move(enemyName), enemyHealth, enemyAttack);
    battle_ = std::make_unique<Battle>(*player_, *enemy_, std::move(starterDeck), 42);
    battle_->startPlayerTurn();

    turnNumber_ = 1;
    feedback_.clear();
    feedbackTimer_ = 0.0F;
    gameState_ = GameState::Battle;
}

std::vector<Rectangle> GameApp::getHandCardBounds() const {
    // 这里的尺寸必须与 drawBattleState() 绘制卡片时使用的尺寸一致。
    // 将计算提取为函数后，绘制与点击会天然保持同步。
    constexpr int margin = 56;
    constexpr int cardGap = 16;
    constexpr int maximumCardWidth = 230;
    constexpr int cardHeight = 210;
    constexpr int cardY = 485;

    const int screenWidth = GetScreenWidth();
    const int contentWidth = screenWidth - margin * 2;
    const int cardCount = static_cast<int>(battle_->getHand().size());
    // 牌数较少时使用理想宽度；牌数较多时缩小到一行仍能显示完整手牌。
    const int cardWidth = cardCount > 0
        ? std::min(maximumCardWidth, (contentWidth - cardGap * (cardCount - 1)) / cardCount)
        : maximumCardWidth;
    const int handWidth = cardCount * cardWidth + std::max(0, cardCount - 1) * cardGap;
    int cardX = (screenWidth - handWidth) / 2;

    // 按从左到右的顺序生成矩形，因此它们的索引就是 Battle 手牌索引。
    std::vector<Rectangle> cardBounds;
    cardBounds.reserve(static_cast<std::size_t>(cardCount));
    for (int index = 0; index < cardCount; ++index) {
        cardBounds.push_back(Rectangle{static_cast<float>(cardX), static_cast<float>(cardY),
                                       static_cast<float>(cardWidth), static_cast<float>(cardHeight)});
        cardX += cardWidth + cardGap;
    }
    return cardBounds;
}

Rectangle GameApp::getEndTurnButtonBounds() const {
    // 按钮固定在牌堆行的水平中心；绘制时也调用此函数，而不是重复写坐标。
    constexpr int endTurnButtonWidth = 240;
    constexpr int pileY = 790;
    constexpr int pileHeight = 70;
    const int buttonX = (GetScreenWidth() - endTurnButtonWidth) / 2;
    return Rectangle{static_cast<float>(buttonX), static_cast<float>(pileY),
                     static_cast<float>(endTurnButtonWidth), static_cast<float>(pileHeight)};
}

std::vector<Rectangle> GameApp::getEnemyChoiceBounds() const {
    constexpr int cardWidth = 400;
    constexpr int cardHeight = 280;
    constexpr int gap = 64;
    const int totalWidth = cardWidth * 2 + gap;
    const int firstX = (GetScreenWidth() - totalWidth) / 2;
    constexpr int cardY = 315;

    return {
        Rectangle{static_cast<float>(firstX), static_cast<float>(cardY),
                  static_cast<float>(cardWidth), static_cast<float>(cardHeight)},
        Rectangle{static_cast<float>(firstX + cardWidth + gap), static_cast<float>(cardY),
                  static_cast<float>(cardWidth), static_cast<float>(cardHeight)}
    };
}

void GameApp::setFeedback(std::string message, Color color) {
    // std::move 将传入字符串所有权转给成员变量，避免不必要的复制。
    feedback_ = std::move(message);
    feedbackColor_ = color;
    feedbackTimer_ = 2.0F;
}

void GameApp::drawMainMenu(Font uiFont) const {
    const Color primaryText{226, 232, 240, 255};
    const Color mutedText{148, 163, 184, 255};
    const Color borderColor{71, 85, 105, 255};
    const Color fastEnemyColor{117, 52, 68, 255};
    const Color toughEnemyColor{43, 83, 108, 255};

    const auto drawText = [uiFont](const char* text, int x, int y, int fontSize, Color color) {
        DrawTextEx(uiFont, text, Vector2{static_cast<float>(x), static_cast<float>(y)},
                   static_cast<float>(fontSize), 1.0F, color);
    };
    const auto drawCenteredText = [&drawText, uiFont](const char* text, int centerX, int y,
                                                       int fontSize, Color color) {
        const float textWidth = MeasureTextEx(uiFont, text, static_cast<float>(fontSize), 1.0F).x;
        drawText(text, centerX - static_cast<int>(textWidth / 2.0F), y, fontSize, color);
    };

    const int screenWidth = GetScreenWidth();
    drawCenteredText("域上行者", screenWidth / 2, 100, 58, primaryText);
    drawCenteredText("选择敌人开始战斗", screenWidth / 2, 190, 30, mutedText);

    const std::vector<Rectangle> enemyChoices = getEnemyChoiceBounds();
    const auto drawEnemyChoice = [&drawCenteredText, borderColor](Rectangle bounds, Color background,
                                                                    const char* name, const char* role,
                                                                    int health, int attack) {
        DrawRectangleRounded(bounds, 0.08F, 8, background);
        DrawRectangleRoundedLinesEx(bounds, 0.08F, 8, 3.0F, borderColor);
        const int centerX = static_cast<int>(bounds.x + bounds.width / 2.0F);
        drawCenteredText(name, centerX, static_cast<int>(bounds.y) + 42, 44, RAYWHITE);
        drawCenteredText(role, centerX, static_cast<int>(bounds.y) + 104, 26, RAYWHITE);
        drawCenteredText(TextFormat("生命 %i", health), centerX, static_cast<int>(bounds.y) + 162, 28, RAYWHITE);
        drawCenteredText(TextFormat("攻击 %i", attack), centerX, static_cast<int>(bounds.y) + 206, 28, RAYWHITE);
    };

    drawEnemyChoice(enemyChoices[0], fastEnemyColor, "狌狌", "高攻低血", 26, 8);
    drawEnemyChoice(enemyChoices[1], toughEnemyColor, "旋龟", "高血低攻", 48, 4);
    drawCenteredText("点击敌人开始战斗", screenWidth / 2, 660, 24, mutedText);
}

void GameApp::drawResultScreen(Font uiFont) const {
    const bool victory = gameState_ == GameState::Victory;
    const Color resultColor = victory ? Color{74, 222, 128, 255} : Color{248, 113, 113, 255};
    const Color primaryText{226, 232, 240, 255};

    const auto drawText = [uiFont](const char* text, int x, int y, int fontSize, Color color) {
        DrawTextEx(uiFont, text, Vector2{static_cast<float>(x), static_cast<float>(y)},
                   static_cast<float>(fontSize), 1.0F, color);
    };
    const auto drawCenteredText = [&drawText, uiFont](const char* text, int centerX, int y,
                                                       int fontSize, Color color) {
        const float textWidth = MeasureTextEx(uiFont, text, static_cast<float>(fontSize), 1.0F).x;
        drawText(text, centerX - static_cast<int>(textWidth / 2.0F), y, fontSize, color);
    };

    const int centerX = GetScreenWidth() / 2;
    drawCenteredText(victory ? "战斗胜利" : "战斗失败", centerX, 280, 56, resultColor);
    drawCenteredText("点击返回菜单", centerX, 390, 28, primaryText);
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
    drawText(player_->getName().c_str(), 88, 225, 42, primaryText);
    drawText(TextFormat("生命  %i", player_->getHealth()), 88, 300, 32, healthColor);
    drawText(TextFormat("格挡  %i", player_->getBlock()), 88, 350, 32, primaryText);
    drawText(TextFormat("能量  %i", player_->getEnergy()), 330, 350, 32, energyColor);

    // -------- 右上：敌人状态面板 --------
    // enemyPanelX 由左面板宽度和间距计算，窗口变宽时两块面板会一起扩展。
    const int enemyPanelX = margin + statusPanelWidth + panelGap;
    DrawRectangle(enemyPanelX, 145, statusPanelWidth, 260, panelColor);
    DrawRectangleLinesEx(Rectangle{static_cast<float>(enemyPanelX), 145.0F,
                                   static_cast<float>(statusPanelWidth), 260.0F},
                         2.0F, borderColor);
    drawText("敌人", enemyPanelX + 32, 178, 28, enemyColor);
    drawText(enemy_->getName().c_str(), enemyPanelX + 32, 225, 42, primaryText);
    drawText(TextFormat("生命  %i", enemy_->getHealth()), enemyPanelX + 32, 300, 32, healthColor);
    drawText(TextFormat("意图  攻击 %i", enemy_->getAttackDamage()), enemyPanelX + 32, 350, 32, enemyColor);

    // -------- 中部：手牌区域标题 --------
    drawText("手牌", margin, 445, 24, mutedText);
    if (feedbackTimer_ > 0.0F) {
        drawText(feedback_.c_str(), margin + 80, 447, 22, feedbackColor_);
    }

    // -------- 底部：抽牌堆、弃牌堆和结束回合按钮 --------
    // 抽牌堆固定在左、弃牌堆固定在右；结束回合按钮位于视觉中心。
    constexpr int pileWidth = 320;
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

    drawPile("抽牌堆", static_cast<int>(battle_->getDrawPileSize()), margin);
    drawPile("弃牌堆", static_cast<int>(battle_->getDiscardPileSize()), screenWidth - margin - pileWidth);

    // 点击区域与 handleInput() 共用 getEndTurnButtonBounds()，防止按钮外观和判定位置不同步。
    const Rectangle endTurnButtonBounds = getEndTurnButtonBounds();
    const bool battleOver = battle_->isOver();
    DrawRectangleRounded(endTurnButtonBounds, 0.12F, 8, battleOver ? unavailableColor : buttonColor);
    drawCenteredText(battleOver ? "战斗结束" : "结束回合",
                     static_cast<int>(endTurnButtonBounds.x + endTurnButtonBounds.width / 2.0F),
                     pileY + 25, 28, RAYWHITE);

    // -------- 中下部：手牌卡片 --------
    // getHand() 返回只读引用，UI 仅展示卡牌，不在这里移除或打出卡牌。
    const std::vector<Card>& hand = battle_->getHand();
    // getHandCardBounds() 同时供绘制和点击检测使用，保证两者始终对齐。
    const std::vector<Rectangle> cardBounds = getHandCardBounds();

    for (std::size_t index = 0; index < hand.size(); ++index) {
        const Card& card = hand[index];
        const Rectangle cardRect = cardBounds[index];
        const int cardX = static_cast<int>(cardRect.x);
        const int cardY = static_cast<int>(cardRect.y);
        const int cardWidth = static_cast<int>(cardRect.width);
        // 费用：每张牌右上角的数字表示使用它要消耗多少能量。
        // Day 4 点击卡牌时，Battle::playCard() 会再次做同样的合法性检查。
        const bool canAfford = player_->getEnergy() >= card.getEnergyCost();
        // 类型决定卡片颜色和效果文本：攻击牌造成伤害，防御牌获得格挡。
        const bool isAttack = card.getType() == CardType::Attack;
        const Color cardColor = canAfford ? (isAttack ? attackColor : blockColor) : unavailableColor;
        const Color cardBorder = canAfford ? (isAttack ? enemyColor : playerColor) : borderColor;
        const char* cardType = isAttack ? "攻击" : "防御";
        const std::string effect = isAttack
            ? TextFormat("造成 %i 点伤害", card.getValue())
            : TextFormat("获得 %i 点格挡", card.getValue());

        // 1) 卡片底色和边框：能量不足时统一使用灰色，方便玩家识别不可用牌。
        DrawRectangleRounded(cardRect, 0.08F, 8, cardColor);
        DrawRectangleRoundedLinesEx(cardRect, 0.08F, 8, 3.0F, cardBorder);
        // 2) 费用角标：黄色圆圈中的数值就是 energyCost，例如 1 点能量。
        DrawCircle(cardX + cardWidth - 34, cardY + 34, 21.0F, canAfford ? energyColor : mutedText);
        drawCenteredText(TextFormat("%i", card.getEnergyCost()), cardX + cardWidth - 34, cardY + 17, 26,
                         canAfford ? Color{20, 24, 33, 255} : primaryText);
        // 3) 卡片信息：类型、名称、分隔线与由 Card::getValue() 生成的效果说明。
        drawText(cardType, cardX + 20, cardY + 20, 20, primaryText);
        drawCenteredText(card.getName().c_str(), cardX + cardWidth / 2, cardY + 65, 34, RAYWHITE);
        DrawLine(cardX + 20, cardY + 112, cardX + cardWidth - 20, cardY + 112, borderColor);
        drawCenteredText(effect.c_str(), cardX + cardWidth / 2, cardY + 137, 22, primaryText);
    }

    // 牌堆标题和分隔线放在最后绘制，保持它们位于同一区域的最上层。
    drawText("牌堆", margin, 735, 24, mutedText);
    DrawLine(margin, 770, screenWidth - margin, 770, borderColor);
}
