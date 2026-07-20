# C++ Card Roguelike

一个使用 C++ 制作的卡牌 Roguelike 学习项目。当前阶段为命令行战斗原型；后续会接入 Raylib 图形界面。

## 当前进度

- [x] Day 1：CMake 工程可成功构建并运行
- [x] Day 2：建立 `Player`、`Enemy` 基础类，并完成两回合命令行战斗
- [x] Day 3：建立卡牌模型，并实现能量、格挡与卡牌效果结算

## 当前可运行内容

- 玩家每回合恢复 3 点能量；格挡会优先抵消敌人的伤害，并在下一玩家回合清零。
- `打击`（攻击，1 能量，6 伤害）实际对敌人造成伤害；`防御`（防御，1 能量，5 格挡）实际提供格挡。
- 卡牌类型通过 `enum class CardType` 表示，`Card::play()` 使用类型结算效果。

## 项目结构

```text
include/game/  # 类的声明（.hpp）
src/game/      # 类的实现（.cpp）
src/main.cpp   # 命令行演示与游戏流程入口
assets/        # 后续存放图片、字体等资源
data/          # 后续存放卡牌、敌人等数据
tests/         # 后续存放自动化测试
```

## 构建（Windows / MinGW）

```powershell
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
chcp 65001
.\build\card_roguelike.exe
```

`chcp 65001` 用于让 Windows 终端正确显示 UTF-8 中文。

## 下一步

加入抽牌堆、手牌与弃牌堆，让玩家每回合从真实卡组中抽牌和出牌。
