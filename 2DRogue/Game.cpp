#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <ctime> // <-- 新增：用于初始化随机数种子

// --- 构造函数 ---
Game::Game()
    : m_cameraX(0), m_cameraY(0), m_zoom(0.5f), m_isRunning(false),
    m_activeNpcCount(0), m_gameTimer(0.0f), m_npcSpawnTimer(0.0f)
{
    // 初始化NPC池
    for (int i = 0; i < MAX_NPCS; ++i) {
        m_npcPool[i] = nullptr;
    }
    // 初始化随机数种子，为以后随机事件做准备
    srand(static_cast<unsigned int>(time(0)));
}

// --- 析构函数 ---
Game::~Game() {
    Shutdown();
}

// --- 初始化函数 ---
bool Game::Initialize() {
    // 1. 创建窗口
    m_window.create(1024, 768, "2D Rogue Game");

    // 2. 加载关卡
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

    // 3. 加载玩家资源
    if (!m_player.Load("Resources/A.png")) {
        system("pause");
        return false;
    }

    // 4. --- 修改：根据地图数据设置玩家和NPC的初始位置 ---
    const GameObject* objects = m_level.getGameObjects();
    bool playerSpawnPointFound = false;
    for (int i = 0; i < m_level.getObjectCount(); ++i) {
        const GameObject& obj = objects[i];

        if (obj.type == "hero_respawn") {
            m_player.SetPosition(obj.x, obj.y);
            playerSpawnPointFound = true;
        }
        // --- 核心修改：在这里根据地图对象生成NPC ---
        else if (obj.type == "generic_npc_respawn") {
            // 生成一个普通怪 (暂时还使用 'A.png' 作为占位符)
            SpawnNPC(obj.x, obj.y, "Resources/A.png", 50, 2.0f);
        }
        else if (obj.type == "boss_npc_respawn") {
            // 生成一个精英怪 (血量更多，速度稍快)
            SpawnNPC(obj.x, obj.y, "Resources/A.png", 150, 2.5f);
        }
    }
    // 如果地图里没定义玩家出生点，给一个默认位置
    if (!playerSpawnPointFound) {
        m_player.SetPosition(100, 100);
    }

    m_isRunning = true;
    return true;
}

// --- 游戏主循环 ---
void Game::Run() {
    // 使用框架提供的计时器来获取帧间隔时间
    GamesEngineeringBase::Timer timer;

    while (m_isRunning) {
        float deltaTime = timer.dt(); // 获取自上一帧以来的时间（秒）

        // 更新计时器
        m_gameTimer += deltaTime;
        m_npcSpawnTimer += deltaTime;

        // 执行游戏逻辑
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
    std::cout << "游戏关闭，NPC资源已清理。" << std::endl;
}

// --- 输入处理 ---
void Game::ProcessInput() {
    m_window.checkInput();
    if (m_window.keyPressed(VK_ESCAPE)) {
        m_isRunning = false;
    }
}

// --- 游戏逻辑更新 ---
void Game::Update(float deltaTime) {
    if (!m_isRunning) return;

    // 1. 更新玩家
    m_player.Update(m_level, deltaTime);

    // 2. 更新所有NPC
    UpdateNPCs(deltaTime);

    // 3. 更新摄像机
    m_cameraX = (int)(m_player.getX() - (m_window.getWidth() / 2.0f / m_zoom) + (m_player.getWidth() / 2));
    m_cameraY = (int)(m_player.getY() - (m_window.getHeight() / 2.0f / m_zoom) + (m_player.getHeight() / 2));
    m_level.setCameraPosition(m_cameraX, m_cameraY);
}

// --- NPC逻辑更新 (独立函数) ---
void Game::UpdateNPCs(float deltaTime) {
    for (int i = 0; i < m_activeNpcCount; ++i) {
        if (m_npcPool[i] && m_npcPool[i]->getIsAlive()) {
            // AI 核心：让NPC朝玩家移动
            m_npcPool[i]->MoveTowards(m_player.getX(), m_player.getY());

            // 调用NPC自己的更新逻辑 (为以后扩展AI状态机)
            m_npcPool[i]->Update(m_level, deltaTime);
        }
    }
}


// --- 渲染 ---
void Game::Render() {
    m_window.clear();
    m_level.render(m_window);
    m_player.Render(m_window, m_cameraX, m_cameraY, m_zoom);

    // 渲染所有激活的NPC
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
        std::cout << "NPC池已满，无法生成新的NPC。" << std::endl;
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

