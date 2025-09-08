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
    void Update(float deltaTime); // <-- �������� deltaTime
    void Render();

    // NPC ������
    void SpawnNPC(int x, int y, const std::string& spritePath, int health, float speed);
    void UpdateNPCs(float deltaTime); // <-- �������� deltaTime

    GamesEngineeringBase::Window m_window;
    Level m_level;
    Hero m_player;

    // NPC ��
    static const int MAX_NPCS = 50;
    NPC* m_npcPool[MAX_NPCS];
    int m_activeNpcCount;

    // ��Ϸ״̬�ͼ�ʱ��
    float m_gameTimer;       // ��Ϸ�ܼ�ʱ��
    float m_npcSpawnTimer;   // NPC���ɼ�ʱ����Ϊδ�����������׼����

    int m_cameraX;
    int m_cameraY;
    float m_zoom;

    bool m_isRunning;
};

