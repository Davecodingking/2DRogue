#include "NPC.h"
#include "Level.h"
#include <cmath>
#include <iostream>

NPC::NPC(NPCType type)
    : m_currentState(WALKING), m_type(type),
    m_frameCountWalk(12), m_frameCountExplode(7),
    m_currentFrame(0.0f), m_animationSpeed(10.0f),
    m_explodeAnimationSpeed(10.0f), m_renderScale(0.3f),
    m_preferredDistance(300.0f), m_fireCooldown(0.0f), m_fireRate(2.0f) // Shoots every 2 seconds
{
    movementSpeed = 100.0f;
    currentHealth = 50;
    maxHealth = 50;
}

NPC::~NPC() {}

bool NPC::Load() {
    if (!m_walkAnimationSheet.load("Resources/npc_walk.png")) {
        std::cerr << "Failed to load npc_walk.png" << std::endl;
        return false;
    }
    if (!m_explodeAnimationSheet.load("Resources/npc_explode.png")) {
        std::cerr << "Failed to load npc_explode.png" << std::endl;
        return false;
    }
    this->width = (m_walkAnimationSheet.width / m_frameCountWalk) * m_renderScale;
    this->height = m_walkAnimationSheet.height * m_renderScale;
    return true;
}

void NPC::InitializeStats(int health, float speed, float renderScale) {
    this->currentHealth = health;
    this->maxHealth = health;
    this->movementSpeed = speed;
    this->m_renderScale = renderScale;
    this->width = (m_walkAnimationSheet.width / m_frameCountWalk) * m_renderScale;
    this->height = m_walkAnimationSheet.height * m_renderScale;
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
        if (m_currentFrame >= m_frameCountExplode) {
            m_currentState = DEAD;
        }
        return;
    }

    m_currentFrame += m_animationSpeed * deltaTime;
    if (m_currentFrame >= m_frameCountWalk) {
        m_currentFrame = 0.0f;
    }

    if (m_fireCooldown > 0) {
        m_fireCooldown -= deltaTime;
    }
}

void NPC::UpdateAI(float targetX, float targetY, float deltaTime) {
    if (m_currentState == DYING || m_currentState == DEAD) return;
    if (m_stunTimer > 0) return;

    if (m_type == MELEE) {
        m_currentState = WALKING;
        MoveTowards(targetX, targetY, deltaTime);
    }
    else if (m_type == SHOOTER) {
        float dirX = targetX - x;
        float dirY = targetY - y;
        float distance = sqrt(dirX * dirX + dirY * dirY);

        if (distance > m_preferredDistance + 50) {
            m_currentState = WALKING;
            MoveTowards(targetX, targetY, deltaTime);
        }
        else if (distance < m_preferredDistance - 50) {
            m_currentState = WALKING;
            MoveTowards(x - dirX, y - dirY, deltaTime);
        }
        else {
            m_currentState = SHOOTING; // In range, stop and prepare to shoot
        }
    }
}

void NPC::MoveTowards(float targetX, float targetY, float deltaTime) {
    if (m_currentState != WALKING) return;
    if (m_stunTimer > 0) return;

    float currentSpeed = movementSpeed;
    if (m_slowTimer > 0) currentSpeed /= 2.0f;

    float dirX = targetX - x;
    float dirY = targetY - y;
    float length = sqrt(dirX * dirX + dirY * dirY);

    if (length < 1.0f) return;

    dirX /= length;
    dirY /= length;

    x += dirX * currentSpeed * deltaTime;
    y += dirY * currentSpeed * deltaTime;
}

bool NPC::canFire() {
    return m_currentState == SHOOTING && m_fireCooldown <= 0;
}

void NPC::resetFireCooldown() {
    m_fireCooldown = m_fireRate;
}

void NPC::Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) {
    if (m_currentState == DEAD) return;

    GamesEngineeringBase::Image* sheet;
    int frameCount;
    if (m_currentState == DYING) {
        sheet = &m_explodeAnimationSheet;
        frameCount = m_frameCountExplode;
    }
    else {
        sheet = &m_walkAnimationSheet;
        frameCount = m_frameCountWalk;
    }

    int frameWidth = sheet->width / frameCount;
    int frameHeight = sheet->height;
    int frameIndex = static_cast<int>(m_currentFrame) % frameCount;

    int screenX = static_cast<int>((x - cameraX) * zoom);
    int screenY = static_cast<int>((y - cameraY) * zoom);
    int renderWidth = static_cast<int>(frameWidth * m_renderScale * zoom);
    int renderHeight = static_cast<int>(frameHeight * m_renderScale * zoom);

    if (screenX + renderWidth < 0 || screenX >(int)canvas.getWidth() ||
        screenY + renderHeight < 0 || screenY >(int)canvas.getHeight()) {
        return;
    }

    for (int sy = 0; sy < renderHeight; ++sy) {
        for (int sx = 0; sx < renderWidth; ++sx) {
            int canvasX = screenX + sx;
            int canvasY = screenY + sy;

            if (canvasX >= 0 && canvasX < (int)canvas.getWidth() && canvasY >= 0 && canvasY < (int)canvas.getHeight()) {
                unsigned int srcX = frameIndex * frameWidth + static_cast<unsigned int>(sx / (m_renderScale * zoom));
                unsigned int srcY = static_cast<unsigned int>(sy / (m_renderScale * zoom));

                if (srcX < sheet->width && srcY < sheet->height) {
                    if (sheet->alphaAt(srcX, srcY) > 200) {
                        canvas.draw(canvasX, canvasY, sheet->at(srcX, srcY));
                    }
                }
            }
        }
    }
}

