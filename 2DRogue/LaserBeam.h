#pragma once
#include "GamesEngineeringBase.h"

// Forward declarations
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

    // Initialization flag
    bool m_isInitialized;
};

