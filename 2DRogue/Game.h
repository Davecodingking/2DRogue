#pragma once

#include "Hero.h"
#include "Level.h"
#include "NPC.h"
#include "Projectile.h"
#include "GamesEngineeringBase.h"
#include <string>

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
    void RenderUI(); // Added for weapon indicator

    void UpdateNPCs(float deltaTime);
    void UpdateProjectiles(float deltaTime);
    void UpdateSpawning(float deltaTime);
    void CheckCollisions();

    void SpawnNPC(int x, int y, NPC::NPCType type);
    // FIX: Updated function declaration to accept angle
    void SpawnProjectile(float startX, float startY, float angle, Projectile::Type type, Projectile::Owner owner);

    GamesEngineeringBase::Window m_window;
    Level m_level;
    Hero m_player;

    static const int MAX_NPCS = 50;
    NPC* m_npcPool[MAX_NPCS];
    int m_activeNpcCount;

    static const int MAX_PROJECTILES = 100;
    Projectile m_projectilePool[MAX_PROJECTILES];
    int m_activeProjectileCount;

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

