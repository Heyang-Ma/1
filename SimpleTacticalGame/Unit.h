#pragma once
#ifndef UNIT_H
#define UNIT_H

#include <string>
#include <SFML/Graphics.hpp>

using namespace std;

// 单位类型枚举
enum class UnitType {
    Warrior,    // 近战战士
    Archer      // 远程弓箭手
};

// 队伍枚举
enum class Team {
    Player,     // 玩家队伍（蓝色）
    Enemy       // AI敌人队伍（红色）
};

// 单位类 - 代表游戏中的一个角色
class Unit {
public:
    // 构造函数
    Unit(UnitType type, Team team, int startX, int startY);

    // 基本属性
    string name;           // 单位名称
    UnitType unitType;     // 单位类型
    Team team;             // 所属队伍

    // 位置
    int gridX;             // 网格X坐标
    int gridY;             // 网格Y坐标

    // 战斗属性
    int health;            // 当前生命值
    int maxHealth;         // 最大生命值
    int attackPower;       // 攻击力
    int moveRange;         // 移动范围
    int attackRange;       // 攻击范围

    // 状态
    bool isAlive;          // 是否存活
    bool hasActed;         // 本回合是否已行动

    // 方法
    void move(int newX, int newY);           // 移动到新位置
    void attack(Unit* target);               // 攻击目标
    void takeDamage(int damage);             // 受到伤害
    void resetTurn();                        // 重置回合状态

    // 计算到目标的曼哈顿距离
    int distanceTo(int x, int y);
    int distanceTo(Unit* other);

    // 检查是否能攻击目标
    bool canAttack(Unit* target);

    // 检查是否能移动到目标位置
    bool canMoveTo(int x, int y);

    // 获取单位类型的字符串表示（用于显示）
    string getTypeName();
};

#endif // UNIT_H
