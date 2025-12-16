#include "GameMap.h"
#include <cmath>
#include <algorithm>

using namespace std;

GameMap::GameMap() {
    // 初始化所有格子为未占用
    for (int x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            occupied[x][y] = false;
        }
    }
}

// 检查坐标是否在地图范围内
bool GameMap::isValidPosition(int x, int y) {
    return x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT;
}

// 检查格子是否被占用
bool GameMap::isOccupied(int x, int y) {
    if (!isValidPosition(x, y)) return true; // 地图外视为被占用
    return occupied[x][y];
}

// 根据单位列表更新占用状态
void GameMap::updateOccupancy(vector<Unit*>& units) {
    // 先清空所有占用状态
    for (int x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            occupied[x][y] = false;
        }
    }

    // 标记所有存活单位的位置为占用
    for (Unit* unit : units) {
        if (unit != nullptr && unit->isAlive) {
            occupied[unit->gridX][unit->gridY] = true;
        }
    }
}

// 查找某位置的单位
Unit* GameMap::getUnitAt(int x, int y, vector<Unit*>& units) {
    for (Unit* unit : units) {
        if (unit != nullptr && unit->isAlive &&
            unit->gridX == x && unit->gridY == y) {
            return unit;
        }
    }
    return nullptr;
}

// 计算朝向目标移动的最佳位置
// 算法：在移动范围内找到距离目标最近的空格子
pair<int, int> GameMap::getBestMoveTowards(Unit* mover, int targetX, int targetY,
                                            vector<Unit*>& units) {
    int bestX = mover->gridX;
    int bestY = mover->gridY;
    int bestDist = abs(mover->gridX - targetX) + abs(mover->gridY - targetY);

    // 遍历移动范围内的所有格子
    for (int dx = -mover->moveRange; dx <= mover->moveRange; dx++) {
        for (int dy = -mover->moveRange; dy <= mover->moveRange; dy++) {
            int newX = mover->gridX + dx;
            int newY = mover->gridY + dy;

            // 检查是否在地图范围内
            if (!isValidPosition(newX, newY)) continue;

            // 检查是否在移动范围内（曼哈顿距离）
            if (abs(dx) + abs(dy) > mover->moveRange) continue;

            // 检查格子是否为空（或者是自己的位置）
            if (isOccupied(newX, newY) &&
                !(newX == mover->gridX && newY == mover->gridY)) continue;

            // 计算到目标的距离
            int distToTarget = abs(newX - targetX) + abs(newY - targetY);

            // 如果更近，更新最佳位置
            if (distToTarget < bestDist) {
                bestDist = distToTarget;
                bestX = newX;
                bestY = newY;
            }
        }
    }

    return make_pair(bestX, bestY);
}

// 计算远离目标移动的最佳位置
// 算法：在移动范围内找到距离目标最远的空格子
pair<int, int> GameMap::getBestMoveAwayFrom(Unit* mover, int targetX, int targetY,
                                             vector<Unit*>& units) {
    int bestX = mover->gridX;
    int bestY = mover->gridY;
    int bestDist = abs(mover->gridX - targetX) + abs(mover->gridY - targetY);

    // 遍历移动范围内的所有格子
    for (int dx = -mover->moveRange; dx <= mover->moveRange; dx++) {
        for (int dy = -mover->moveRange; dy <= mover->moveRange; dy++) {
            int newX = mover->gridX + dx;
            int newY = mover->gridY + dy;

            // 检查是否在地图范围内
            if (!isValidPosition(newX, newY)) continue;

            // 检查是否在移动范围内（曼哈顿距离）
            if (abs(dx) + abs(dy) > mover->moveRange) continue;

            // 检查格子是否为空（或者是自己的位置）
            if (isOccupied(newX, newY) &&
                !(newX == mover->gridX && newY == mover->gridY)) continue;

            // 计算到目标的距离
            int distToTarget = abs(newX - targetX) + abs(newY - targetY);

            // 如果更远，更新最佳位置
            if (distToTarget > bestDist) {
                bestDist = distToTarget;
                bestX = newX;
                bestY = newY;
            }
        }
    }

    return make_pair(bestX, bestY);
}

// 网格坐标转屏幕坐标
float GameMap::gridToScreenX(int gridX) {
    return gridX * TILE_SIZE;
}

float GameMap::gridToScreenY(int gridY) {
    return gridY * TILE_SIZE;
}

// 屏幕坐标转网格坐标
int GameMap::screenToGridX(float screenX) {
    return static_cast<int>(screenX / TILE_SIZE);
}

int GameMap::screenToGridY(float screenY) {
    return static_cast<int>(screenY / TILE_SIZE);
}
