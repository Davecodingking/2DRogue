#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <ctime> 

// --- 构造函数 ---
Game::Game()
    : m_cameraX(0), m_cameraY(0), m_zoom(1.5f), m_isRunning(false), // 稍微放大一点看得更清楚
    m_activeNpcCount(0), m_gameTimer(0.0f), m_npcSpawnTimer(0.0f)
{
    for (int i = 0; i < MAX_NPCS; ++i) {
        m_npcPool[i] = nullptr;
    }
    srand(static_cast<unsigned int>(time(0)));
}

// --- 析构函数 ---
Game::~Game() {
    Shutdown();
}

// --- 初始化函数 ---
bool Game::Initialize() {
    m_window.create(1280, 800, "2D Mech Rogue Game"); // 窗口可以稍大一些

    std::string levelFile = "Resources/level2.json";
    if (!m_level.loadFromFile(levelFile)) {
        std::cerr << "加载关卡失败，程序退出。" << std::endl;
        system("pause");
        return false;
    }
    if (levelFile == "Resources/level2.json") {
        m_level.setInfinite(true);
    }
    m_level.setZoom(m_zoom);

    // --- 修改点 1: 调用新的 Load 函数 ---
    if (!m_player.Load()) {
        std::cerr << "加载玩家资源失败!" << std::endl;
        system("pause");
        return false;
    }

    const GameObject* objects = m_level.getGameObjects();
    bool playerSpawnPointFound = false;
    for (int i = 0; i < m_level.getObjectCount(); ++i) {
        const GameObject& obj = objects[i];

        if (obj.type == "hero_respawn") {
            m_player.SetPosition(obj.x, obj.y);
            playerSpawnPointFound = true;
        }
        else if (obj.type == "generic_npc_respawn") {
            SpawnNPC(obj.x, obj.y, "Resources/A.png", 50, 2.0f);
        }
        else if (obj.type == "boss_npc_respawn") {
            SpawnNPC(obj.x, obj.y, "Resources/A.png", 150, 2.5f);
        }
    }
    if (!playerSpawnPointFound) {
        m_player.SetPosition(500, 500); // 给个默认位置
    }

    m_isRunning = true;
    return true;
}

// --- 游戏主循环 ---
void Game::Run() {
    GamesEngineeringBase::Timer timer;

    while (m_isRunning) {
        float deltaTime = timer.dt();

        m_gameTimer += deltaTime;
        m_npcSpawnTimer += deltaTime;

        ProcessInput();
        Update(deltaTime);
        Render();
    }
}

// --- 关闭与资源清理 ---
void Game::Shutdown() {
    for (int i = 0; i < m_activeNpcCount; ++i) {
        if (m_npcPool[i]) {
            delete m_npcPool[i];
            m_npcPool[i] = nullptr;
        }
    }
    m_activeNpcCount = 0;
}

// --- 输入处理 ---
void Game::ProcessInput() {
    m_window.checkInput();
    if (m_window.keyPressed(VK_ESCAPE)) {
        m_isRunning = false;
    }

    // --- 新增：恢复鼠标滚轮缩放功能 ---
    // getMouseWheel() 返回的是自上次调用以来的累计值，所以我们需要处理它
    int wheelDelta = m_window.getMouseWheel();
    if (wheelDelta != 0) {
        // 根据滚轮方向调整缩放级别
        m_zoom += wheelDelta * 0.01f; // 0.001f 是一个灵敏度因子，可以调整

        // 限制缩放范围，防止缩得太大或太小
        if (m_zoom < 0.5f) {
            m_zoom = 0.5f;
        }
        if (m_zoom > 1.0f) {
            m_zoom = 1.0f;
        }

        // 更新关卡中的缩放值，确保地图渲染同步
        m_level.setZoom(m_zoom);
    }
}

// --- 游戏逻辑更新 ---
void Game::Update(float deltaTime) {
    if (!m_isRunning) return;

    // --- 修改点 2: 传递 m_window 引用给玩家的 Update 函数 ---
    m_player.Update(m_level, deltaTime, m_window);

    UpdateNPCs(deltaTime);

    m_cameraX = (int)(m_player.getX() - (m_window.getWidth() / 2.0f / m_zoom) + (m_player.getWidth() / 2));
    m_cameraY = (int)(m_player.getY() - (m_window.getHeight() / 2.0f / m_zoom) + (m_player.getHeight() / 2));
    m_level.setCameraPosition(m_cameraX, m_cameraY);
}

// --- NPC逻辑更新 (独立函数) ---
void Game::UpdateNPCs(float deltaTime) {
    for (int i = 0; i < m_activeNpcCount; ++i) {
        if (m_npcPool[i] && m_npcPool[i]->getIsAlive()) {
            m_npcPool[i]->MoveTowards(m_player.getX(), m_player.getY());
            m_npcPool[i]->Update(m_level, deltaTime);
        }
    }
}

// --- 渲染 ---
void Game::Render() {
    m_window.clear();
    m_level.render(m_window);
    m_player.Render(m_window, m_cameraX, m_cameraY, m_zoom);

    for (int i = 0; i < m_activeNpcCount; ++i) {
        if (m_npcPool[i] && m_npcPool[i]->getIsAlive()) {
            m_npcPool[i]->Render(m_window, m_cameraX, m_cameraY, m_zoom);
        }
    }

    m_window.present();
}

// --- NPC生成函数 ---
void Game::SpawnNPC(int x, int y, const std::string& spritePath, int health, float speed) {
    if (m_activeNpcCount >= MAX_NPCS) {
        return;
    }

    NPC* newNpc = new NPC();
    if (newNpc->Load(spritePath)) {
        newNpc->SetPosition(x, y);
        newNpc->InitializeStats(health, speed);
        m_npcPool[m_activeNpcCount] = newNpc;
        m_activeNpcCount++;
    }
    else {
        delete newNpc;
    }
}

