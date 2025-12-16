#include "Unit.h"
#include <cmath>
#include <iostream>

using namespace std;

// 构造函数 - 根据单位类型初始化属性
Unit::Unit(UnitType type, Team t, int startX, int startY) {
    unitType = type;
    team = t;
    gridX = startX;
    gridY = startY;
    isAlive = true;
    hasActed = false;

    // 根据单位类型设置属性
    // ===============================
    // 单位属性设计说明：
    // - 战士：高生命值、高近战攻击、低移动、近战攻击范围1
    // - 弓箭手：低生命值、低攻击、高移动、远程攻击范围2-4
    // ===============================

    if (type == UnitType::Warrior) {
        // 近战战士
        name = (t == Team::Player) ? "玩家战士" : "敌方战士";
        maxHealth = 100;
        health = maxHealth;
        attackPower = 25;
        moveRange = 2;      // 移动范围较小
        attackRange = 1;    // 只能近战攻击（相邻格子）
    }
    else { // Archer
        // 远程弓箭手
        name = (t == Team::Player) ? "玩家弓箭手" : "敌方弓箭手";
        maxHealth = 60;
        health = maxHealth;
        attackPower = 20;
        moveRange = 3;      // 移动范围较大
        attackRange = 3;    // 可以远程攻击（1-3格距离）
    }
}

// 移动到新位置
void Unit::move(int newX, int newY) {
    cout << name << " 从 (" << gridX << "," << gridY
         << ") 移动到 (" << newX << "," << newY << ")" << endl;
    gridX = newX;
    gridY = newY;
}

// 攻击目标单位
void Unit::attack(Unit* target) {
    if (target == nullptr || !target->isAlive) {
        cout << name << " 攻击失败：无效目标" << endl;
        return;
    }

    cout << name << " 攻击 " << target->name
         << "，造成 " << attackPower << " 点伤害！" << endl;

    target->takeDamage(attackPower);
}

// 受到伤害
void Unit::takeDamage(int damage) {
    health -= damage;
    cout << name << " 受到 " << damage << " 点伤害，剩余生命值: " << health << endl;

    if (health <= 0) {
        health = 0;
        isAlive = false;
        cout << name << " 已被击败！" << endl;
    }
}

// 重置回合状态（每回合开始时调用）
void Unit::resetTurn() {
    hasActed = false;
}

// 计算到目标坐标的曼哈顿距离
// 曼哈顿距离 = |x1-x2| + |y1-y2|，用于网格游戏中的距离计算
int Unit::distanceTo(int x, int y) {
    return abs(gridX - x) + abs(gridY - y);
}

// 计算到另一个单位的距离
int Unit::distanceTo(Unit* other) {
    if (other == nullptr) return 9999;
    return distanceTo(other->gridX, other->gridY);
}

// 检查是否能攻击目标
bool Unit::canAttack(Unit* target) {
    if (target == nullptr || !target->isAlive) return false;
    if (target->team == team) return false; // 不能攻击己方

    int dist = distanceTo(target);

    // 近战单位：只能攻击相邻格子（距离=1）
    if (unitType == UnitType::Warrior) {
        return dist == 1;
    }
    // 远程单位：可以攻击1-attackRange内的目标
    else {
        return dist >= 1 && dist <= attackRange;
    }
}

// 检查是否能移动到目标位置（简化版，不考虑障碍物）
bool Unit::canMoveTo(int x, int y) {
    int dist = distanceTo(x, y);
    return dist > 0 && dist <= moveRange;
}

// 获取单位类型名称
string Unit::getTypeName() {
    return (unitType == UnitType::Warrior) ? "战士" : "弓箭手";
}
