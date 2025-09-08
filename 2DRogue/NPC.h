#pragma once
#include "Character.h"
#include "GamesEngineeringBase.h"
#include <string>

// NPC �࣬ͬ���̳��� Character
class NPC : public Character {
public:
    // ���캯��
    NPC();
    // ��������
    ~NPC();

    // ����NPC������Դ�����ͼƬ��
    bool Load(const std::string& filename);

    // ����NPC�ĳ�ʼλ��
    void SetPosition(int startX, int startY);

    // ��ʼ��NPC�����ԣ�����ֵ���ٶȵȣ�
    void InitializeStats(int health, float speed);

    // --- ��д (Override) ������麯�� ---
    void Update(Level& level, float deltaTime) override;
    void Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) override;

    // --- NPC ���е���Ϊ ---
    // AI �߼����� NPC ����һ��Ŀ�������ƶ�
    void MoveTowards(int targetX, int targetY);

private:
    // NPC�����ͼƬ
    GamesEngineeringBase::Image npcImage;
    const int TILE_SIZE = 32;
};
