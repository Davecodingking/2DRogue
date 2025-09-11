#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <ctime> 

// --- ���캯�� ---
Game::Game()
    : m_cameraX(0), m_cameraY(0), m_zoom(1.5f), m_isRunning(false), // ��΢�Ŵ�һ�㿴�ø����
    m_activeNpcCount(0), m_gameTimer(0.0f), m_npcSpawnTimer(0.0f)
{
    for (int i = 0; i < MAX_NPCS; ++i) {
        m_npcPool[i] = nullptr;
    }
    srand(static_cast<unsigned int>(time(0)));
}

// --- �������� ---
Game::~Game() {
    Shutdown();
}

// --- ��ʼ������ ---
bool Game::Initialize() {
    m_window.create(1280, 800, "2D Mech Rogue Game"); // ���ڿ����Դ�һЩ

    std::string levelFile = "Resources/level2.json";
    if (!m_level.loadFromFile(levelFile)) {
        std::cerr << "���عؿ�ʧ�ܣ������˳���" << std::endl;
        system("pause");
        return false;
    }
    if (levelFile == "Resources/level2.json") {
        m_level.setInfinite(true);
    }
    m_level.setZoom(m_zoom);

    // --- �޸ĵ� 1: �����µ� Load ���� ---
    if (!m_player.Load()) {
        std::cerr << "���������Դʧ��!" << std::endl;
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
        m_player.SetPosition(500, 500); // ����Ĭ��λ��
    }

    m_isRunning = true;
    return true;
}

// --- ��Ϸ��ѭ�� ---
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

// --- �ر�����Դ���� ---
void Game::Shutdown() {
    for (int i = 0; i < m_activeNpcCount; ++i) {
        if (m_npcPool[i]) {
            delete m_npcPool[i];
            m_npcPool[i] = nullptr;
        }
    }
    m_activeNpcCount = 0;
}

// --- ���봦�� ---
void Game::ProcessInput() {
    m_window.checkInput();
    if (m_window.keyPressed(VK_ESCAPE)) {
        m_isRunning = false;
    }

    // --- �������ָ����������Ź��� ---
    // getMouseWheel() ���ص������ϴε����������ۼ�ֵ������������Ҫ������
    int wheelDelta = m_window.getMouseWheel();
    if (wheelDelta != 0) {
        // ���ݹ��ַ���������ż���
        m_zoom += wheelDelta * 0.01f; // 0.001f ��һ�����������ӣ����Ե���

        // �������ŷ�Χ����ֹ����̫���̫С
        if (m_zoom < 0.5f) {
            m_zoom = 0.5f;
        }
        if (m_zoom > 1.0f) {
            m_zoom = 1.0f;
        }

        // ���¹ؿ��е�����ֵ��ȷ����ͼ��Ⱦͬ��
        m_level.setZoom(m_zoom);
    }
}

// --- ��Ϸ�߼����� ---
void Game::Update(float deltaTime) {
    if (!m_isRunning) return;

    // --- �޸ĵ� 2: ���� m_window ���ø���ҵ� Update ���� ---
    m_player.Update(m_level, deltaTime, m_window);

    UpdateNPCs(deltaTime);

    m_cameraX = (int)(m_player.getX() - (m_window.getWidth() / 2.0f / m_zoom) + (m_player.getWidth() / 2));
    m_cameraY = (int)(m_player.getY() - (m_window.getHeight() / 2.0f / m_zoom) + (m_player.getHeight() / 2));
    m_level.setCameraPosition(m_cameraX, m_cameraY);
}

// --- NPC�߼����� (��������) ---
void Game::UpdateNPCs(float deltaTime) {
    for (int i = 0; i < m_activeNpcCount; ++i) {
        if (m_npcPool[i] && m_npcPool[i]->getIsAlive()) {
            m_npcPool[i]->MoveTowards(m_player.getX(), m_player.getY());
            m_npcPool[i]->Update(m_level, deltaTime);
        }
    }
}

// --- ��Ⱦ ---
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

// --- NPC���ɺ��� ---
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

