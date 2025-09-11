#pragma once
#include "GamesEngineeringBase.h"

class Projectile {
public:
    enum Owner { PLAYER, ENEMY };
    enum Type { MACHINE_GUN, CANNON, ENEMY_BULLET };

    Projectile();

    void Activate(float startX, float startY, float dirX, float dirY, Type type, Owner owner);
    void Update(float deltaTime);
    void Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom);

    bool IsActive() const { return m_isActive; }
    void Deactivate() { m_isActive = false; }

    // Getters for collision detection
    float getX() const { return m_x; }
    float getY() const { return m_y; }
    float getWidth() const { return m_width; }
    float getHeight() const { return m_height; }
    Owner getOwner() const { return m_owner; }
    Type getType() const { return m_type; }

private:
    float m_x, m_y;
    float m_velX, m_velY;
    float m_width, m_height;
    int m_damage;
    float m_speed;
    bool m_isActive;

    Owner m_owner;
    Type m_type;

    // We don't need to load a unique image for each projectile instance,
    // so we'll just draw colored rectangles for now.
    // GamesEngineeringBase::Image m_image;
};

