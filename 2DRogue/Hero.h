#pragma once
#include "Character.h"
#include "GamesEngineeringBase.h"
#include <string>
#include <vector> // 需要 vector 来存储动画序列

class Hero : public Character {
public:
    Hero();
    ~Hero();

    // --- 核心功能 ---
    // 新的加载函数，不再需要文件名，它会自动加载所有必需的动画资源
    bool Load();
    void SetPosition(int startX, int startY);

    // --- 重写 (Override) 基类的虚函数 ---
    // Update 函数现在需要 Window 的引用，以便获取鼠标坐标
    void Update(Level& level, float deltaTime, GamesEngineeringBase::Window& window);
    void Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) override;

    // 为了让 Game 类能够传递 window 引用，我们需要重载基类的 Update
    // 但基类没有接收 window 的版本，所以我们在这里隐藏它
    // 注意：这不是最佳实践，但在当前框架下是最直接的修改方式
    void Update(Level& level, float deltaTime) override {
        // 这个函数体可以留空，因为我们总是会调用下面那个带 window 的版本
    }


private:
    // --- 私有成员变量 ---
    // 下半身（腿部）动画资源
    std::vector<GamesEngineeringBase::Image> m_legAnimations; // 存储8个方向的序列图
    int m_currentLegDirection;    // 当前腿部朝向 (0-7)
    float m_currentFrame;         // 当前动画帧 (使用 float 以便平滑控制速度)
    float m_animationSpeed;       // 动画播放速度（帧/秒）
    bool m_isMoving;              // 标记玩家是否正在移动

    // 上半身（躯干）瞄准资源
    std::vector<GamesEngineeringBase::Image> m_torsoImages; // 存储32个方向的瞄准图
    int m_currentTorsoFrame;      // 当前躯干朝向 (0-31)
    int m_torsoOffsetY;           // 新增：Y轴偏移量，用于调整上半身位置

    // 动画帧尺寸
    int m_frameWidth;
    int m_frameHeight;
    const int TILE_SIZE = 32;     // 地图图块大小

    // --- 私有辅助函数 ---
    void HandleInput();
    void CheckMapCollision(Level& level, int newX, int newY);
    // 核心：更新动画状态（包括移动和瞄准）
    void UpdateAnimations(float deltaTime, GamesEngineeringBase::Window& window, int cameraX, int cameraY, float zoom);
    // 核心：渲染一个指定的动画帧
    void RenderFrame(GamesEngineeringBase::Window& canvas, GamesEngineeringBase::Image& spriteSheet, int frameIndex, int screenX, int screenY, float zoom);
};

