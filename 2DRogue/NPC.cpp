#include "NPC.h"
#include "Level.h"
#include <cmath>
#include <iostream>
#include <string>
#include <fstream>

#ifndef M_PI
#define M_PI 3.1415926535823846
#endif

bool LoadImageWithCheck(GamesEngineeringBase::Image& image, const std::string& path) {
    std::ifstream file(path);
    if (!file.good()) {
        std::cerr << "ERROR: Cannot find or open image file at path: " << path << std::endl;
        return false;
    }
    file.close();

    if (!image.load(path)) {
        std::cerr << "ERROR: Found file, but failed to load/decode image: " << path << std::endl;
        return false;
    }
    return true;
}


NPC::NPC(NPCType type)
    : m_currentState(WALKING), m_type(type),
    m_frameCountWalk(12), m_frameCountExplode(7), m_frameCountSniper(6),
    m_currentFrame(0.0f), m_animationSpeed(10.0f),
    m_explodeAnimationSpeed(10.0f), m_renderScale(0.3f), m_direction(0),
    m_preferredDistance(300.0f), m_fireCooldown(0.0f), m_fireRate(2.0f),
    m_bossAttackTimer(0.0f)
{
    // Set stats based on type
    switch (m_type) {
    case MELEE:
        // ** 关键修正: 恢复原始尺寸 **
        InitializeStats(50, 100.0f, 0.15f);
        break;
    case SHOOTER:
        // ** 关键修正: 恢复原始尺寸 **
        InitializeStats(40, 80.0f, 0.3f);
        m_preferredDistance = 300.0f;
        m_fireRate = 2.0f;
        break;
    case SNIPER:
        // ** 关键修正: 放大狙击手(NPC3)的尺寸 **
        InitializeStats(60, 60.0f, 4.0f);
        m_preferredDistance = 500.0f; // Longer range
        m_fireRate = 3.5f;           // Slower fire rate
        break;
    case BOSS_AIRCRAFT:
        InitializeStats(2000, 250.0f, 5.0f); // High health, fast speed
        m_preferredDistance = 400.0f;
        m_fireRate = 0.2f; // Fires rapidly during attack phase
        break;
    }
}

NPC::~NPC() {}

bool NPC::Load() {
    bool success = true;
    switch (m_type) {
    case MELEE:
    case SHOOTER:
        if (!LoadImageWithCheck(m_walkAnimationSheet, "Resources/npc_walk.png")) {
            success = false;
        }
        break;
    case SNIPER:
        for (int i = 0; i < 8; ++i) {
            std::string path = "Resources/npc3/npc3_export_dir" + std::to_string(i + 1) + ".png";
            if (!LoadImageWithCheck(m_sniperAnimationSheets[i], path)) {
                success = false;
                break;
            }
        }
        break;
    case BOSS_AIRCRAFT:
        if (!LoadImageWithCheck(m_bossImage, "Resources/npc4/npc4.png")) {
            success = false;
        }
        break;
    }

    // ** 关键修正: 只为需要爆炸特效的NPC加载爆炸资源 **
    if (success && (m_type == MELEE || m_type == SHOOTER)) {
        if (!LoadImageWithCheck(m_explodeAnimationSheet, "Resources/npc_explode.png")) {
            success = false;
        }
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

    int finalDamage = (m_type == SNIPER) ? static_cast<int>(damage * 1.5) : damage;

    Character::TakeDamage(finalDamage);

    if (!isAlive) {
        // ** 关键修正: 根据NPC类型决定死亡状态 **
        if (m_type == SNIPER || m_type == BOSS_AIRCRAFT) {
            // 狙击手和Boss死亡后直接标记为DEAD，跳过爆炸动画
            m_currentState = DEAD;
        }
        else {
            // 近战和普通射手播放爆炸动画
            m_currentState = DYING;
            m_currentFrame = 0.0f;
        }
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

    if (m_type == MELEE || m_type == SHOOTER) {
        m_currentFrame += m_animationSpeed * deltaTime;
        if (m_currentFrame >= m_frameCountWalk) {
            m_currentFrame = 0.0f;
        }
    }


    if (m_fireCooldown > 0) {
        m_fireCooldown -= deltaTime;
    }
}

void NPC::UpdateAI(float targetX, float targetY, float deltaTime) {
    if (m_currentState == DYING || m_currentState == DEAD) return;
    if (m_stunTimer > 0) return;

    float dirX = targetX - x;
    float dirY = targetY - y;
    float distance = sqrt(dirX * dirX + dirY * dirY);

    if (m_type == SNIPER) {
        float angle = atan2(dirY, dirX) * 180.0f / M_PI;
        if (angle < 0) angle += 360;

        // ** 关键修正: 修正狙击手的动画方向 **
        // 1. 先计算出标准的8方向索引 (0=East, 1=NE, 2=N, 3=NW, 4=W, 5=SW, 6=S, 7=SE)
        int standard_direction = static_cast<int>((angle + 22.5f) / 45.0f) % 8;

        // 2. 根据美术资源顺序("从下逆时针")，将标准方向映射到正确的图片索引
        // 美术资源顺序: 0:S, 1:SE, 2:E, 3:NE, 4:N, 5:NW, 6:W, 7:SW
        const int direction_map[8] = { 2, 3, 4, 5, 6, 7, 0, 1 };
        m_direction = direction_map[standard_direction];
    }


    switch (m_type) {
    case MELEE:
        m_currentState = WALKING;
        MoveTowards(targetX, targetY, deltaTime);
        break;

    case SHOOTER:
    case SNIPER:
        if (distance > m_preferredDistance + 50) {
            m_currentState = WALKING;
            MoveTowards(targetX, targetY, deltaTime);
        }
        else if (distance < m_preferredDistance - 50) {
            m_currentState = WALKING;
            MoveTowards(x - dirX, y - dirY, deltaTime);
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
            MoveTowards(targetX, targetY, deltaTime);
            if (distance <= m_preferredDistance) {
                m_currentState = SHOOTING;
                m_bossAttackTimer = 4.0f;
            }
        }
        break;
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

    GamesEngineeringBase::Image* sheet = nullptr;
    int frameCount = 1;
    int frameIndex = 0;

    if (m_currentState == DYING) {
        sheet = &m_explodeAnimationSheet;
        frameCount = m_frameCountExplode;
        frameIndex = static_cast<int>(m_currentFrame);
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
            frameIndex = 0;
            break;
        case BOSS_AIRCRAFT:
            sheet = &m_bossImage;
            frameCount = 1;
            frameIndex = 0;
            break;
        }
    }

    if (!sheet || sheet->width == 0) return;

    int frameWidth = sheet->width / frameCount;
    int frameHeight = sheet->height;
    frameIndex = frameIndex % frameCount;

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


