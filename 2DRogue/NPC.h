#pragma once
#include "Character.h"
#include "GamesEngineeringBase.h" // <-- ¹Ø¼üÐÞÕý
#include <string>

class NPC : public Character {
public:
    enum State { WALKING, DYING, DEAD, SHOOTING };
    enum NPCType { MELEE, SHOOTER };

    NPC(NPCType type = MELEE);
    ~NPC();

    bool Load();

    void Update(Level& level, float deltaTime) override;
    void Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) override;
    void TakeDamage(int damage) override;

    void InitializeStats(int health, float speed, float renderScale = 1.0f);

    // AI Methods
    void UpdateAI(float targetX, float targetY, float deltaTime);


    State getCurrentState() const { return m_currentState; }
    NPCType getNPCType() const { return m_type; }
    bool canFire();
    void resetFireCooldown();

private:
    void MoveTowards(float targetX, float targetY, float deltaTime);

    GamesEngineeringBase::Image m_walkAnimationSheet;
    GamesEngineeringBase::Image m_explodeAnimationSheet;

    State m_currentState;
    NPCType m_type;
    int m_frameCountWalk;
    int m_frameCountExplode;
    float m_currentFrame;
    float m_animationSpeed;
    float m_explodeAnimationSpeed;
    float m_renderScale;

    // AI variables
    float m_preferredDistance;
    float m_fireCooldown;
    float m_fireRate;
};

