#include "BehaviorTree_AI.h"
#include <iostream>
#include <climits>

using namespace std;

// ============================================================================
// 条件函数定义
// ============================================================================

// 检查是否有敌人
bool hasEnemy(BTContext& ctx) {
    return ctx.nearestEnemy != nullptr && ctx.nearestEnemy->isAlive;
}

// 检查生命值是否低
bool isLowHealth(BTContext& ctx) {
    float healthPercent = (float)ctx.unit->health / ctx.unit->maxHealth;
    return healthPercent < ctx.lowHealthThreshold;
}

// 检查敌人是否在攻击范围内
bool isEnemyInAttackRange(BTContext& ctx) {
    if (!hasEnemy(ctx)) return false;
    return ctx.unit->canAttack(ctx.nearestEnemy);
}

// 检查是否被近身（远程单位用）
bool isEnemyTooClose(BTContext& ctx) {
    if (!hasEnemy(ctx)) return false;
    return ctx.unit->distanceTo(ctx.nearestEnemy) <= 1;
}

// 检查是否在安全距离且在攻击范围内（远程单位用）
bool isSafeAndInRange(BTContext& ctx) {
    if (!hasEnemy(ctx)) return false;
    int dist = ctx.unit->distanceTo(ctx.nearestEnemy);
    return dist >= ctx.safeDistance && dist <= ctx.unit->attackRange;
}

// ============================================================================
// 动作函数定义
// ============================================================================

// 攻击最近的敌人
BTStatus attackEnemy(BTContext& ctx) {
    if (ctx.nearestEnemy == nullptr || !ctx.nearestEnemy->isAlive) {
        return BTStatus::FAILURE;
    }

    ctx.unit->attack(ctx.nearestEnemy);
    return BTStatus::SUCCESS;
}

// 追击敌人（向敌人移动）
BTStatus chaseEnemy(BTContext& ctx) {
    if (ctx.nearestEnemy == nullptr) {
        return BTStatus::FAILURE;
    }

    cout << "[BT] " << ctx.unit->name << " 追击 " << ctx.nearestEnemy->name << endl;

    pair<int, int> newPos = ctx.gameMap->getBestMoveTowards(
        ctx.unit,
        ctx.nearestEnemy->gridX,
        ctx.nearestEnemy->gridY,
        *ctx.allUnits
    );

    if (newPos.first != ctx.unit->gridX || newPos.second != ctx.unit->gridY) {
        ctx.unit->move(newPos.first, newPos.second);
        ctx.gameMap->updateOccupancy(*ctx.allUnits);

        // 移动后尝试攻击
        if (ctx.unit->canAttack(ctx.nearestEnemy)) {
            ctx.unit->attack(ctx.nearestEnemy);
        }
    }

    return BTStatus::SUCCESS;
}

// 撤退（远离敌人）
BTStatus retreat(BTContext& ctx) {
    if (ctx.nearestEnemy == nullptr) {
        return BTStatus::FAILURE;
    }

    cout << "[BT] " << ctx.unit->name << " 撤退中! (HP: "
         << ctx.unit->health << "/" << ctx.unit->maxHealth << ")" << endl;

    pair<int, int> newPos = ctx.gameMap->getBestMoveAwayFrom(
        ctx.unit,
        ctx.nearestEnemy->gridX,
        ctx.nearestEnemy->gridY,
        *ctx.allUnits
    );

    if (newPos.first != ctx.unit->gridX || newPos.second != ctx.unit->gridY) {
        ctx.unit->move(newPos.first, newPos.second);
        ctx.gameMap->updateOccupancy(*ctx.allUnits);
    }

    return BTStatus::SUCCESS;
}

// 逃离（快速远离近身敌人）
BTStatus flee(BTContext& ctx) {
    if (ctx.nearestEnemy == nullptr) {
        return BTStatus::FAILURE;
    }

    cout << "[BT] " << ctx.unit->name << " 被近身，逃离中!" << endl;

    pair<int, int> newPos = ctx.gameMap->getBestMoveAwayFrom(
        ctx.unit,
        ctx.nearestEnemy->gridX,
        ctx.nearestEnemy->gridY,
        *ctx.allUnits
    );

    if (newPos.first != ctx.unit->gridX || newPos.second != ctx.unit->gridY) {
        ctx.unit->move(newPos.first, newPos.second);
        ctx.gameMap->updateOccupancy(*ctx.allUnits);
    }

    // 逃离后尝试攻击
    if (ctx.unit->canAttack(ctx.nearestEnemy)) {
        ctx.unit->attack(ctx.nearestEnemy);
    }

    return BTStatus::SUCCESS;
}

// 风筝（保持距离）
BTStatus kite(BTContext& ctx) {
    if (ctx.nearestEnemy == nullptr) {
        return BTStatus::FAILURE;
    }

    int dist = ctx.unit->distanceTo(ctx.nearestEnemy);
    cout << "[BT] " << ctx.unit->name << " 风筝中 (距离: " << dist << ")" << endl;

    // 如果太近，远离
    if (dist < ctx.safeDistance) {
        pair<int, int> newPos = ctx.gameMap->getBestMoveAwayFrom(
            ctx.unit,
            ctx.nearestEnemy->gridX,
            ctx.nearestEnemy->gridY,
            *ctx.allUnits
        );
        if (newPos.first != ctx.unit->gridX || newPos.second != ctx.unit->gridY) {
            ctx.unit->move(newPos.first, newPos.second);
            ctx.gameMap->updateOccupancy(*ctx.allUnits);
        }
    }
    // 如果太远，靠近
    else if (dist > ctx.unit->attackRange) {
        pair<int, int> newPos = ctx.gameMap->getBestMoveTowards(
            ctx.unit,
            ctx.nearestEnemy->gridX,
            ctx.nearestEnemy->gridY,
            *ctx.allUnits
        );
        if (newPos.first != ctx.unit->gridX || newPos.second != ctx.unit->gridY) {
            ctx.unit->move(newPos.first, newPos.second);
            ctx.gameMap->updateOccupancy(*ctx.allUnits);
        }
    }

    // 尝试攻击
    if (ctx.unit->canAttack(ctx.nearestEnemy)) {
        ctx.unit->attack(ctx.nearestEnemy);
    }

    return BTStatus::SUCCESS;
}

// 待机
BTStatus idle(BTContext& ctx) {
    cout << "[BT] " << ctx.unit->name << " 待机中" << endl;
    return BTStatus::SUCCESS;
}

// ============================================================================
// BehaviorTree_AI 实现
// ============================================================================

BehaviorTree_AI::BehaviorTree_AI() {
    buildWarriorTree();
    buildArcherTree();
}

// 查找最近的敌人
Unit* BehaviorTree_AI::findNearestEnemy(Unit* unit, vector<Unit*>& allUnits) {
    Unit* nearest = nullptr;
    int minDist = INT_MAX;

    for (Unit* other : allUnits) {
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
// 构建近战战士的行为树
// ============================================================================
void BehaviorTree_AI::buildWarriorTree() {
    /*
    行为树结构：
    Selector (根)
    ├── Sequence (撤退)
    │   ├── Condition: 生命值低?
    │   └── Action: 撤退
    ├── Sequence (攻击)
    │   ├── Condition: 敌人在攻击范围?
    │   └── Action: 攻击
    └── Sequence (追击)
        ├── Condition: 有敌人?
        └── Action: 追击
    */

    auto root = make_shared<BTSelector>("战士根选择器");

    // 撤退分支
    auto retreatSeq = make_shared<BTSequence>("撤退序列");
    retreatSeq->addChild(make_shared<BTCondition>("生命值低?", isLowHealth));
    retreatSeq->addChild(make_shared<BTAction>("撤退", retreat));

    // 攻击分支
    auto attackSeq = make_shared<BTSequence>("攻击序列");
    attackSeq->addChild(make_shared<BTCondition>("敌人在攻击范围?", isEnemyInAttackRange));
    attackSeq->addChild(make_shared<BTAction>("攻击", attackEnemy));

    // 追击分支
    auto chaseSeq = make_shared<BTSequence>("追击序列");
    chaseSeq->addChild(make_shared<BTCondition>("有敌人?", hasEnemy));
    chaseSeq->addChild(make_shared<BTAction>("追击", chaseEnemy));

    // 添加到根节点
    root->addChild(retreatSeq);
    root->addChild(attackSeq);
    root->addChild(chaseSeq);

    warriorTree = root;
}

// ============================================================================
// 构建远程弓箭手的行为树
// ============================================================================
void BehaviorTree_AI::buildArcherTree() {
    /*
    行为树结构：
    Selector (根)
    ├── Sequence (逃离)
    │   ├── Condition: 被近身?
    │   └── Action: 逃离
    ├── Sequence (安全攻击)
    │   ├── Condition: 安全距离且在范围内?
    │   └── Action: 攻击
    └── Sequence (风筝)
        ├── Condition: 有敌人?
        └── Action: 风筝
    */

    auto root = make_shared<BTSelector>("弓箭手根选择器");

    // 逃离分支
    auto fleeSeq = make_shared<BTSequence>("逃离序列");
    fleeSeq->addChild(make_shared<BTCondition>("被近身?", isEnemyTooClose));
    fleeSeq->addChild(make_shared<BTAction>("逃离", flee));

    // 安全攻击分支
    auto safeAttackSeq = make_shared<BTSequence>("安全攻击序列");
    safeAttackSeq->addChild(make_shared<BTCondition>("安全且在范围?", isSafeAndInRange));
    safeAttackSeq->addChild(make_shared<BTAction>("攻击", attackEnemy));

    // 风筝分支
    auto kiteSeq = make_shared<BTSequence>("风筝序列");
    kiteSeq->addChild(make_shared<BTCondition>("有敌人?", hasEnemy));
    kiteSeq->addChild(make_shared<BTAction>("风筝", kite));

    // 添加到根节点
    root->addChild(fleeSeq);
    root->addChild(safeAttackSeq);
    root->addChild(kiteSeq);

    archerTree = root;
}

// ============================================================================
// 主更新函数
// ============================================================================
void BehaviorTree_AI::update(Unit* unit, vector<Unit*>& allUnits, GameMap& gameMap) {
    if (unit == nullptr || !unit->isAlive || unit->hasActed) {
        return;
    }

    cout << "\n[BT] ========== " << unit->name << " 行为树决策 ==========" << endl;

    // 构建上下文
    BTContext context;
    context.unit = unit;
    context.nearestEnemy = findNearestEnemy(unit, allUnits);
    context.allUnits = &allUnits;
    context.gameMap = &gameMap;
    context.lowHealthThreshold = lowHealthThreshold;
    context.safeDistance = safeDistance;

    // 根据单位类型执行对应的行为树
    if (unit->unitType == UnitType::Warrior) {
        warriorTree->execute(context);
    } else {
        archerTree->execute(context);
    }

    // 标记已行动
    unit->hasActed = true;

    cout << "[BT] ========== 决策完成 ==========\n" << endl;
}
