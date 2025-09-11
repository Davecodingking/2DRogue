#pragma once

#include "Hero.h"
#include "Level.h"
#include "NPC.h"
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
    void UpdateSpawning(float deltaTime);
    void SpawnNPC(int x, int y, int health, float speed);
    void CheckCollisions();

    GamesEngineeringBase::Window m_window;
    Level m_level;
    Hero m_player;

    // NPC 池
    static const int MAX_NPCS = 20;
    NPC* m_npcPool[MAX_NPCS];
    int m_activeNpcCount;

    // 刷怪点
    struct SpawnPoint {
        int x;
        int y;
    };
    static const int MAX_SPAWN_POINTS = 10;
    SpawnPoint m_npcSpawnPoints[MAX_SPAWN_POINTS];
    int m_spawnPointCount;

    // 刷怪逻辑
    float m_waveSpawnTimer;
    static constexpr float WAVE_INTERVAL = 10.0f;
    int m_npcsToSpawn;

    // 游戏状态和计时器
    float m_gameTimer;
    float m_playerDamageCooldown; // <-- 关键修正：在这里补上声明

    // 摄像机
    int m_cameraX;
    int m_cameraY;
    float m_zoom;

    bool m_isRunning;
};

