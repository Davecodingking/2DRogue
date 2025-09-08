#pragma once
#pragma once
#include "Character.h"
#include "GamesEngineeringBase.h"
#include <string>

// Hero ��̳��� Character ��
// "public Character" ��ζ�� Character �������е� public ��Ա�� Hero ����Ҳ�� public��
// protected ��Ա�� Hero ��Ҳ�� protected��
class Hero : public Character {
public:
    // ���캯��
    Hero();
    // ��������
    ~Hero();

    // --- ���Ĺ��� ---
    // ����Ӣ���������Դ������ͼƬ��
    bool Load(const std::string& filename);

    // ����Ӣ�۵ĳ�ʼλ��
    void SetPosition(int startX, int startY);

    // --- ��д (Override) ������麯�� ---
    // override �ؼ��ָ��߱���������������������д�����е�ͬ���麯����
    // ���������û������������ߺ���ǩ����ƥ�䣬�������ᱨ�����ܷ�ֹ�ܶ����
    void Update(Level& level, float deltaTime) override;
    void Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) override;

private:
    // --- ˽�г�Ա���� ---
    // Ӣ�۵����ͼƬ
    GamesEngineeringBase::Image playerImage;

    // ͼ��Ĵ�С��������ײ������
    const int TILE_SIZE = 32;

    // --- ˽�и������� ---
    // ר�Ŵ����������
    void HandleInput();
    // ר�Ŵ������ͼ����ײ
    void CheckMapCollision(Level& level, int newX, int newY);
};
