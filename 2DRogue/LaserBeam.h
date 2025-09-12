#pragma once
#include "GamesEngineeringBase.h"

// 前向声明
class NPC;
class Hero;

class LaserBeam {
public:
    LaserBeam();

    void Activate(Hero* owner, float targetX, float targetY, float delay);

    bool Update(float deltaTime);
    void Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom);
    bool IsActive() const { return m_isActive; }

    bool CheckCollision(NPC* npc);

    static bool LoadAssets(const std::string& filename);
    static void FreeAssets();

private:
    static GamesEngineeringBase::Image s_laserImage;

    bool m_isActive;
    float m_lifeTimer;

    Hero* m_owner;
    float m_actualStartX, m_actualStartY;

    float m_targetX, m_targetY;
    float m_angle;
    float m_length;
    float m_activateDelay;
    bool m_hasDamaged;

    // --- 新增: 初始化标志位 ---
    bool m_isInitialized;
};

