#pragma once
#include "Character.h"
#include "GamesEngineeringBase.h"

// Forward declaration
class Level;

class Hero : public Character {
public:
    // --- 修改: 移除 LASER, 它现在是一个独立的技能 ---
    enum class WeaponType { MACHINE_GUN, CANNON };

    Hero();
    ~Hero();

    bool Load();
    void Update(Level& level, float deltaTime);
    void Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom);

    void UpdateAiming(GamesEngineeringBase::Window& window, int cameraX, int cameraY, float zoom);
    void CheckMapCollision(Level& level, float newX, float newY);
    void SetSlowed(bool slowed);
    void SwitchWeapon();
    void ResetFireCooldown();

    bool CanFire();
    float getAimAngle() const { return m_aimAngle; }
    WeaponType GetCurrentWeapon() const { return m_currentWeapon; }

    // Get the logical position from where projectiles should be fired
    float GetFirePosX() const;
    float GetFirePosY() const;

    // --- 新增: 激光技能相关函数 ---
    void AddLaserCharges(int amount);
    void UseLaserCharge();
    int GetLaserCharges() const;
    float m_laserCooldown; // 将激光冷却时间设为公开，方便Game类访问

private:
    void HandleInput(Level& level, float deltaTime);

    // Animations
    GamesEngineeringBase::Image m_legAnimations[8];
    GamesEngineeringBase::Image m_torsoAnimations[32];

    int m_legDirection;
    int m_torsoDirection;
    float m_currentFrame;
    float m_animationSpeed;

    // Aiming & Positioning
    float m_aimAngle;
    float m_renderScale;
    float m_torsoOffsetX;
    float m_torsoOffsetY;

    // State
    bool m_isMoving;
    bool m_isSlowed;

    // Weapon
    WeaponType m_currentWeapon;
    float m_machineGunCooldown;
    float m_cannonCooldown;
    float m_fireCooldown;

    // --- 新增: 激光充能次数 ---
    int m_laserCharges;
};

