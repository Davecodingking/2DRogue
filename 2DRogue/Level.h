#pragma once
#include <string>
#include "GamesEngineeringBase.h" // 包含框架頭文件

// 用於儲存從 Tiled 加載的遊戲對象（如生成點）的數據結構
struct GameObject {
    std::string type;
    int x;
    int y;
};

class Level {
public:
    // 構造函數與析構函數
    Level();
    ~Level();

    // 從 Tiled 導出的 .json 文件加載關卡數據
    bool loadFromFile(const std::string& filename);

    // 根據攝像機位置渲染關卡
    void render(GamesEngineeringBase::Window& canvas);

    // --- 數據查詢 (Getters) ---
    int getWidth() const;
    int getHeight() const;
    int getObjectCount() const;
    const GameObject* getGameObjects() const;
    bool isObstacleAt(int tx, int ty) const; // 檢查指定圖塊座標是否是障礙物

    // --- 攝像機控制 ---
    void setCameraPosition(int x, int y);

private:
    // 地圖尺寸（以圖塊為單位）
    int mapWidth;
    int mapHeight;

    // 攝像機位置（以像素為單位）
    int cameraX;
    int cameraY;

    // 用於儲存圖塊數據的動態陣列
    int* backgroundData;
    int* roadData;
    int* obstaclesData;
    int* debrisData;

    // 用於儲存遊戲對象的動態陣列
    GameObject* gameObjects;
    int objectCount;

    // 用於儲存圖塊集圖片
    GamesEngineeringBase::Image tilesetImage;

    // 私有輔助函數
    void cleanup();
    bool findIntValue(const std::string& content, const std::string& key, int& outValue, size_t& searchPos);
    bool findFloatValue(const std::string& content, const std::string& key, float& outValue, size_t& searchPos);
    bool findStringValue(const std::string& content, const std::string& key, std::string& outValue, size_t& searchPos);
};

