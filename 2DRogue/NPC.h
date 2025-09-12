#pragma once
#include "Character.h"
#include "GamesEngineeringBase.h"

class NPC : public Character {
public:
    enum NPCType { MELEE, SHOOTER, SNIPER, BOSS_AIRCRAFT };
    enum State { WALKING, SHOOTING, DYING, DEAD };

    NPC(NPCType type);
    ~NPC();

    bool Load();
    void InitializeStats(int health, float speed, float renderScale);

    void Update(Level& level, float deltaTime) override;
    void UpdateAI(float targetX, float targetY, float deltaTime);
    void Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) override;
    void TakeDamage(int damage) override;

    State getCurrentState() const { return m_currentState; }
    NPCType getNPCType() const { return m_type; }

    bool canFire();
    void resetFireCooldown();

private:
    void MoveTowards(float targetX, float targetY, float deltaTime);

    State m_currentState;
    NPCType m_type;

    // Animation assets
    GamesEngineeringBase::Image m_walkAnimationSheet;
    GamesEngineeringBase::Image m_explodeAnimationSheet;
    GamesEngineeringBase::Image m_sniperAnimationSheets[8];
    GamesEngineeringBase::Image m_bossImage;

    // Animation properties
    int m_frameCountWalk;
    int m_frameCountExplode;
    int m_frameCountSniper;
    float m_currentFrame;
    float m_animationSpeed;
    float m_explodeAnimationSpeed;
    float m_renderScale;
    int m_direction; // For 8-directional sprites

    // AI properties
    float m_preferredDistance;
    float m_fireCooldown;
    float m_fireRate;

    // Boss specific AI
    float m_bossAttackTimer;
};

