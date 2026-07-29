#include "game/GameData.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <utility>

namespace {

std::string trim(std::string value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return {};
    }

    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

std::vector<std::string> splitCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream stream(line);
    std::string field;
    while (std::getline(stream, field, ',')) {
        fields.push_back(trim(std::move(field)));
    }
    return fields;
}

int parsePositiveInt(const std::string& value, std::string_view fieldName, std::string_view fileName, int lineNumber) {
    try {
        const int parsed = std::stoi(value);
        if (parsed <= 0) {
            throw std::runtime_error("non-positive value");
        }
        return parsed;
    } catch (const std::exception&) {
        throw std::runtime_error(std::string(fileName) + " 第 " + std::to_string(lineNumber) +
                                 " 行的 " + std::string(fieldName) + " 必须是正整数。");
    }
}

std::ifstream openDataFile(const char* fileName) {
    std::ifstream file(std::string("data/") + fileName);
    if (file) {
        return file;
    }

    // 从 build/ 目录双击启动时，构建脚本复制的 data/ 位于当前目录。
    file.open(fileName);
    if (file) {
        return file;
    }

    throw std::runtime_error(std::string("无法读取数据文件: data/") + fileName);
}

CardType parseCardType(const std::string& value, int lineNumber) {
    if (value == "attack") {
        return CardType::Attack;
    }
    if (value == "block") {
        return CardType::Block;
    }
    throw std::runtime_error("cards.csv 第 " + std::to_string(lineNumber) +
                             " 行的 type 必须是 attack 或 block。");
}

} // namespace

GameData GameData::load() {
    GameData data;

    {
        std::ifstream file = openDataFile("cards.csv");
        std::string line;
        std::getline(file, line); // 跳过表头。
        int lineNumber = 1;
        while (std::getline(file, line)) {
            ++lineNumber;
            if (trim(line).empty() || line.starts_with('#')) {
                continue;
            }

            const std::vector<std::string> fields = splitCsvLine(line);
            if (fields.size() != 6) {
                throw std::runtime_error("cards.csv 第 " + std::to_string(lineNumber) + " 行应有 6 列。");
            }

            data.starterDeck_.push_back(CardDefinition{
                fields[0], fields[1], parseCardType(fields[2], lineNumber),
                parsePositiveInt(fields[3], "cost", "cards.csv", lineNumber),
                parsePositiveInt(fields[4], "value", "cards.csv", lineNumber),
                parsePositiveInt(fields[5], "copies", "cards.csv", lineNumber)
            });
        }
    }

    {
        std::ifstream file = openDataFile("enemies.csv");
        std::string line;
        std::getline(file, line); // 跳过表头。
        int lineNumber = 1;
        while (std::getline(file, line)) {
            ++lineNumber;
            if (trim(line).empty() || line.starts_with('#')) {
                continue;
            }

            const std::vector<std::string> fields = splitCsvLine(line);
            if (fields.size() != 4) {
                throw std::runtime_error("enemies.csv 第 " + std::to_string(lineNumber) + " 行应有 4 列。");
            }

            data.enemies_.push_back(EnemyDefinition{
                fields[0], fields[1],
                parsePositiveInt(fields[2], "max_health", "enemies.csv", lineNumber),
                parsePositiveInt(fields[3], "attack", "enemies.csv", lineNumber)
            });
        }
    }

    if (data.starterDeck_.empty() || data.enemies_.empty()) {
        throw std::runtime_error("卡牌或敌人数据不能为空。");
    }
    return data;
}

const std::vector<CardDefinition>& GameData::getStarterDeck() const {
    return starterDeck_;
}

const EnemyDefinition& GameData::getEnemy(const std::string& id) const {
    const auto found = std::find_if(enemies_.begin(), enemies_.end(), [&id](const EnemyDefinition& enemy) {
        return enemy.id == id;
    });
    if (found == enemies_.end()) {
        throw std::runtime_error("未找到敌人配置: " + id);
    }
    return *found;
}
