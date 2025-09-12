#pragma once
#include "Character.h"
#include "GamesEngineeringBase.h"

// Forward declaration to avoid circular dependency
class Level;

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

    // --- FUNCTION MOVED TO PUBLIC ---
    void MoveTowards(Level& level, float targetX, float targetY, float deltaTime);

    State getCurrentState() const { return m_currentState; }
    NPCType getNPCType() const { return m_type; }

    bool canFire();
    void resetFireCooldown();

    // --- Static Asset Management ---
    static bool LoadSharedAssets();
    static void FreeSharedAssets();

private:
    void CheckMapCollision(Level& level, float& newX, float& newY);

    State m_currentState;
    NPCType m_type;

    // Animation assets
    GamesEngineeringBase::Image m_walkAnimationSheet;
    GamesEngineeringBase::Image m_explodeAnimationSheet;
    GamesEngineeringBase::Image m_sniperAnimationSheets[8];
    GamesEngineeringBase::Image m_bossImage;

    // Shared asset for special explosion
    static GamesEngineeringBase::Image s_specialExplosionSheet;
    static bool s_assetsLoaded;

    // Animation properties
    int m_frameCountWalk;
    int m_frameCountExplode;
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

