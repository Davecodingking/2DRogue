#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <ctime> // <-- ���������ڳ�ʼ�����������

// --- ���캯�� ---
Game::Game()
    : m_cameraX(0), m_cameraY(0), m_zoom(0.5f), m_isRunning(false),
    m_activeNpcCount(0), m_gameTimer(0.0f), m_npcSpawnTimer(0.0f)
{
    // ��ʼ��NPC��
    for (int i = 0; i < MAX_NPCS; ++i) {
        m_npcPool[i] = nullptr;
    }
    // ��ʼ����������ӣ�Ϊ�Ժ�����¼���׼��
    srand(static_cast<unsigned int>(time(0)));
}

// --- �������� ---
Game::~Game() {
    Shutdown();
}

// --- ��ʼ������ ---
bool Game::Initialize() {
    // 1. ��������
    m_window.create(1024, 768, "2D Rogue Game");

    // 2. ���عؿ�
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

    // 3. ���������Դ
    if (!m_player.Load("Resources/A.png")) {
        system("pause");
        return false;
    }

    // 4. --- �޸ģ����ݵ�ͼ����������Һ�NPC�ĳ�ʼλ�� ---
    const GameObject* objects = m_level.getGameObjects();
    bool playerSpawnPointFound = false;
    for (int i = 0; i < m_level.getObjectCount(); ++i) {
        const GameObject& obj = objects[i];

        if (obj.type == "hero_respawn") {
            m_player.SetPosition(obj.x, obj.y);
            playerSpawnPointFound = true;
        }
        // --- �����޸ģ���������ݵ�ͼ��������NPC ---
        else if (obj.type == "generic_npc_respawn") {
            // ����һ����ͨ�� (��ʱ��ʹ�� 'A.png' ��Ϊռλ��)
            SpawnNPC(obj.x, obj.y, "Resources/A.png", 50, 2.0f);
        }
        else if (obj.type == "boss_npc_respawn") {
            // ����һ����Ӣ�� (Ѫ�����࣬�ٶ��Կ�)
            SpawnNPC(obj.x, obj.y, "Resources/A.png", 150, 2.5f);
        }
    }
    // �����ͼ��û������ҳ����㣬��һ��Ĭ��λ��
    if (!playerSpawnPointFound) {
        m_player.SetPosition(100, 100);
    }

    m_isRunning = true;
    return true;
}

// --- ��Ϸ��ѭ�� ---
void Game::Run() {
    // ʹ�ÿ���ṩ�ļ�ʱ������ȡ֡���ʱ��
    GamesEngineeringBase::Timer timer;

    while (m_isRunning) {
        float deltaTime = timer.dt(); // ��ȡ����һ֡������ʱ�䣨�룩

        // ���¼�ʱ��
        m_gameTimer += deltaTime;
        m_npcSpawnTimer += deltaTime;

        // ִ����Ϸ�߼�
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
    std::cout << "��Ϸ�رգ�NPC��Դ������" << std::endl;
}

// --- ���봦�� ---
void Game::ProcessInput() {
    m_window.checkInput();
    if (m_window.keyPressed(VK_ESCAPE)) {
        m_isRunning = false;
    }
}

// --- ��Ϸ�߼����� ---
void Game::Update(float deltaTime) {
    if (!m_isRunning) return;

    // 1. �������
    m_player.Update(m_level, deltaTime);

    // 2. ��������NPC
    UpdateNPCs(deltaTime);

    // 3. ���������
    m_cameraX = (int)(m_player.getX() - (m_window.getWidth() / 2.0f / m_zoom) + (m_player.getWidth() / 2));
    m_cameraY = (int)(m_player.getY() - (m_window.getHeight() / 2.0f / m_zoom) + (m_player.getHeight() / 2));
    m_level.setCameraPosition(m_cameraX, m_cameraY);
}

// --- NPC�߼����� (��������) ---
void Game::UpdateNPCs(float deltaTime) {
    for (int i = 0; i < m_activeNpcCount; ++i) {
        if (m_npcPool[i] && m_npcPool[i]->getIsAlive()) {
            // AI ���ģ���NPC������ƶ�
            m_npcPool[i]->MoveTowards(m_player.getX(), m_player.getY());

            // ����NPC�Լ��ĸ����߼� (Ϊ�Ժ���չAI״̬��)
            m_npcPool[i]->Update(m_level, deltaTime);
        }
    }
}


// --- ��Ⱦ ---
void Game::Render() {
    m_window.clear();
    m_level.render(m_window);
    m_player.Render(m_window, m_cameraX, m_cameraY, m_zoom);

    // ��Ⱦ���м����NPC
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
        std::cout << "NPC���������޷������µ�NPC��" << std::endl;
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

