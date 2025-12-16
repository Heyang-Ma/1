#pragma once
#ifndef FSM_AI_H
#define FSM_AI_H

#include "Unit.h"
#include "GameMap.h"
#include <vector>

using namespace std;

/*
============================================================================
有限状态机（Finite State Machine, FSM）AI 说明
============================================================================

FSM是一种简单直观的AI设计模式，核心思想是：
1. AI在任意时刻只能处于一个"状态"
2. 根据条件（触发器）在不同状态之间"转换"
3. 每个状态有对应的行为

本游戏中的状态设计：

【近战战士状态图】
    ┌─────────┐
    │  IDLE   │←──────────────────────────┐
    │ (待机)  │                           │
    └────┬────┘                           │
         │ 发现敌人                       │ 敌人死亡
         ▼                                │
    ┌─────────┐    敌人在攻击范围    ┌─────────┐
    │  CHASE  │───────────────────→│ ATTACK  │
    │ (追击)  │←───────────────────│ (攻击)  │
    └────┬────┘    敌人逃离范围     └─────────┘
         │
         │ 生命值低(<30%)
         ▼
    ┌─────────┐
    │ RETREAT │
    │ (撤退)  │
    └─────────┘

【远程弓箭手状态图】
    ┌─────────┐
    │  IDLE   │←──────────────────────────┐
    │ (待机)  │                           │
    └────┬────┘                           │ 敌人死亡
         │ 发现敌人                       │
         ▼                                │
    ┌─────────┐    敌人在攻击范围    ┌─────────┐
    │  KITE   │───────────────────→│ ATTACK  │
    │(保持距离)│←───────────────────│ (攻击)  │
    └────┬────┘    需要调整位置     └─────────┘
         │
         │ 敌人太近(距离<=1)
         ▼
    ┌─────────┐
    │  FLEE   │
    │ (逃离)  │
    └─────────┘

============================================================================
*/

// FSM状态枚举
enum class FSMState {
    IDLE,       // 待机状态：没有敌人或等待
    CHASE,      // 追击状态：向敌人移动（近战用）
    ATTACK,     // 攻击状态：攻击范围内的敌人
    RETREAT,    // 撤退状态：生命值低时远离敌人（近战用）
    KITE,       // 风筝状态：保持安全距离同时攻击（远程用）
    FLEE        // 逃离状态：被近身时快速远离（远程用）
};

// 将状态转换为字符串（用于调试输出）
string stateToString(FSMState state);

// FSM AI类
class FSM_AI {
public:
    FSM_AI();

    // 执行AI决策并返回决定的动作
    // 参数:
    //   unit - 需要做决策的AI单位
    //   allUnits - 所有单位列表（用于找敌人）
    //   gameMap - 游戏地图（用于移动计算）
    void update(Unit* unit, vector<Unit*>& allUnits, GameMap& gameMap);

    // 获取当前状态
    FSMState getCurrentState();

    // 低生命值阈值（百分比）
    float lowHealthThreshold = 0.3f; // 30%

    // 远程单位的安全距离
    int safeDistance = 2;

private:
    FSMState currentState;
    Unit* currentTarget;

    // 查找最近的敌人
    Unit* findNearestEnemy(Unit* unit, vector<Unit*>& allUnits);

    // 状态转换逻辑
    void updateStateWarrior(Unit* unit, Unit* nearestEnemy);
    void updateStateArcher(Unit* unit, Unit* nearestEnemy);

    // 各状态的行为执行
    void executeIdle(Unit* unit);
    void executeChase(Unit* unit, Unit* target, GameMap& gameMap, vector<Unit*>& allUnits);
    void executeAttack(Unit* unit, Unit* target);
    void executeRetreat(Unit* unit, Unit* target, GameMap& gameMap, vector<Unit*>& allUnits);
    void executeKite(Unit* unit, Unit* target, GameMap& gameMap, vector<Unit*>& allUnits);
    void executeFlee(Unit* unit, Unit* target, GameMap& gameMap, vector<Unit*>& allUnits);
};

#endif // FSM_AI_H
