#pragma once
#include "Character.h"
#include "GamesEngineeringBase.h"
#include <string>

class Hero : public Character {
public:
    Hero();
    ~Hero();

    bool Load();

    // 重写基类的虚函数
    void Update(Level& level, float deltaTime) override;
    void Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) override;
    void SetPosition(float x, float y) override;

    // 公开函数
    void UpdateAiming(GamesEngineeringBase::Window& window, int cameraX, int cameraY, float zoom);
    void SetSlowed(bool isSlowed);

private:
    // 动画资源
    GamesEngineeringBase::Image m_legsAnimationSheets[8];
    GamesEngineeringBase::Image m_torsoAnimationSheets[32];

    // 动画控制
    int m_currentLegsDirection;
    float m_currentFrame;
    float m_animationSpeed;
    int m_currentTorsoFrame;

    // 状态
    bool m_isMoving;
    bool m_isSlowed;

    // 渲染参数
    float m_renderScale;
    float m_torsoOffsetY;

    // 内部辅助函数
    // --- 关键修正：让HandleInput可以接收Level对象 ---
    void HandleInput(Level& level, float deltaTime);
    void CheckMapCollision(Level& level, float newX, float newY);
};

