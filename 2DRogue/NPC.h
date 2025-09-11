#pragma once
#include "Character.h"
#include "GamesEngineeringBase.h"
#include <string>

class NPC : public Character {
public:
    enum class State {
        WALKING,
        DYING,
        DEAD
    };

    NPC();
    ~NPC();

    bool Load();
    void InitializeStats(int health, float speed);

    void Update(Level& level, float deltaTime) override;
    void Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) override;
    void SetPosition(float x, float y) override;

    void MoveTowards(float targetX, float targetY, float deltaTime);
    void TakeDamage(int damage) override;
    bool getIsAlive() const override;
    State getCurrentState() const;

private:
    GamesEngineeringBase::Image m_walkAnimationSheet;
    GamesEngineeringBase::Image m_explodeAnimationSheet;

    State m_currentState;
    float m_renderScale;

    // 动画控制
    float m_currentFrame;
    float m_animationSpeed;
    float m_explodeAnimationSpeed; // 新增：爆炸专用动画速度
    int m_walkFrames;
    int m_explodeFrames;
};

