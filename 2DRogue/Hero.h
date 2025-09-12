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
};
