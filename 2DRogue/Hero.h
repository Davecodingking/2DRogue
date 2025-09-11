#pragma once
#include "Character.h"
#include "GamesEngineeringBase.h"
#include <string>

class Hero : public Character {
public:
    Hero();
    ~Hero();

    bool Load();

    // ��д������麯��
    void Update(Level& level, float deltaTime) override;
    void Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) override;
    void SetPosition(float x, float y) override;

    // ��������
    void UpdateAiming(GamesEngineeringBase::Window& window, int cameraX, int cameraY, float zoom);
    void SetSlowed(bool isSlowed);

private:
    // ������Դ
    GamesEngineeringBase::Image m_legsAnimationSheets[8];
    GamesEngineeringBase::Image m_torsoAnimationSheets[32];

    // ��������
    int m_currentLegsDirection;
    float m_currentFrame;
    float m_animationSpeed;
    int m_currentTorsoFrame;

    // ״̬
    bool m_isMoving;
    bool m_isSlowed;

    // ��Ⱦ����
    float m_renderScale;
    float m_torsoOffsetY;

    // �ڲ���������
    // --- �ؼ���������HandleInput���Խ���Level���� ---
    void HandleInput(Level& level, float deltaTime);
    void CheckMapCollision(Level& level, float newX, float newY);
};

