#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <ctime> 
#include <cmath> // For abs()

// --- 构造函数 ---
Game::Game()
    : m_cameraX(0), m_cameraY(0), m_zoom(1.5f), m_isRunning(false),
    m_activeNpcCount(0), m_gameTimer(0.0f),
    m_spawnPointCount(0), m_waveSpawnTimer(WAVE_INTERVAL),
    m_npcsToSpawn(2), m_playerDamageCooldown(0.0f)
{
    for (int i = 0; i < MAX_NPCS; ++i) {
        m_npcPool[i] = nullptr;
    }
    for (int i = 0; i < MAX_SPAWN_POINTS; ++i) {
        m_npcSpawnPoints[i] = { 0, 0 };
    }
    srand(static_cast<unsigned int>(time(0)));
}

// --- 析构函数 ---
Game::~Game() {
    Shutdown();
}

// --- 初始化函数 ---
bool Game::Initialize() {
    m_window.create(1024, 768, "2D Mech Rogue Game");

    std::string levelFile = "Resources/level1.json";
    if (!m_level.loadFromFile(levelFile)) {
        std::cerr << "加载关卡失败，程序退出。" << std::endl;
        system("pause");
        return false;
    }
    m_level.setInfinite(false);
    m_level.setZoom(m_zoom);

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
            if (m_spawnPointCount < MAX_SPAWN_POINTS) {
                m_npcSpawnPoints[m_spawnPointCount].x = obj.x;
                m_npcSpawnPoints[m_spawnPointCount].y = obj.y;
                m_spawnPointCount++;
            }
        }
    }

    if (!playerSpawnPointFound) {
        m_player.SetPosition(1378, 2106);
    }

    m_isRunning = true;
    return true;
}

// --- 游戏主循环 ---
void Game::Run() {
    GamesEngineeringBase::Timer timer;
    while (m_isRunning) {
        float deltaTime = timer.dt();
        if (deltaTime > 0.1f) deltaTime = 0.1f;

        m_gameTimer += deltaTime;

        ProcessInput();
        Update(deltaTime);
        Render();
    }
}

void Game::Shutdown() {
    for (int i = 0; i < m_activeNpcCount; ++i) {
        if (m_npcPool[i]) {
            delete m_npcPool[i];
            m_npcPool[i] = nullptr;
        }
    }
    m_activeNpcCount = 0;
    // --- 恢复的调试语句 ---
    std::cout << "游戏关闭，NPC资源已清理。" << std::endl;
}

void Game::ProcessInput() {
    m_window.checkInput();
    if (m_window.keyPressed(VK_ESCAPE)) {
        m_isRunning = false;
    }

    int wheelDelta = m_window.getMouseWheel();
    if (wheelDelta != 0) {
        m_zoom += wheelDelta * 0.001f;
        if (m_zoom < 0.5f) m_zoom = 0.5f;
        if (m_zoom > 3.0f) m_zoom = 3.0f;
        m_level.setZoom(m_zoom);
    }

    static bool wasMouseDown = false;
    bool isMouseDown = m_window.mouseButtonPressed(GamesEngineeringBase::MouseButton::MouseLeft);

    if (isMouseDown && !wasMouseDown) {
        if (m_activeNpcCount > 0) {
            int targetIndex = -1;
            int attempts = 0;
            while (targetIndex == -1 && attempts < m_activeNpcCount) {
                int randomIndex = rand() % m_activeNpcCount;
                if (m_npcPool[randomIndex] && m_npcPool[randomIndex]->getCurrentState() == NPC::State::WALKING) {
                    targetIndex = randomIndex;
                }
                attempts++;
            }
            if (targetIndex != -1) {
                m_npcPool[targetIndex]->TakeDamage(100);
            }
        }
    }
    wasMouseDown = isMouseDown;
}


void Game::CheckCollisions() {
    m_player.SetSlowed(false);

    float heroX = m_player.getX();
    float heroY = m_player.getY();
    float heroW = m_player.getWidth();
    float heroH = m_player.getHeight();

    for (int i = 0; i < m_activeNpcCount; i++) {
        if (m_npcPool[i] && m_npcPool[i]->getCurrentState() == NPC::State::WALKING) {
            float npcX = m_npcPool[i]->getX();
            float npcY = m_npcPool[i]->getY();
            float npcW = m_npcPool[i]->getWidth();
            float npcH = m_npcPool[i]->getHeight();

            if (heroX < npcX + npcW && heroX + heroW > npcX &&
                heroY < npcY + npcH && heroY + heroH > npcY)
            {
                m_player.SetSlowed(true);
            }
        }
    }
}

// --- 游戏逻辑更新 ---
void Game::Update(float deltaTime) {
    if (!m_isRunning) return;

    UpdateSpawning(deltaTime);

    m_player.Update(m_level, deltaTime);

    UpdateNPCs(deltaTime);

    CheckCollisions();

    m_cameraX = (int)(m_player.getX() - (m_window.getWidth() / 2.0f / m_zoom) + (m_player.getWidth() / 2));
    m_cameraY = (int)(m_player.getY() - (m_window.getHeight() / 2.0f / m_zoom) + (m_player.getHeight() / 2));

    m_player.UpdateAiming(m_window, m_cameraX, m_cameraY, m_zoom);

    if (!m_level.isInfiniteMode()) {
        int mapWidthPixels = m_level.getWidth() * 32;
        int mapHeightPixels = m_level.getHeight() * 32;
        int cameraViewWidth = static_cast<int>(m_window.getWidth() / m_zoom);
        int cameraViewHeight = static_cast<int>(m_window.getHeight() / m_zoom);

        if (m_cameraX < 0) m_cameraX = 0;
        if (m_cameraX > mapWidthPixels - cameraViewWidth) m_cameraX = mapWidthPixels - cameraViewWidth;
        if (m_cameraY < 0) m_cameraY = 0;
        if (m_cameraY > mapHeightPixels - cameraViewHeight) m_cameraY = mapHeightPixels - cameraViewHeight;
    }

    m_level.setCameraPosition(m_cameraX, m_cameraY);
}

void Game::UpdateNPCs(float deltaTime) {
    for (int i = 0; i < m_activeNpcCount; ++i) {
        if (m_npcPool[i]) {
            if (m_npcPool[i]->getCurrentState() == NPC::State::WALKING) {
                m_npcPool[i]->MoveTowards(m_player.getX(), m_player.getY(), deltaTime);
            }
            m_npcPool[i]->Update(m_level, deltaTime);
        }
    }

    for (int i = m_activeNpcCount - 1; i >= 0; --i) {
        if (m_npcPool[i] && m_npcPool[i]->getCurrentState() == NPC::State::DEAD) {
            delete m_npcPool[i];
            m_npcPool[i] = m_npcPool[m_activeNpcCount - 1];
            m_npcPool[m_activeNpcCount - 1] = nullptr;
            m_activeNpcCount--;
        }
    }
}

void Game::UpdateSpawning(float deltaTime) {
    if (m_spawnPointCount == 0) return;

    m_waveSpawnTimer -= deltaTime;
    if (m_waveSpawnTimer <= 0) {
        for (int i = 0; i < m_npcsToSpawn; ++i) {
            if (m_activeNpcCount >= MAX_NPCS) break;
            int spawnIndex = rand() % m_spawnPointCount;
            int spawnX = m_npcSpawnPoints[spawnIndex].x;
            int spawnY = m_npcSpawnPoints[spawnIndex].y;

            SpawnNPC(spawnX, spawnY, 100, 80.0f);
        }

        if (m_npcsToSpawn < 8) {
            m_npcsToSpawn++;
        }

        m_waveSpawnTimer = WAVE_INTERVAL;
    }
}

void Game::Render() {
    m_window.clear();
    m_level.render(m_window);

    for (int i = 0; i < m_activeNpcCount; ++i) {
        if (m_npcPool[i]) {
            m_npcPool[i]->Render(m_window, m_cameraX, m_cameraY, m_zoom);
        }
    }

    m_player.Render(m_window, m_cameraX, m_cameraY, m_zoom);
    m_window.present();
}

void Game::SpawnNPC(int x, int y, int health, float speed) {
    if (m_activeNpcCount >= MAX_NPCS) {
        // --- 恢复的调试语句 ---
        std::cout << "NPC池已满，无法生成新的NPC。" << std::endl;
        return;
    }

    NPC* newNpc = new NPC();
    if (newNpc->Load()) {
        newNpc->SetPosition(x, y);
        newNpc->InitializeStats(health, speed);
        m_npcPool[m_activeNpcCount] = newNpc;
        m_activeNpcCount++;
    }
    else {
        delete newNpc;
        std::cerr << "SpawnNPC failed because resource loading failed." << std::endl;
    }
}

