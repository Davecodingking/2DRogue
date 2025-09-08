#pragma once
#include <string>
#include "GamesEngineeringBase.h"

// GameObject 结构体，用于存储 Tiled 中的对象信息
struct GameObject {
    std::string type;
    int x;
    int y;
};

class Level {
public:
    Level();
    ~Level();

    // 从 Tiled 导出的 .json 文件加载关卡数据
    bool loadFromFile(const std::string& filename);
    // 渲染关卡
    void render(GamesEngineeringBase::Window& canvas);

    // 获取地图宽度（单位：图块）
    int getWidth() const;
    // 获取地图高度（单位：图块）
    int getHeight() const;
    // 获取游戏对象数量
    int getObjectCount() const;
    // 获取游戏对象数组的指针
    const GameObject* getGameObjects() const;
    // 检查指定坐标的图块是否为障碍物
    bool isObstacleAt(int tx, int ty) const;

    // 获取当前的缩放级别
    float getZoom() const;

    // 设置摄像机的世界坐标
    void setCameraPosition(int x, int y);
    // 设置渲染的缩放级别
    void setZoom(float zoomLevel);

private:
    int mapWidth;
    int mapHeight;
    int cameraX;
    int cameraY;
    float zoom; // 缩放比例

    // 各图层的图块数据
    int* backgroundData;
    int* roadData;
    int* obstaclesData;
    int* debrisData;

    // 游戏对象数据
    GameObject* gameObjects;
    int objectCount;

    // 图块集图像
    GamesEngineeringBase::Image tilesetImage;

    // 释放内存的辅助函数
    void cleanup();

    // 解析JSON的辅助函数
    bool findIntValueInSubstring(const std::string& content, const std::string& key, int& outValue);
    bool findStringValueInSubstring(const std::string& content, const std::string& key, std::string& outValue);
};

