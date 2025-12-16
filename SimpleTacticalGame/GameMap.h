#pragma once
#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <vector>
#include "Unit.h"

using namespace std;

// 地图常量
const int MAP_WIDTH = 8;    // 地图宽度（8格）
const int MAP_HEIGHT = 8;   // 地图高度（8格）
const int TILE_SIZE = 64;   // 每个格子的像素大小

// 游戏地图类
class GameMap {
public:
    GameMap();

    // 检查坐标是否在地图范围内
    bool isValidPosition(int x, int y);

    // 检查格子是否被占用
    bool isOccupied(int x, int y);

    // 根据单位列表更新占用状态
    void updateOccupancy(vector<Unit*>& units);

    // 查找某位置的单位
    Unit* getUnitAt(int x, int y, vector<Unit*>& units);

    // 计算从起点到终点的最佳移动位置（朝向目标移动）
    // 返回单位在移动范围内最接近目标的位置
    pair<int, int> getBestMoveTowards(Unit* mover, int targetX, int targetY,
                                       vector<Unit*>& units);

    // 计算远离目标的最佳移动位置
    pair<int, int> getBestMoveAwayFrom(Unit* mover, int targetX, int targetY,
                                        vector<Unit*>& units);

    // 将网格坐标转换为屏幕坐标
    float gridToScreenX(int gridX);
    float gridToScreenY(int gridY);

    // 将屏幕坐标转换为网格坐标
    int screenToGridX(float screenX);
    int screenToGridY(float screenY);

private:
    // 记录每个格子是否被占用
    bool occupied[MAP_WIDTH][MAP_HEIGHT];
};

#endif // GAMEMAP_H
