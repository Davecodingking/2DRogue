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
    bool Initialize();
    void Run();
    void Shutdown();

private:
    void ProcessInput();
    void Update(float deltaTime);
    void Render();

    void UpdateNPCs(float deltaTime);
    void UpdateProjectiles(float deltaTime);
    void UpdateSpawning(float deltaTime);
    void CheckCollisions();

    // Spawning methods
    void SpawnNPC(int x, int y, NPC::NPCType type, int health, float speed, float renderScale);
    void SpawnProjectile(float startX, float startY, float dirX, float dirY, Projectile::Type type, Projectile::Owner owner);

    GamesEngineeringBase::Window m_window;
    Level m_level;
    Hero m_player;

    // --- Object Pools (No STL) ---
    static const int MAX_NPCS = 20;
    NPC* m_npcPool[MAX_NPCS];
    int m_activeNpcCount;

    static const int MAX_PROJECTILES = 100;
    Projectile m_projectilePool[MAX_PROJECTILES];
    int m_activeProjectileCount;

    // --- Spawning System ---
    struct SpawnPoint {
        int x;
        int y;
    };
    static const int MAX_SPAWN_POINTS = 10;
    SpawnPoint m_npcSpawnPoints[MAX_SPAWN_POINTS];
    int m_spawnPointCount;

    // --- Game State & Timers ---
    float m_gameTimer;
    float m_waveSpawnTimer;
    const float WAVE_INTERVAL = 10.0f;
    int m_npcsToSpawn;
    float m_playerDamageCooldown;

    // --- Camera & View ---
    int m_cameraX;
    int m_cameraY;
    float m_zoom;

    bool m_isRunning;
};

