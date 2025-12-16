#include "FSM_AI.h"
#include <iostream>
#include <climits>

using namespace std;

// 状态转字符串
string stateToString(FSMState state) {
    switch (state) {
        case FSMState::IDLE:    return "IDLE(待机)";
        case FSMState::CHASE:   return "CHASE(追击)";
        case FSMState::ATTACK:  return "ATTACK(攻击)";
        case FSMState::RETREAT: return "RETREAT(撤退)";
        case FSMState::KITE:    return "KITE(保持距离)";
        case FSMState::FLEE:    return "FLEE(逃离)";
        default:                return "UNKNOWN";
    }
}

FSM_AI::FSM_AI() {
    currentState = FSMState::IDLE;
    currentTarget = nullptr;
}

FSMState FSM_AI::getCurrentState() {
    return currentState;
}

// 查找最近的敌人
Unit* FSM_AI::findNearestEnemy(Unit* unit, vector<Unit*>& allUnits) {
    Unit* nearest = nullptr;
    int minDist = INT_MAX;

    for (Unit* other : allUnits) {
        // 跳过己方单位和死亡单位
        if (other == nullptr || !other->isAlive || other->team == unit->team) {
            continue;
        }

        int dist = unit->distanceTo(other);
        if (dist < minDist) {
            minDist = dist;
            nearest = other;
        }
    }

    return nearest;
}

// ============================================================================
// 主更新函数 - AI决策的入口点
// ============================================================================
void FSM_AI::update(Unit* unit, vector<Unit*>& allUnits, GameMap& gameMap) {
    if (unit == nullptr || !unit->isAlive || unit->hasActed) {
        return;
    }

    // 1. 查找最近的敌人
    Unit* nearestEnemy = findNearestEnemy(unit, allUnits);

    // 2. 根据单位类型更新状态
    FSMState previousState = currentState;

    if (unit->unitType == UnitType::Warrior) {
        updateStateWarrior(unit, nearestEnemy);
    } else {
        updateStateArcher(unit, nearestEnemy);
    }

    // 输出状态变化（调试用）
    if (previousState != currentState) {
        cout << "[FSM] " << unit->name << " 状态转换: "
             << stateToString(previousState) << " -> "
             << stateToString(currentState) << endl;
    }

    // 3. 执行当前状态的行为
    switch (currentState) {
        case FSMState::IDLE:
            executeIdle(unit);
            break;
        case FSMState::CHASE:
            executeChase(unit, nearestEnemy, gameMap, allUnits);
            break;
        case FSMState::ATTACK:
            executeAttack(unit, nearestEnemy);
            break;
        case FSMState::RETREAT:
            executeRetreat(unit, nearestEnemy, gameMap, allUnits);
            break;
        case FSMState::KITE:
            executeKite(unit, nearestEnemy, gameMap, allUnits);
            break;
        case FSMState::FLEE:
            executeFlee(unit, nearestEnemy, gameMap, allUnits);
            break;
    }

    // 标记已行动
    unit->hasActed = true;
}

// ============================================================================
// 近战战士的状态转换逻辑
// ============================================================================
void FSM_AI::updateStateWarrior(Unit* unit, Unit* nearestEnemy) {
    // 如果没有敌人，进入待机状态
    if (nearestEnemy == nullptr) {
        currentState = FSMState::IDLE;
        return;
    }

    int dist = unit->distanceTo(nearestEnemy);
    float healthPercent = (float)unit->health / unit->maxHealth;

    // 决策优先级：
    // 1. 生命值低 -> 撤退
    // 2. 敌人在攻击范围内 -> 攻击
    // 3. 否则 -> 追击

    if (healthPercent < lowHealthThreshold) {
        // 生命值低于阈值，撤退！
        currentState = FSMState::RETREAT;
    }
    else if (dist <= unit->attackRange) {
        // 敌人在攻击范围内，攻击！
        currentState = FSMState::ATTACK;
    }
    else {
        // 敌人不在攻击范围，追击！
        currentState = FSMState::CHASE;
    }
}

// ============================================================================
// 远程弓箭手的状态转换逻辑
// ============================================================================
void FSM_AI::updateStateArcher(Unit* unit, Unit* nearestEnemy) {
    // 如果没有敌人，进入待机状态
    if (nearestEnemy == nullptr) {
        currentState = FSMState::IDLE;
        return;
    }

    int dist = unit->distanceTo(nearestEnemy);

    // 远程单位的决策优先级：
    // 1. 敌人太近（被近身）-> 逃离
    // 2. 敌人在攻击范围内且距离安全 -> 攻击
    // 3. 否则 -> 风筝（调整位置）

    if (dist <= 1) {
        // 被近身了！逃离！
        currentState = FSMState::FLEE;
    }
    else if (dist >= safeDistance && dist <= unit->attackRange) {
        // 在安全距离且在攻击范围内，攻击！
        currentState = FSMState::ATTACK;
    }
    else {
        // 需要调整位置（太近或太远）
        currentState = FSMState::KITE;
    }
}

// ============================================================================
// 状态行为实现
// ============================================================================

// 待机状态：什么都不做
void FSM_AI::executeIdle(Unit* unit) {
    cout << "[FSM] " << unit->name << " 待机中，没有发现敌人" << endl;
}

// 追击状态：向最近的敌人移动
void FSM_AI::executeChase(Unit* unit, Unit* target, GameMap& gameMap, vector<Unit*>& allUnits) {
    if (target == nullptr) return;

    cout << "[FSM] " << unit->name << " 正在追击 " << target->name << endl;

    // 计算朝向敌人的最佳移动位置
    pair<int, int> newPos = gameMap.getBestMoveTowards(
        unit, target->gridX, target->gridY, allUnits
    );

    // 如果可以移动到新位置
    if (newPos.first != unit->gridX || newPos.second != unit->gridY) {
        unit->move(newPos.first, newPos.second);
        gameMap.updateOccupancy(allUnits);

        // 移动后检查是否可以攻击
        if (unit->canAttack(target)) {
            unit->attack(target);
        }
    }
}

// 攻击状态：攻击目标
void FSM_AI::executeAttack(Unit* unit, Unit* target) {
    if (target == nullptr) return;

    cout << "[FSM] " << unit->name << " 发起攻击！" << endl;
    unit->attack(target);
}

// 撤退状态：远离敌人
void FSM_AI::executeRetreat(Unit* unit, Unit* target, GameMap& gameMap, vector<Unit*>& allUnits) {
    if (target == nullptr) return;

    cout << "[FSM] " << unit->name << " 生命值低，正在撤退！(HP: "
         << unit->health << "/" << unit->maxHealth << ")" << endl;

    // 计算远离敌人的最佳移动位置
    pair<int, int> newPos = gameMap.getBestMoveAwayFrom(
        unit, target->gridX, target->gridY, allUnits
    );

    if (newPos.first != unit->gridX || newPos.second != unit->gridY) {
        unit->move(newPos.first, newPos.second);
        gameMap.updateOccupancy(allUnits);
    }
}

// 风筝状态：保持安全距离同时尝试攻击
void FSM_AI::executeKite(Unit* unit, Unit* target, GameMap& gameMap, vector<Unit*>& allUnits) {
    if (target == nullptr) return;

    int dist = unit->distanceTo(target);
    cout << "[FSM] " << unit->name << " 正在调整位置 (当前距离: " << dist << ")" << endl;

    // 如果太近，先移动
    if (dist < safeDistance) {
        // 远离敌人
        pair<int, int> newPos = gameMap.getBestMoveAwayFrom(
            unit, target->gridX, target->gridY, allUnits
        );
        if (newPos.first != unit->gridX || newPos.second != unit->gridY) {
            unit->move(newPos.first, newPos.second);
            gameMap.updateOccupancy(allUnits);
        }
    }
    else if (dist > unit->attackRange) {
        // 太远了，靠近一点
        pair<int, int> newPos = gameMap.getBestMoveTowards(
            unit, target->gridX, target->gridY, allUnits
        );
        if (newPos.first != unit->gridX || newPos.second != unit->gridY) {
            unit->move(newPos.first, newPos.second);
            gameMap.updateOccupancy(allUnits);
        }
    }

    // 移动后尝试攻击
    if (unit->canAttack(target)) {
        unit->attack(target);
    }
}

// 逃离状态：快速远离近身的敌人
void FSM_AI::executeFlee(Unit* unit, Unit* target, GameMap& gameMap, vector<Unit*>& allUnits) {
    if (target == nullptr) return;

    cout << "[FSM] " << unit->name << " 被近身，正在逃离！" << endl;

    // 尽可能远离敌人
    pair<int, int> newPos = gameMap.getBestMoveAwayFrom(
        unit, target->gridX, target->gridY, allUnits
    );

    if (newPos.first != unit->gridX || newPos.second != unit->gridY) {
        unit->move(newPos.first, newPos.second);
        gameMap.updateOccupancy(allUnits);
    }

    // 逃离后如果能攻击就攻击
    if (unit->canAttack(target)) {
        unit->attack(target);
    }
}
