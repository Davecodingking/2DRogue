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
    void Update(float deltaTime); // <-- 参数增加 deltaTime
    void Render();

    // NPC 管理函数
    void SpawnNPC(int x, int y, const std::string& spritePath, int health, float speed);
    void UpdateNPCs(float deltaTime); // <-- 参数增加 deltaTime

    GamesEngineeringBase::Window m_window;
    Level m_level;
    Hero m_player;

    // NPC 池
    static const int MAX_NPCS = 50;
    NPC* m_npcPool[MAX_NPCS];
    int m_activeNpcCount;

    // 游戏状态和计时器
    float m_gameTimer;       // 游戏总计时器
    float m_npcSpawnTimer;   // NPC生成计时器（为未来随机生成做准备）

    int m_cameraX;
    int m_cameraY;
    float m_zoom;

    bool m_isRunning;
};

