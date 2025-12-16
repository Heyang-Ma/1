#pragma once
#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Unit.h"
#include "GameMap.h"
#include "FSM_AI.h"
#include "BehaviorTree_AI.h"

using namespace std;

// AI类型枚举 - 用于切换比较
enum class AIType {
    FSM,            // 有限状态机
    BehaviorTree    // 行为树
};

// 游戏状态枚举
enum class GameState {
    PlayerTurn,     // 玩家回合
    AITurn,         // AI回合
    GameOver        // 游戏结束
};

// 玩家操作状态
enum class PlayerAction {
    Waiting,        // 等待选择
    SelectingMove,  // 选择移动位置
    SelectingAttack // 选择攻击目标
};

// 游戏主控制类
class Game {
public:
    Game();
    ~Game();

    // 初始化游戏
    void init();

    // 游戏主循环相关
    void handleEvents(sf::RenderWindow& window);
    void update(float dt);
    void render(sf::RenderWindow& window);

    // 游戏是否结束
    bool isGameOver();

    // 切换AI类型（用于比较测试）
    void setAIType(AIType type);
    AIType getAIType();

private:
    // 游戏组件
    GameMap gameMap;
    vector<Unit*> units;
    FSM_AI fsmAI;
    BehaviorTree_AI btAI;

    // 游戏状态
    GameState gameState;
    PlayerAction playerAction;
    AIType currentAIType;

    // 当前选中的单位索引
    int currentUnitIndex;
    int selectedUnitIndex;

    // 图形资源
    sf::Font font;
    sf::Texture groundTexture;
    sf::Texture warriorTexture;
    sf::Texture archerTexture;
    sf::Texture highlightTexture;

    // 消息显示
    string statusMessage;
    float messageTimer;

    // AI回合延迟
    float aiTurnDelay;
    float aiTurnTimer;

    // 初始化函数
    void setupUnits();
    void loadResources();

    // 游戏逻辑
    void nextTurn();
    void executeAITurn();
    void checkGameOver();

    // 玩家输入处理
    void handleMouseClick(int screenX, int screenY);
    void selectUnit(int unitIndex);
    void moveSelectedUnit(int gridX, int gridY);
    void attackWithSelectedUnit(int gridX, int gridY);

    // 辅助函数
    int getUnitIndexAt(int gridX, int gridY);
    int getCurrentPlayerUnit();
    bool isValidMoveTarget(int gridX, int gridY);
    bool isValidAttackTarget(int gridX, int gridY);

    // 渲染函数
    void drawMap(sf::RenderWindow& window);
    void drawUnits(sf::RenderWindow& window);
    void drawUI(sf::RenderWindow& window);
    void drawHighlights(sf::RenderWindow& window);

    // 高亮显示的格子
    vector<pair<int, int>> highlightedTiles;
    sf::Color highlightColor;

    void updateHighlights();
    void clearHighlights();
};

#endif // GAME_H
