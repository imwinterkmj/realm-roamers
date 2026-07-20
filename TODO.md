# TODO

## 已完成（Day 2）

- [X] 创建 `Player` 和 `Enemy` 基础类。
- [X] 建立 `include/game/` 的头文件结构。
- [X] 为每个类写一个可运行的最小验证。
- [X] 完成两回合的命令行战斗：敌人被击败后不再反击。

## 下一步（Day 3：卡牌数据模型）

目标：在不改变现有战斗逻辑的前提下，创建能表示卡牌与小型卡组的数据结构。

- [x] 使用 `enum class` 表示攻击牌与防御牌。
- [x] 使用 `std::vector` 与范围 `for` 循环保存、遍历卡组。
- [x] 创建 `Card` 类（名称、类型、能量消耗、数值）。
- [x] 创建两张示例卡：`打击`（1 能量、6 伤害）与 `防御`（1 能量、5 格挡）。
- [x] 使用 `std::vector<Card>` 输出卡组中每张卡的信息。
- [x] 更新 CMake 并构建运行。
- [ ] 提交 Git。

## 开始 Day 3 前先复习

- `.hpp` 负责声明，`.cpp` 负责实现。
- 修改 `.cpp` 后执行 Build；修改 `CMakeLists.txt` 后先 Configure 再 Build。
- 提交前依次执行：`git status`、`git add .`、`git commit -m "..."`、`git push`。
