#pragma once
#include "Character.h"
#include "GamesEngineeringBase.h"

// Forward declaration
class Level;

class Hero : public Character {
public:
    enum class WeaponType { MACHINE_GUN, CANNON };

    Hero();
    ~Hero();

    bool Load();
    void Update(Level& level, float deltaTime);
    void Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom);

    // ★★★ 错误修复: 将缺失的函数声明添加回来 ★★★
    void SetPosition(float newX, float newY);
    void TakeDamage(int damage);

    void UpdateAiming(GamesEngineeringBase::Window& window, int cameraX, int cameraY, float zoom);
    void SetSlowed(bool slowed);
    void SwitchWeapon();
    void ResetFireCooldown();
    bool CanFire();
    void RestoreFullHealth();

    WeaponType GetCurrentWeapon() const;
    float GetCurrentCooldownTimer() const;
    float GetCurrentMaxCooldown() const;
    float getAimAngle() const { return m_aimAngle; }
    float GetFirePosX() const;
    float GetFirePosY() const;
    void AddLaserCharges(int amount);
    void UseLaserCharge();
    int GetLaserCharges() const;

    float m_laserCooldown;
    float m_laserMaxCooldown;

private:
    void HandleInput(Level& level, float deltaTime);
    void CheckMapCollision(Level& level, float newX, float newY);

    // Animations & Rendering
    GamesEngineeringBase::Image m_legAnimations[8];
    GamesEngineeringBase::Image m_torsoAnimations[32];
    int m_legDirection;
    int m_torsoDirection;
    float m_currentFrame;
    float m_animationSpeed;
    float m_renderScale;
    float m_torsoOffsetX;
    float m_torsoOffsetY;
    float m_aimAngle;

    // State
    bool m_isMoving;
    bool m_isSlowed;

    // Weapon
    WeaponType m_currentWeapon;
    float m_machineGunCooldown;
    float m_cannonCooldown;
    float m_machineGunTimer;
    float m_cannonTimer;
    int m_laserCharges;
};

