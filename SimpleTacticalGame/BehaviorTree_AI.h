#pragma once
#ifndef BEHAVIORTREE_AI_H
#define BEHAVIORTREE_AI_H

#include "Unit.h"
#include "GameMap.h"
#include <vector>
#include <memory>
#include <functional>

using namespace std;

/*
============================================================================
行为树（Behavior Tree, BT）AI 说明
============================================================================

行为树是一种层次化的AI决策结构，由节点组成：

【节点类型】
1. 叶子节点（Leaf Nodes）：
   - 条件节点（Condition）：检查条件，返回成功/失败
   - 动作节点（Action）：执行行为，返回成功/失败/运行中

2. 复合节点（Composite Nodes）：
   - 选择器（Selector）：依次执行子节点，直到一个成功（OR逻辑）
   - 序列（Sequence）：依次执行子节点，直到一个失败（AND逻辑）

【节点返回状态】
- SUCCESS（成功）：节点执行完成
- FAILURE（失败）：节点执行失败
- RUNNING（运行中）：节点还在执行

【近战战士行为树结构】
                         Selector (根选择器)
                              │
        ┌─────────────────────┼─────────────────────┐
        │                     │                     │
   Sequence               Sequence             Sequence
   (撤退分支)             (攻击分支)           (追击分支)
        │                     │                     │
   ┌────┴────┐           ┌────┴────┐          ┌────┴────┐
   │         │           │         │          │         │
 生命低?   撤退      在攻击范围?  攻击     有敌人?    追击

【远程弓箭手行为树结构】
                         Selector (根选择器)
                              │
        ┌─────────────────────┼─────────────────────┐
        │                     │                     │
   Sequence               Sequence             Sequence
   (逃离分支)             (攻击分支)           (风筝分支)
        │                     │                     │
   ┌────┴────┐           ┌────┴────┐          ┌────┴────┐
   │         │           │         │          │         │
被近身?    逃离       安全+范围内?  攻击     有敌人?   风筝

============================================================================
*/

// 行为树节点状态
enum class BTStatus {
    SUCCESS,    // 成功
    FAILURE,    // 失败
    RUNNING     // 运行中
};

// 行为树上下文 - 存储决策所需的所有信息
struct BTContext {
    Unit* unit;                 // 当前AI控制的单位
    Unit* nearestEnemy;         // 最近的敌人
    vector<Unit*>* allUnits;    // 所有单位指针
    GameMap* gameMap;           // 游戏地图指针

    // 配置参数
    float lowHealthThreshold = 0.3f;
    int safeDistance = 2;
};

// ============================================================================
// 行为树节点基类
// ============================================================================
class BTNode {
public:
    virtual ~BTNode() = default;
    virtual BTStatus execute(BTContext& context) = 0;
    virtual string getName() { return "BTNode"; }
};

// ============================================================================
// 复合节点：选择器（Selector）
// 依次执行子节点，直到一个返回SUCCESS
// 类似于 OR 逻辑
// ============================================================================
class BTSelector : public BTNode {
public:
    string name;

    BTSelector(string n = "Selector") : name(n) {}

    void addChild(shared_ptr<BTNode> child) {
        children.push_back(child);
    }

    BTStatus execute(BTContext& context) override {
        for (auto& child : children) {
            BTStatus status = child->execute(context);
            if (status == BTStatus::SUCCESS) {
                return BTStatus::SUCCESS;
            }
            if (status == BTStatus::RUNNING) {
                return BTStatus::RUNNING;
            }
            // FAILURE 则继续下一个子节点
        }
        return BTStatus::FAILURE;
    }

    string getName() override { return name; }

private:
    vector<shared_ptr<BTNode>> children;
};

// ============================================================================
// 复合节点：序列（Sequence）
// 依次执行子节点，直到一个返回FAILURE
// 类似于 AND 逻辑
// ============================================================================
class BTSequence : public BTNode {
public:
    string name;

    BTSequence(string n = "Sequence") : name(n) {}

    void addChild(shared_ptr<BTNode> child) {
        children.push_back(child);
    }

    BTStatus execute(BTContext& context) override {
        for (auto& child : children) {
            BTStatus status = child->execute(context);
            if (status == BTStatus::FAILURE) {
                return BTStatus::FAILURE;
            }
            if (status == BTStatus::RUNNING) {
                return BTStatus::RUNNING;
            }
            // SUCCESS 则继续下一个子节点
        }
        return BTStatus::SUCCESS;
    }

    string getName() override { return name; }

private:
    vector<shared_ptr<BTNode>> children;
};

// ============================================================================
// 叶子节点：条件检查
// ============================================================================
class BTCondition : public BTNode {
public:
    string name;
    function<bool(BTContext&)> conditionFunc;

    BTCondition(string n, function<bool(BTContext&)> func)
        : name(n), conditionFunc(func) {}

    BTStatus execute(BTContext& context) override {
        bool result = conditionFunc(context);
        cout << "[BT] 条件检查: " << name << " = " << (result ? "TRUE" : "FALSE") << endl;
        return result ? BTStatus::SUCCESS : BTStatus::FAILURE;
    }

    string getName() override { return name; }
};

// ============================================================================
// 叶子节点：动作执行
// ============================================================================
class BTAction : public BTNode {
public:
    string name;
    function<BTStatus(BTContext&)> actionFunc;

    BTAction(string n, function<BTStatus(BTContext&)> func)
        : name(n), actionFunc(func) {}

    BTStatus execute(BTContext& context) override {
        cout << "[BT] 执行动作: " << name << endl;
        return actionFunc(context);
    }

    string getName() override { return name; }
};

// ============================================================================
// 行为树AI类
// ============================================================================
class BehaviorTree_AI {
public:
    BehaviorTree_AI();

    // 执行AI决策
    void update(Unit* unit, vector<Unit*>& allUnits, GameMap& gameMap);

    // 配置参数
    float lowHealthThreshold = 0.3f;
    int safeDistance = 2;

private:
    shared_ptr<BTNode> warriorTree;
    shared_ptr<BTNode> archerTree;

    // 构建行为树
    void buildWarriorTree();
    void buildArcherTree();

    // 辅助函数
    Unit* findNearestEnemy(Unit* unit, vector<Unit*>& allUnits);
};

#endif // BEHAVIORTREE_AI_H
