#include "Projectile.h"
#include <cmath>

Projectile::Projectile()
    : m_x(0), m_y(0), m_velX(0), m_velY(0),
    m_width(10), m_height(10), m_damage(0), m_speed(0),
    m_isActive(false), m_owner(PLAYER), m_type(MACHINE_GUN)
{
}

void Projectile::Activate(float startX, float startY, float dirX, float dirY, Type type, Owner owner) {
    m_isActive = true;
    m_x = startX;
    m_y = startY;
    m_owner = owner;
    m_type = type;

    switch (m_type) {
    case MACHINE_GUN:
        m_speed = 800.0f;
        m_damage = 10;
        m_width = 8;
        m_height = 8;
        // You might want to load a specific image here later
        break;
    case CANNON:
        m_speed = 500.0f;
        m_damage = 50;
        m_width = 16;
        m_height = 16;
        // You might want to load a specific image here later
        break;
    case ENEMY_BULLET:
        m_speed = 400.0f;
        m_damage = 10;
        m_width = 10;
        m_height = 10;
        // You might want to load a specific image here later
        break;
    }

    // Normalize direction vector and apply speed
    float length = sqrt(dirX * dirX + dirY * dirY);
    if (length > 0) {
        m_velX = (dirX / length) * m_speed;
        m_velY = (dirY / length) * m_speed;
    }
}

void Projectile::Update(float deltaTime) {
    if (!m_isActive) return;

    m_x += m_velX * deltaTime;
    m_y += m_velY * deltaTime;

    // Deactivate if it goes too far off-screen
    // This is a simple boundary check
    if (m_x < -1000 || m_x > 3500 || m_y < -1000 || m_y > 3500) {
        Deactivate();
    }
}

void Projectile::Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) {
    if (!m_isActive) return;

    int screenX = static_cast<int>((m_x - cameraX) * zoom);
    int screenY = static_cast<int>((m_y - cameraY) * zoom);
    int scrWidth = static_cast<int>(m_width * zoom);
    int scrHeight = static_cast<int>(m_height * zoom);

    if (screenX + scrWidth < 0 || screenX >(int)canvas.getWidth() ||
        screenY + scrHeight < 0 || screenY >(int)canvas.getHeight()) {
        return;
    }

    unsigned char r = 255, g = 255, b = 0; // Yellow for player bullets by default
    if (m_owner == ENEMY) {
        r = 255; g = 0; b = 0; // Red for enemy bullets
    }
    else if (m_type == CANNON) {
        r = 255; g = 165; b = 0; // Orange for cannonballs
    }

    for (int i = 0; i < scrHeight; ++i) {
        for (int j = 0; j < scrWidth; ++j) {
            int px = screenX + j;
            int py = screenY + i;
            if (px >= 0 && px < (int)canvas.getWidth() && py >= 0 && py < (int)canvas.getHeight()) {
                canvas.draw(px, py, r, g, b);
            }
        }
    }
}
