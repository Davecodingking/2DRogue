#include "Projectile.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Initialize static member variables
GamesEngineeringBase::Image Projectile::s_bulletImage;
GamesEngineeringBase::Image Projectile::s_explodeSheet;
bool Projectile::s_assetsLoaded = false;

bool Projectile::LoadAssets() {
    if (s_assetsLoaded) return true;
    if (!s_bulletImage.load("Resources/bullet.png")) {
        std::cerr << "Failed to load bullet.png" << std::endl;
        return false;
    }
    if (!s_explodeSheet.load("Resources/Explode-sheet.png")) {
        std::cerr << "Failed to load Explode-sheet.png" << std::endl;
        return false;
    }
    s_assetsLoaded = true;
    std::cout << "Projectile assets loaded successfully." << std::endl;
    return true;
}

void Projectile::FreeAssets() {
    s_bulletImage.free();
    s_explodeSheet.free();
    s_assetsLoaded = false;
}

Projectile::Projectile()
    : m_x(0), m_y(0), m_velX(0), m_velY(0),
    m_width(0), m_height(0), m_angle(0.0f), m_renderScale(1.0f),
    m_damage(0), m_speed(0), m_state(INACTIVE),
    m_owner(PLAYER), m_type(MACHINE_GUN),
    m_animationFrame(0.0f), m_animationSpeed(15.0f)
{
}

void Projectile::Activate(float startX, float startY, float angle, Type type, Owner owner) {
    m_state = FLYING;
    m_x = startX;
    m_y = startY;
    m_angle = angle;
    m_owner = owner;
    m_type = type;
    m_animationFrame = 0.0f;

    switch (m_type) {
    case MACHINE_GUN:
        m_speed = 800.0f;
        m_damage = 5;
        m_renderScale = 0.025f; // Scale down by 20x
        m_width = s_bulletImage.width * m_renderScale;
        m_height = s_bulletImage.height * m_renderScale;
        break;
    case CANNON:
        m_speed = 500.0f;
        m_damage = 50;
        m_renderScale = 1.5f; // Cannonballs can be larger
        m_width = (s_explodeSheet.width / 9) * m_renderScale;
        m_height = s_explodeSheet.height * m_renderScale;
        break;
    case ENEMY_BULLET:
        m_speed = 400.0f;
        m_damage = 10;
        m_renderScale = 0.05f;
        m_width = s_bulletImage.width * m_renderScale;
        m_height = s_bulletImage.height * m_renderScale;
        break;
    }

    m_velX = cos(m_angle) * m_speed;
    m_velY = -sin(m_angle) * m_speed; // Invert Y for screen coordinates
}

void Projectile::Update(float deltaTime) {
    if (m_state == INACTIVE) return;
    if (m_state == FLYING) {
        m_x += m_velX * deltaTime;
        m_y += m_velY * deltaTime;
        if (m_x < -1000 || m_x > 5000 || m_y < -1000 || m_y > 5000) {
            Deactivate();
        }
    }
    else if (m_state == EXPLODING) {
        m_animationFrame += m_animationSpeed * deltaTime;
        if (m_animationFrame >= 8.0f) { // Explosion has 8 frames (frames 2 to 9)
            Deactivate();
        }
    }
}

void Projectile::StartExplosion() {
    if (m_type == CANNON) {
        m_state = EXPLODING;
        m_animationFrame = 0.0f;
        m_velX = 0;
        m_velY = 0;
        m_width = (s_explodeSheet.width / 9) * m_renderScale * 2.0f;
        m_height = s_explodeSheet.height * m_renderScale * 2.0f;
    }
    else {
        Deactivate();
    }
}

void Projectile::Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) {
    if (m_state == INACTIVE || !s_assetsLoaded) return;

    if (m_state == FLYING) {
        GamesEngineeringBase::Image* sheet = nullptr;
        float rotation = -m_angle; // FIX: Invert angle for screen coordinates
        unsigned int src_frame_width = 0;
        unsigned int src_frame_height = 0;
        unsigned int src_offset_x = 0;

        if (m_type == MACHINE_GUN || m_type == ENEMY_BULLET) {
            sheet = &s_bulletImage;
            src_frame_width = sheet->width;
            src_frame_height = sheet->height;
        }
        else if (m_type == CANNON) {
            sheet = &s_explodeSheet;
            src_frame_width = sheet->width / 9;
            src_frame_height = sheet->height;
            rotation += (float)M_PI; // FIX: Add PI to correct left-facing sprite
        }
        if (!sheet) return;

        float centerX = src_frame_width / 2.0f;
        float centerY = src_frame_height / 2.0f;
        float cosA = cos(rotation);
        float sinA = sin(rotation);

        float scaledWidth = src_frame_width * m_renderScale * zoom;
        float scaledHeight = src_frame_height * m_renderScale * zoom;
        int renderWidth = static_cast<int>(ceil(abs(cosA * scaledWidth) + abs(sinA * scaledHeight)));
        int renderHeight = static_cast<int>(ceil(abs(sinA * scaledWidth) + abs(cosA * scaledHeight)));

        int screenCenterX = static_cast<int>((m_x - cameraX) * zoom);
        int screenCenterY = static_cast<int>((m_y - cameraY) * zoom);

        int startX = screenCenterX - renderWidth / 2;
        int startY = screenCenterY - renderHeight / 2;

        for (int y = 0; y < renderHeight; ++y) {
            for (int x = 0; x < renderWidth; ++x) {
                int canvasX = startX + x;
                int canvasY = startY + y;
                if (canvasX < 0 || canvasX >= (int)canvas.getWidth() || canvasY < 0 || canvasY >= (int)canvas.getHeight()) continue;

                float relativeX = ((float)x - renderWidth / 2.0f) / (m_renderScale * zoom);
                float relativeY = ((float)y - renderHeight / 2.0f) / (m_renderScale * zoom);

                unsigned int srcX = static_cast<unsigned int>(relativeX * cosA + relativeY * sinA + centerX);
                unsigned int srcY = static_cast<unsigned int>(-relativeX * sinA + relativeY * cosA + centerY);

                if (srcX < src_frame_width && srcY < src_frame_height) {
                    if (sheet->alphaAt(srcX + src_offset_x, srcY) > 200) {
                        canvas.draw(canvasX, canvasY, sheet->at(srcX + src_offset_x, srcY));
                    }
                }
            }
        }
    }
    else if (m_state == EXPLODING) {
        int frame_width = s_explodeSheet.width / 9;
        int current_frame_index = static_cast<int>(m_animationFrame) + 1; // Animation starts at frame 2 (index 1)
        if (current_frame_index > 8) current_frame_index = 8;

        int src_x_start = current_frame_index * frame_width;

        int renderWidth = static_cast<int>(frame_width * m_renderScale * 2.0f * zoom);
        int renderHeight = static_cast<int>(s_explodeSheet.height * m_renderScale * 2.0f * zoom);
        int screenX = static_cast<int>((m_x - (renderWidth / (2.0f * zoom)) - cameraX) * zoom);
        int screenY = static_cast<int>((m_y - (renderHeight / (2.0f * zoom)) - cameraY) * zoom);

        for (int sy = 0; sy < renderHeight; ++sy) {
            for (int sx = 0; sx < renderWidth; ++sx) {
                int canvasX = screenX + sx;
                int canvasY = screenY + sy;
                if (canvasX >= 0 && canvasX < (int)canvas.getWidth() && canvasY >= 0 && canvasY < (int)canvas.getHeight()) {
                    unsigned int srcX = src_x_start + static_cast<unsigned int>(sx / (m_renderScale * 2.0f * zoom));
                    unsigned int srcY = static_cast<unsigned int>(sy / (m_renderScale * 2.0f * zoom));
                    if (s_explodeSheet.alphaAt(srcX, srcY) > 200) {
                        canvas.draw(canvasX, canvasY, s_explodeSheet.at(srcX, srcY));
                    }
                }
            }
        }
    }
}

