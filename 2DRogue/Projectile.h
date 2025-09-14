#pragma once
#include "GamesEngineeringBase.h"
#include <string>
#include <iostream>

class Projectile {
public:
    enum Owner { PLAYER, ENEMY };
    enum Type { MACHINE_GUN, CANNON, ENEMY_BULLET, ENEMY_SPHERE };
    enum State { INACTIVE, FLYING, EXPLODING };

    Projectile();

    void Activate(float startX, float startY, float angle, Type type, Owner owner);
    void Update(float deltaTime);
    void Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom);

    bool IsActive() const { return m_state != INACTIVE; }
    void Deactivate() { m_state = INACTIVE; }
    void StartExplosion();

    float getX() const { return m_x; }
    float getY() const { return m_y; }
    float getWidth() const { return m_width; }
    float getHeight() const { return m_height; }
    Owner getOwner() const { return m_owner; }
    Type getType() const { return m_type; }
    State GetState() const { return m_state; }

    static bool LoadAssets();
    static void FreeAssets();

private:
    float m_x, m_y;
    float m_velX, m_velY;
    float m_width, m_height;
    float m_angle;
    float m_renderScale;
    int m_damage;
    float m_speed;

    State m_state;
    Owner m_owner;
    Type m_type;

    float m_animationFrame;
    float m_animationSpeed;

    // Lifetime for projectile
    float m_lifeTimer;

    static GamesEngineeringBase::Image s_bulletImage;
    static GamesEngineeringBase::Image s_explodeSheet;
    static bool s_assetsLoaded;
};

