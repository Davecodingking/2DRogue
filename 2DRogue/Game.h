#pragma once
#include "Hero.h"
#include "Level.h"
#include "NPC.h"
#include "Projectile.h"
#include "LaserBeam.h"
#include "GamesEngineeringBase.h"
#include <string>

const int MAX_PICKUPS = 10;
const int MAX_HIT_NPCS = 50;

class Game {
public:
    Game();
    ~Game();
    bool Initialize(const std::string& levelFile);
    void Run();
    void Shutdown();

private:
    void ProcessInput();
    void Update(float deltaTime);
    void Render();
    void RenderUI();

    void UpdateNPCs(float deltaTime);
    void UpdateProjectiles(float deltaTime);
    void UpdateSpawning(float deltaTime);
    void UpdateLasers(float deltaTime);

    void CheckCollisions();
    void SpawnNPC(int x, int y, NPC::NPCType type);
    void SpawnProjectile(float startX, float startY, float angle, Projectile::Type type, Projectile::Owner owner);

    void FireLaser();
    void RenderPickups(float deltaTime);

    GamesEngineeringBase::Window m_window;
    Level m_level;
    Hero m_player;

    static const int MAX_NPCS = 50;
    NPC* m_npcPool[MAX_NPCS];
    int m_activeNpcCount;

    static const int MAX_PROJECTILES = 100;
    Projectile m_projectilePool[MAX_PROJECTILES];
    int m_activeProjectileCount;

    static const int MAX_LASERS = 10;
    LaserBeam m_laserPool[MAX_LASERS];
    int m_activeLaserCount;
    NPC* m_npcsHitThisFrame[MAX_HIT_NPCS];
    int m_npcsHitCount;

    struct Pickup {
        int x, y;
        bool isCollected = false;
        float floatOffset = 0.0f;
    };
    Pickup m_laserPickups[MAX_PICKUPS];
    int m_pickupCount;

    struct SpawnPoint { int x; int y; };
    static const int MAX_SPAWN_POINTS = 10;
    SpawnPoint m_npcSpawnPoints[MAX_SPAWN_POINTS];
    int m_spawnPointCount;
    SpawnPoint m_bossSpawnPoints[MAX_SPAWN_POINTS];
    int m_bossSpawnPointCount;

    float m_gameTimer;
    float m_playerDamageCooldown;

    int m_currentLevel;
    int m_currentWave;
    bool m_waveInProgress;
    float m_waveCooldownTimer;

    int m_level2_npcSpawnedCount;
    float m_level2_spawnTimer;
    bool m_bossSpawned;

    int m_cameraX;
    int m_cameraY;
    float m_zoom;
    bool m_isRunning;
};

