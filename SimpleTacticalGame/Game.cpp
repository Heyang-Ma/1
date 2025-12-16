#include "Game.h"
#include <iostream>
#include <sstream>

using namespace std;

Game::Game() {
    gameState = GameState::PlayerTurn;
    playerAction = PlayerAction::Waiting;
    currentAIType = AIType::FSM;  // 默认使用FSM
    currentUnitIndex = 0;
    selectedUnitIndex = -1;
    statusMessage = "游戏开始！玩家回合";
    messageTimer = 0;
    aiTurnDelay = 1.0f;  // AI回合延迟1秒
    aiTurnTimer = 0;
    highlightColor = sf::Color(0, 255, 0, 100);  // 半透明绿色
}

Game::~Game() {
    for (Unit* unit : units) {
        delete unit;
    }
    units.clear();
}

void Game::init() {
    loadResources();
    setupUnits();
    gameMap.updateOccupancy(units);
}

// 加载图形资源
void Game::loadResources() {
    // 加载字体
    if (!font.openFromFile("Assets/DefaultAriel.ttf")) {
        cout << "警告: 无法加载字体文件" << endl;
    }

    // 加载纹理（使用现有的资源）
    if (!groundTexture.loadFromFile("Assets/Ground.png")) {
        cout << "警告: 无法加载地面纹理" << endl;
    }
    if (!warriorTexture.loadFromFile("Assets/Fighter_v3.png")) {
        cout << "警告: 无法加载战士纹理" << endl;
    }
    if (!archerTexture.loadFromFile("Assets/Archer_v3.png")) {
        cout << "警告: 无法加载弓箭手纹理" << endl;
    }
    if (!highlightTexture.loadFromFile("Assets/Highlight.png")) {
        cout << "警告: 无法加载高亮纹理" << endl;
    }
}

// 设置初始单位
void Game::setupUnits() {
    // 玩家单位（蓝色，左侧）
    units.push_back(new Unit(UnitType::Warrior, Team::Player, 1, 3));
    units.push_back(new Unit(UnitType::Archer, Team::Player, 0, 5));

    // AI单位（红色，右侧）
    units.push_back(new Unit(UnitType::Warrior, Team::Enemy, 6, 4));
    units.push_back(new Unit(UnitType::Archer, Team::Enemy, 7, 2));

    cout << "\n=== 单位初始化完成 ===" << endl;
    cout << "玩家: 战士(1,3), 弓箭手(0,5)" << endl;
    cout << "敌方: 战士(6,4), 弓箭手(7,2)" << endl;
    cout << "当前AI类型: " << (currentAIType == AIType::FSM ? "FSM" : "行为树") << endl;
    cout << "按 Tab 键切换AI类型\n" << endl;
}

// 设置AI类型
void Game::setAIType(AIType type) {
    currentAIType = type;
    cout << "AI类型已切换为: " << (type == AIType::FSM ? "FSM(有限状态机)" : "行为树") << endl;
}

AIType Game::getAIType() {
    return currentAIType;
}

// 事件处理
void Game::handleEvents(sf::RenderWindow& window) {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            // ESC退出
            if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                window.close();
            }
            // Tab切换AI类型
            else if (keyPressed->scancode == sf::Keyboard::Scancode::Tab) {
                if (currentAIType == AIType::FSM) {
                    setAIType(AIType::BehaviorTree);
                    statusMessage = "切换到: 行为树 AI";
                } else {
                    setAIType(AIType::FSM);
                    statusMessage = "切换到: 有限状态机 AI";
                }
            }
            // 空格跳过回合
            else if (keyPressed->scancode == sf::Keyboard::Scancode::Space) {
                if (gameState == GameState::PlayerTurn) {
                    nextTurn();
                }
            }
            // R重置游戏
            else if (keyPressed->scancode == sf::Keyboard::Scancode::R) {
                // 清理旧单位
                for (Unit* unit : units) {
                    delete unit;
                }
                units.clear();
                setupUnits();
                gameMap.updateOccupancy(units);
                gameState = GameState::PlayerTurn;
                currentUnitIndex = 0;
                statusMessage = "游戏重置！";
            }
        }
        else if (const auto* mouseButton = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseButton->button == sf::Mouse::Button::Left) {
                handleMouseClick(mouseButton->position.x, mouseButton->position.y);
            }
            else if (mouseButton->button == sf::Mouse::Button::Right) {
                // 右键取消选择
                playerAction = PlayerAction::Waiting;
                selectedUnitIndex = -1;
                clearHighlights();
                statusMessage = "已取消选择";
            }
        }
    }
}

// 鼠标点击处理
void Game::handleMouseClick(int screenX, int screenY) {
    if (gameState != GameState::PlayerTurn) return;

    int gridX = gameMap.screenToGridX(screenX);
    int gridY = gameMap.screenToGridY(screenY);

    if (!gameMap.isValidPosition(gridX, gridY)) return;

    cout << "点击位置: (" << gridX << ", " << gridY << ")" << endl;

    switch (playerAction) {
        case PlayerAction::Waiting: {
            // 选择玩家单位
            int unitIdx = getUnitIndexAt(gridX, gridY);
            if (unitIdx >= 0 && units[unitIdx]->team == Team::Player &&
                units[unitIdx]->isAlive && !units[unitIdx]->hasActed) {
                selectUnit(unitIdx);
            }
            break;
        }
        case PlayerAction::SelectingMove: {
            if (isValidMoveTarget(gridX, gridY)) {
                moveSelectedUnit(gridX, gridY);
            } else {
                // 点击其他地方取消
                playerAction = PlayerAction::Waiting;
                clearHighlights();
            }
            break;
        }
        case PlayerAction::SelectingAttack: {
            if (isValidAttackTarget(gridX, gridY)) {
                attackWithSelectedUnit(gridX, gridY);
            } else {
                playerAction = PlayerAction::Waiting;
                clearHighlights();
            }
            break;
        }
    }
}

// 选择单位
void Game::selectUnit(int unitIndex) {
    selectedUnitIndex = unitIndex;
    Unit* unit = units[unitIndex];

    cout << "选中: " << unit->name << endl;
    statusMessage = "选中: " + unit->name + " (左键移动/攻击，右键取消)";

    // 更新高亮显示
    updateHighlights();
    playerAction = PlayerAction::SelectingMove;
}

// 移动选中的单位
void Game::moveSelectedUnit(int gridX, int gridY) {
    if (selectedUnitIndex < 0) return;

    Unit* unit = units[selectedUnitIndex];
    unit->move(gridX, gridY);
    gameMap.updateOccupancy(units);

    // 移动后切换到攻击选择
    updateHighlights();

    // 检查是否有可攻击的目标
    bool hasTarget = false;
    for (Unit* target : units) {
        if (target->team != unit->team && target->isAlive && unit->canAttack(target)) {
            hasTarget = true;
            break;
        }
    }

    if (hasTarget) {
        playerAction = PlayerAction::SelectingAttack;
        highlightColor = sf::Color(255, 0, 0, 100);  // 红色表示攻击
        updateHighlights();
        statusMessage = "选择攻击目标 (右键跳过)";
    } else {
        // 没有攻击目标，结束回合
        unit->hasActed = true;
        playerAction = PlayerAction::Waiting;
        selectedUnitIndex = -1;
        clearHighlights();
        nextTurn();
    }
}

// 攻击
void Game::attackWithSelectedUnit(int gridX, int gridY) {
    if (selectedUnitIndex < 0) return;

    Unit* unit = units[selectedUnitIndex];
    int targetIdx = getUnitIndexAt(gridX, gridY);

    if (targetIdx >= 0) {
        unit->attack(units[targetIdx]);
        gameMap.updateOccupancy(units);
    }

    unit->hasActed = true;
    playerAction = PlayerAction::Waiting;
    selectedUnitIndex = -1;
    clearHighlights();
    highlightColor = sf::Color(0, 255, 0, 100);
    nextTurn();
}

// 更新高亮显示
void Game::updateHighlights() {
    clearHighlights();

    if (selectedUnitIndex < 0) return;

    Unit* unit = units[selectedUnitIndex];

    if (playerAction == PlayerAction::SelectingMove) {
        // 显示可移动的位置
        for (int x = 0; x < MAP_WIDTH; x++) {
            for (int y = 0; y < MAP_HEIGHT; y++) {
                if (unit->canMoveTo(x, y) && !gameMap.isOccupied(x, y)) {
                    highlightedTiles.push_back(make_pair(x, y));
                }
            }
        }
    }
    else if (playerAction == PlayerAction::SelectingAttack) {
        // 显示可攻击的目标
        for (Unit* target : units) {
            if (target->team != unit->team && target->isAlive && unit->canAttack(target)) {
                highlightedTiles.push_back(make_pair(target->gridX, target->gridY));
            }
        }
    }
}

void Game::clearHighlights() {
    highlightedTiles.clear();
}

// 获取指定位置的单位索引
int Game::getUnitIndexAt(int gridX, int gridY) {
    for (int i = 0; i < units.size(); i++) {
        if (units[i]->isAlive && units[i]->gridX == gridX && units[i]->gridY == gridY) {
            return i;
        }
    }
    return -1;
}

// 检查是否是有效的移动目标
bool Game::isValidMoveTarget(int gridX, int gridY) {
    if (selectedUnitIndex < 0) return false;
    Unit* unit = units[selectedUnitIndex];
    return unit->canMoveTo(gridX, gridY) && !gameMap.isOccupied(gridX, gridY);
}

// 检查是否是有效的攻击目标
bool Game::isValidAttackTarget(int gridX, int gridY) {
    if (selectedUnitIndex < 0) return false;

    Unit* unit = units[selectedUnitIndex];
    int targetIdx = getUnitIndexAt(gridX, gridY);

    if (targetIdx < 0) return false;

    Unit* target = units[targetIdx];
    return target->team != unit->team && target->isAlive && unit->canAttack(target);
}

// 下一回合
void Game::nextTurn() {
    checkGameOver();
    if (gameState == GameState::GameOver) return;

    // 检查是否所有玩家单位都已行动
    bool allPlayerActed = true;
    for (Unit* unit : units) {
        if (unit->team == Team::Player && unit->isAlive && !unit->hasActed) {
            allPlayerActed = false;
            break;
        }
    }

    if (allPlayerActed && gameState == GameState::PlayerTurn) {
        // 切换到AI回合
        gameState = GameState::AITurn;
        aiTurnTimer = 0;
        statusMessage = "AI回合...";
        cout << "\n=== AI回合开始 ===" << endl;
    }
    else if (gameState == GameState::AITurn) {
        // 切换到玩家回合
        gameState = GameState::PlayerTurn;
        statusMessage = "玩家回合";
        cout << "\n=== 玩家回合开始 ===" << endl;

        // 重置所有单位的行动状态
        for (Unit* unit : units) {
            unit->resetTurn();
        }
    }
}

// 执行AI回合
void Game::executeAITurn() {
    for (Unit* unit : units) {
        if (unit->team == Team::Enemy && unit->isAlive && !unit->hasActed) {
            cout << "\n--- " << unit->name << " 的回合 ---" << endl;

            // 根据当前AI类型执行决策
            if (currentAIType == AIType::FSM) {
                fsmAI.update(unit, units, gameMap);
            } else {
                btAI.update(unit, units, gameMap);
            }

            gameMap.updateOccupancy(units);
            checkGameOver();
            return;  // 每帧只执行一个AI单位
        }
    }

    // 所有AI单位都已行动
    nextTurn();
}

// 检查游戏是否结束
void Game::checkGameOver() {
    int playerAlive = 0;
    int enemyAlive = 0;

    for (Unit* unit : units) {
        if (unit->isAlive) {
            if (unit->team == Team::Player) {
                playerAlive++;
            } else {
                enemyAlive++;
            }
        }
    }

    if (playerAlive == 0) {
        gameState = GameState::GameOver;
        statusMessage = "游戏结束 - AI获胜！";
        cout << "\n=== 游戏结束: AI获胜！ ===" << endl;
    }
    else if (enemyAlive == 0) {
        gameState = GameState::GameOver;
        statusMessage = "游戏结束 - 玩家获胜！";
        cout << "\n=== 游戏结束: 玩家获胜！ ===" << endl;
    }
}

bool Game::isGameOver() {
    return gameState == GameState::GameOver;
}

// 更新游戏状态
void Game::update(float dt) {
    if (gameState == GameState::AITurn) {
        aiTurnTimer += dt;
        if (aiTurnTimer >= aiTurnDelay) {
            aiTurnTimer = 0;
            executeAITurn();
        }
    }
}

// 渲染
void Game::render(sf::RenderWindow& window) {
    drawMap(window);
    drawHighlights(window);
    drawUnits(window);
    drawUI(window);
}

// 绘制地图
void Game::drawMap(sf::RenderWindow& window) {
    sf::Sprite groundSprite(groundTexture);

    for (int x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            groundSprite.setPosition({gameMap.gridToScreenX(x), gameMap.gridToScreenY(y)});
            window.draw(groundSprite);
        }
    }
}

// 绘制高亮
void Game::drawHighlights(sf::RenderWindow& window) {
    sf::Sprite highlightSprite(highlightTexture);
    highlightSprite.setColor(highlightColor);

    for (auto& tile : highlightedTiles) {
        highlightSprite.setPosition({gameMap.gridToScreenX(tile.first),
                                    gameMap.gridToScreenY(tile.second)});
        window.draw(highlightSprite);
    }
}

// 绘制单位
void Game::drawUnits(sf::RenderWindow& window) {
    for (int i = 0; i < units.size(); i++) {
        Unit* unit = units[i];
        if (!unit->isAlive) continue;

        sf::Sprite sprite(unit->unitType == UnitType::Warrior ? warriorTexture : archerTexture);
        sprite.setPosition({gameMap.gridToScreenX(unit->gridX),
                           gameMap.gridToScreenY(unit->gridY)});

        // 根据队伍设置颜色
        if (unit->team == Team::Player) {
            sprite.setColor(sf::Color(150, 150, 255));  // 蓝色
        } else {
            sprite.setColor(sf::Color(255, 150, 150));  // 红色
        }

        // 如果是选中的单位，添加高亮
        if (i == selectedUnitIndex) {
            sprite.setColor(sf::Color(255, 255, 100));  // 黄色
        }

        window.draw(sprite);

        // 绘制生命条
        float healthPercent = (float)unit->health / unit->maxHealth;
        sf::RectangleShape healthBg(sf::Vector2f(60, 6));
        healthBg.setPosition({gameMap.gridToScreenX(unit->gridX) + 2,
                             gameMap.gridToScreenY(unit->gridY) - 8});
        healthBg.setFillColor(sf::Color(100, 100, 100));
        window.draw(healthBg);

        sf::RectangleShape healthBar(sf::Vector2f(60 * healthPercent, 6));
        healthBar.setPosition({gameMap.gridToScreenX(unit->gridX) + 2,
                              gameMap.gridToScreenY(unit->gridY) - 8});
        healthBar.setFillColor(healthPercent > 0.3f ? sf::Color::Green : sf::Color::Red);
        window.draw(healthBar);
    }
}

// 绘制UI
void Game::drawUI(sf::RenderWindow& window) {
    // 状态信息
    sf::Text statusText(font);
    statusText.setString(statusMessage);
    statusText.setCharacterSize(24);
    statusText.setFillColor(sf::Color::White);
    statusText.setPosition({10, 520});
    window.draw(statusText);

    // AI类型显示
    sf::Text aiText(font);
    string aiStr = "当前AI: ";
    aiStr += (currentAIType == AIType::FSM ? "FSM(有限状态机)" : "行为树");
    aiStr += " [Tab切换]";
    aiText.setString(aiStr);
    aiText.setCharacterSize(20);
    aiText.setFillColor(sf::Color::Yellow);
    aiText.setPosition({10, 550});
    window.draw(aiText);

    // 操作提示
    sf::Text helpText(font);
    helpText.setString("操作: 左键选择/移动/攻击 | 右键取消 | 空格跳过 | R重置 | ESC退出");
    helpText.setCharacterSize(16);
    helpText.setFillColor(sf::Color(200, 200, 200));
    helpText.setPosition({10, 580});
    window.draw(helpText);

    // 单位信息面板
    float panelY = 10;
    for (Unit* unit : units) {
        if (!unit->isAlive) continue;

        sf::Text unitInfo(font);
        ostringstream ss;
        ss << unit->name << " [" << unit->getTypeName() << "] ";
        ss << "HP:" << unit->health << "/" << unit->maxHealth;
        ss << " 位置:(" << unit->gridX << "," << unit->gridY << ")";
        if (unit->hasActed) ss << " [已行动]";

        unitInfo.setString(ss.str());
        unitInfo.setCharacterSize(16);
        unitInfo.setFillColor(unit->team == Team::Player ?
                             sf::Color(150, 150, 255) : sf::Color(255, 150, 150));
        unitInfo.setPosition({550, panelY});
        window.draw(unitInfo);
        panelY += 22;
    }
}
