#pragma once
#include "Character.h"
#include "GamesEngineeringBase.h"
#include <string>
#include <vector> // ��Ҫ vector ���洢��������

class Hero : public Character {
public:
    Hero();
    ~Hero();

    // --- ���Ĺ��� ---
    // �µļ��غ�����������Ҫ�ļ����������Զ��������б���Ķ�����Դ
    bool Load();
    void SetPosition(int startX, int startY);

    // --- ��д (Override) ������麯�� ---
    // Update ����������Ҫ Window �����ã��Ա��ȡ�������
    void Update(Level& level, float deltaTime, GamesEngineeringBase::Window& window);
    void Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) override;

    // Ϊ���� Game ���ܹ����� window ���ã�������Ҫ���ػ���� Update
    // ������û�н��� window �İ汾����������������������
    // ע�⣺�ⲻ�����ʵ�������ڵ�ǰ���������ֱ�ӵ��޸ķ�ʽ
    void Update(Level& level, float deltaTime) override {
        // ���������������գ���Ϊ�������ǻ���������Ǹ��� window �İ汾
    }


private:
    // --- ˽�г�Ա���� ---
    // �°����Ȳ���������Դ
    std::vector<GamesEngineeringBase::Image> m_legAnimations; // �洢8�����������ͼ
    int m_currentLegDirection;    // ��ǰ�Ȳ����� (0-7)
    float m_currentFrame;         // ��ǰ����֡ (ʹ�� float �Ա�ƽ�������ٶ�)
    float m_animationSpeed;       // ���������ٶȣ�֡/�룩
    bool m_isMoving;              // �������Ƿ������ƶ�

    // �ϰ������ɣ���׼��Դ
    std::vector<GamesEngineeringBase::Image> m_torsoImages; // �洢32���������׼ͼ
    int m_currentTorsoFrame;      // ��ǰ���ɳ��� (0-31)
    int m_torsoOffsetY;           // ������Y��ƫ���������ڵ����ϰ���λ��

    // ����֡�ߴ�
    int m_frameWidth;
    int m_frameHeight;
    const int TILE_SIZE = 32;     // ��ͼͼ���С

    // --- ˽�и������� ---
    void HandleInput();
    void CheckMapCollision(Level& level, int newX, int newY);
    // ���ģ����¶���״̬�������ƶ�����׼��
    void UpdateAnimations(float deltaTime, GamesEngineeringBase::Window& window, int cameraX, int cameraY, float zoom);
    // ���ģ���Ⱦһ��ָ���Ķ���֡
    void RenderFrame(GamesEngineeringBase::Window& canvas, GamesEngineeringBase::Image& spriteSheet, int frameIndex, int screenX, int screenY, float zoom);
};

