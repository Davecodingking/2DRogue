#pragma once
#include "Character.h"
#include "GamesEngineeringBase.h"
#include <string>

class Hero : public Character {
public:
    enum class WeaponType { MACHINE_GUN, CANNON };

    Hero();
    ~Hero();

    bool Load();

    // --- Overrides from Character base class ---
    void Update(Level& level, float deltaTime) override;
    void Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) override;
    void TakeDamage(int damage) override;
    void SetPosition(float startX, float startY) override;


    // --- Hero specific functions ---
    void UpdateAiming(GamesEngineeringBase::Window& window, int cameraX, int cameraY, float zoom);
    void SetSlowed(bool slowed);
    float getAimAngle() const { return m_aimAngle; }

    // Weapon System
    bool CanFire();
    void ResetFireCooldown();
    void SwitchWeapon();
    WeaponType GetCurrentWeapon() const { return m_currentWeapon; }

    // �� Hero.h ������ͳһ�����ӿ�
    float GetFirePosX() const;
    float GetFirePosY() const;

    // ����: ��ȡ���ϰ�����һ�µ�����Ƕȣ����ȣ�
    float GetTorsoFireAngle() const;

private:
    void HandleInput(Level& level, float deltaTime);
    void CheckMapCollision(Level& level, float newX, float newY);

    // --- Animation & Rendering ---
    GamesEngineeringBase::Image m_legAnimations[8];
    GamesEngineeringBase::Image m_torsoAnimations[32];
    int m_legDirection;
    int m_torsoDirection;
    float m_currentFrame;
    float m_animationSpeed;
    float m_aimAngle;
    float m_renderScale;
    float m_torsoOffsetX;
    float m_torsoOffsetY;

    // --- State ---
    const int TILE_SIZE = 32;
    bool m_isMoving;
    bool m_isSlowed;

    // --- Weapon Variables ---
    WeaponType m_currentWeapon;
    float m_fireCooldown;
    float m_machineGunCooldown;
    float m_cannonCooldown;
};

