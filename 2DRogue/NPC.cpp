#include "NPC.h"
#include "Level.h"
#include <cmath>
#include <iostream>
#include <string>
#include <fstream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// --- Static Member Initialization ---
GamesEngineeringBase::Image NPC::s_specialExplosionSheet;
bool NPC::s_assetsLoaded = false;

// Helper function to check file existence before loading
bool LoadImageWithCheck(GamesEngineeringBase::Image& image, const std::string& path) {
    std::ifstream file(path.c_str());
    if (!file.good()) {
        std::cerr << "ERROR: Cannot find image file: " << path << std::endl;
        return false;
    }
    return image.load(path);
}

// --- Static Asset Management ---
bool NPC::LoadSharedAssets() {
    if (s_assetsLoaded) return true;
    if (!LoadImageWithCheck(s_specialExplosionSheet, "Resources/Explode-sheet.png")) {
        return false;
    }
    s_assetsLoaded = true;
    std::cout << "NPC shared assets (special explosion) loaded." << std::endl;
    return true;
}

void NPC::FreeSharedAssets() {
    s_specialExplosionSheet.free();
    s_assetsLoaded = false;
}


NPC::NPC(NPCType type)
    : m_currentState(WALKING), m_type(type),
    m_frameCountWalk(12), m_frameCountExplode(7),
    m_currentFrame(0.0f), m_animationSpeed(10.0f),
    m_explodeAnimationSpeed(15.0f), m_renderScale(0.3f), m_direction(0),
    m_preferredDistance(300.0f), m_fireCooldown(0.0f), m_fireRate(2.0f),
    m_bossAttackTimer(0.0f)
{
    // Set stats based on type
    switch (m_type) {
    case MELEE:
        InitializeStats(50, 100.0f, 0.15f);
        break;
    case SHOOTER:
        InitializeStats(40, 80.0f, 0.3f);
        m_preferredDistance = 300.0f;
        m_fireRate = 2.0f;
        break;
    case SNIPER:
        InitializeStats(60, 60.0f, 4.0f);
        m_preferredDistance = 500.0f;
        m_fireRate = 3.5f;
        break;
    case BOSS_AIRCRAFT:
        InitializeStats(2000, 250.0f, 5.0f);
        m_preferredDistance = 400.0f;
        m_fireRate = 0.2f;
        break;
    }
}

NPC::~NPC() {}

bool NPC::Load() {
    bool success = true;
    switch (m_type) {
    case MELEE:
    case SHOOTER:
        if (!LoadImageWithCheck(m_walkAnimationSheet, "Resources/npc_walk.png")) success = false;
        if (!LoadImageWithCheck(m_explodeAnimationSheet, "Resources/npc_explode.png")) success = false;
        break;
    case SNIPER:
        for (int i = 0; i < 8; ++i) {
            if (!LoadImageWithCheck(m_sniperAnimationSheets[i], "Resources/npc3/npc3_export_dir" + std::to_string(i + 1) + ".png")) {
                success = false;
                break;
            }
        }
        break;
    case BOSS_AIRCRAFT:
        if (!LoadImageWithCheck(m_bossImage, "Resources/npc4/npc4.png")) success = false;
        break;
    }

    if (success) {
        if (m_type == MELEE || m_type == SHOOTER) {
            this->width = (m_walkAnimationSheet.width / m_frameCountWalk) * m_renderScale;
            this->height = m_walkAnimationSheet.height * m_renderScale;
        }
        else if (m_type == SNIPER) {
            this->width = m_sniperAnimationSheets[0].width * m_renderScale;
            this->height = m_sniperAnimationSheets[0].height * m_renderScale;
        }
        else if (m_type == BOSS_AIRCRAFT) {
            this->width = m_bossImage.width * m_renderScale;
            this->height = m_bossImage.height * m_renderScale;
        }
    }

    return success;
}

void NPC::InitializeStats(int health, float speed, float renderScale) {
    this->currentHealth = health;
    this->maxHealth = health;
    this->movementSpeed = speed;
    this->m_renderScale = renderScale;
}

void NPC::TakeDamage(int damage) {
    if (m_currentState == DYING || m_currentState == DEAD) return;
    Character::TakeDamage(damage);
    if (!isAlive) {
        m_currentState = DYING;
        m_currentFrame = 0.0f;
    }
}

void NPC::Update(Level& level, float deltaTime) {
    if (m_currentState == DEAD) return;

    UpdateEffects(deltaTime);

    if (m_currentState == DYING) {
        m_currentFrame += m_explodeAnimationSpeed * deltaTime;

        // Regular NPCs have 7 frames, special ones have 8 for explosion
        int frame_count_for_death = (m_type == SNIPER || m_type == BOSS_AIRCRAFT) ? 8 : m_frameCountExplode;

        if (m_currentFrame >= frame_count_for_death) {
            m_currentState = DEAD;
        }
        return;
    }

    if (m_type == MELEE || m_type == SHOOTER) {
        m_currentFrame += m_animationSpeed * deltaTime;
        if (m_currentFrame >= m_frameCountWalk) {
            m_currentFrame -= m_frameCountWalk;
        }
    }

    if (m_fireCooldown > 0) {
        m_fireCooldown -= deltaTime;
    }

    // AI logic is now separated
}

void NPC::UpdateAI(float targetX, float targetY, float deltaTime) {
    if (m_currentState == DYING || m_currentState == DEAD || m_stunTimer > 0) return;

    float dirX = targetX - x;
    float dirY = targetY - y;
    float distance = sqrt(dirX * dirX + dirY * dirY);

    if (m_type == SNIPER) {
        float angle = atan2(dirY, dirX) * 180.0f / (float)M_PI;
        if (angle < 0) angle += 360;
        int standard_direction = static_cast<int>((angle + 22.5f) / 45.0f) % 8;
        const int direction_map[8] = { 2, 3, 4, 5, 6, 7, 0, 1 };
        m_direction = direction_map[standard_direction];
    }

    // This part remains the same as your original logic
    switch (m_type) {
    case MELEE:
        m_currentState = WALKING;
        // The actual movement is now handled in Update function via MoveTowards
        break;
    case SHOOTER:
    case SNIPER:
        if (distance > m_preferredDistance + 50) {
            m_currentState = WALKING;
        }
        else if (distance < m_preferredDistance - 50) {
            m_currentState = WALKING;
        }
        else {
            m_currentState = SHOOTING;
        }
        break;
    case BOSS_AIRCRAFT:
        if (m_currentState == SHOOTING) {
            m_bossAttackTimer -= deltaTime;
            if (m_bossAttackTimer <= 0) {
                m_currentState = WALKING;
            }
        }
        else {
            if (distance <= m_preferredDistance) {
                m_currentState = SHOOTING;
                m_bossAttackTimer = 4.0f;
            }
            else {
                m_currentState = WALKING;
            }
        }
        break;
    }
}


void NPC::MoveTowards(Level& level, float targetX, float targetY, float deltaTime) {
    if (m_currentState != WALKING || m_stunTimer > 0) return;

    float currentSpeed = movementSpeed;
    if (m_slowTimer > 0) currentSpeed /= 2.0f;

    float dirX = targetX - x;
    float dirY = targetY - y;
    float length = sqrt(dirX * dirX + dirY * dirY);
    if (length < 1.0f) return;

    dirX /= length;
    dirY /= length;

    float newX = x + dirX * currentSpeed * deltaTime;
    float newY = y + dirY * currentSpeed * deltaTime;

    // --- NEW: Collision check before moving ---
    CheckMapCollision(level, newX, newY);
}

void NPC::CheckMapCollision(Level& level, float& newX, float& newY) {
    // --- NEW: Boss ignores obstacles ---
    if (m_type == BOSS_AIRCRAFT) {
        x = newX;
        y = newY;
        return;
    }

    int tileX = static_cast<int>((newX + width / 2.0f) / 32);
    int tileY = static_cast<int>((newY + height / 2.0f) / 32);

    if (!level.isObstacleAt(tileX, tileY)) {
        x = newX;
        y = newY;
    }
}


bool NPC::canFire() {
    return m_currentState == SHOOTING && m_fireCooldown <= 0;
}

void NPC::resetFireCooldown() {
    m_fireCooldown = m_fireRate;
}

void NPC::Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) {
    if (m_currentState == DEAD) return;

    GamesEngineeringBase::Image* sheet = nullptr;
    int frameCount = 1;
    int frameIndex = 0;
    int src_offset_x = 0;
    float currentRenderScale = m_renderScale;

    if (m_currentState == DYING) {
        if (m_type == SNIPER || m_type == BOSS_AIRCRAFT) {
            sheet = &s_specialExplosionSheet;
            frameCount = 9;
            frameIndex = static_cast<int>(m_currentFrame) + 1; // Animation starts at frame 2 (index 1)
            if (frameIndex > 8) frameIndex = 8;
            currentRenderScale = m_renderScale * 0.5f; // Explosion can be a different size
        }
        else {
            sheet = &m_explodeAnimationSheet;
            frameCount = m_frameCountExplode;
            frameIndex = static_cast<int>(m_currentFrame);
        }
    }
    else {
        switch (m_type) {
        case MELEE:
        case SHOOTER:
            sheet = &m_walkAnimationSheet;
            frameCount = m_frameCountWalk;
            frameIndex = static_cast<int>(m_currentFrame);
            break;
        case SNIPER:
            sheet = &m_sniperAnimationSheets[m_direction];
            frameCount = 1;
            break;
        case BOSS_AIRCRAFT:
            sheet = &m_bossImage;
            frameCount = 1;
            break;
        }
    }

    if (!sheet || sheet->width == 0) return;

    int frameWidth = sheet->width / frameCount;
    int frameHeight = sheet->height;
    frameIndex = frameIndex % frameCount;

    int renderWidth = static_cast<int>(frameWidth * currentRenderScale * zoom);
    int renderHeight = static_cast<int>(frameHeight * currentRenderScale * zoom);
    int screenX = static_cast<int>((x - cameraX) * zoom - (renderWidth / 2.0f) + (width / 2.0f * zoom));
    int screenY = static_cast<int>((y - cameraY) * zoom - (renderHeight / 2.0f) + (height / 2.0f * zoom));

    if (screenX + renderWidth < 0 || screenX >(int)canvas.getWidth() || screenY + renderHeight < 0 || screenY >(int)canvas.getHeight()) {
        return;
    }

    for (int sy = 0; sy < renderHeight; ++sy) {
        for (int sx = 0; sx < renderWidth; ++sx) {
            int canvasX = screenX + sx;
            int canvasY = screenY + sy;
            if (canvasX >= 0 && canvasX < (int)canvas.getWidth() && canvasY >= 0 && canvasY < (int)canvas.getHeight()) {
                unsigned int srcX = (frameIndex * frameWidth) + static_cast<unsigned int>(sx / (currentRenderScale * zoom));
                unsigned int srcY = static_cast<unsigned int>(sy / (currentRenderScale * zoom));
                if (sheet->alphaAt(srcX, srcY) > 200) {
                    canvas.draw(canvasX, canvasY, sheet->at(srcX, srcY));
                }
            }
        }
    }
}
