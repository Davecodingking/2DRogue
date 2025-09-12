#include "LaserBeam.h"
#include "Hero.h"
#include "NPC.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

GamesEngineeringBase::Image LaserBeam::s_laserImage;

LaserBeam::LaserBeam()
    : m_isActive(false), m_lifeTimer(0.0f),
    m_owner(nullptr), m_actualStartX(0), m_actualStartY(0),
    m_targetX(0), m_targetY(0),
    m_angle(0.0f), m_length(0.0f), m_activateDelay(0.0f),
    m_hasDamaged(false), m_isInitialized(false)
{
}

bool LaserBeam::LoadAssets(const std::string& filename) {
    if (!s_laserImage.load(filename)) {
        return false;
    }
    return true;
}

void LaserBeam::FreeAssets() {
    s_laserImage.free();
}

void LaserBeam::Activate(Hero* owner, float targetX, float targetY, float delay) {
    m_isActive = true;
    m_lifeTimer = 0.5f;
    m_owner = owner;
    m_targetX = targetX;
    m_targetY = targetY;
    m_activateDelay = delay;
    m_hasDamaged = false;
    m_isInitialized = false; // --- 核心修改: 每次激活时重置初始化状态 ---
}

bool LaserBeam::Update(float deltaTime) {
    if (!m_isActive) return false;

    // 首先处理激活延迟
    if (m_activateDelay > 0) {
        m_activateDelay -= deltaTime;
        return false;
    }

    // --- 核心修改: 延迟结束后，如果尚未初始化，则执行初始化 ---
    if (!m_isInitialized) {
        if (m_owner) {
            // 获取英雄当前最新位置作为发射点
            m_actualStartX = m_owner->GetFirePosX();
            m_actualStartY = m_owner->GetFirePosY();

            // 根据最新位置计算角度和长度
            float dx = m_targetX - m_actualStartX;
            float dy = m_targetY - m_actualStartY;
            m_angle = atan2(dy, dx);
            m_length = sqrt(dx * dx + dy * dy) + 500;

            m_isInitialized = true; // 标记为已初始化
        }
        else {
            m_isActive = false; // 如果发射源无效，则取消
            return false;
        }
    }

    // 处理生命周期
    m_lifeTimer -= deltaTime;
    if (m_lifeTimer <= 0) {
        m_isActive = false;
    }

    // 处理伤害信号 (只发送一次)
    if (!m_hasDamaged) {
        m_hasDamaged = true;
        return true;
    }

    return false;
}

void LaserBeam::Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) {
    // 只有在初始化之后才渲染
    if (!m_isActive || !m_isInitialized || s_laserImage.width == 0) return;

    float screenStartX = (m_actualStartX - cameraX) * zoom;
    float screenStartY = (m_actualStartY - cameraY) * zoom;

    float beamWidth = s_laserImage.height;
    float renderAngle = m_angle;

    for (int i = 0; i < m_length * zoom; ++i) {
        float currentX = screenStartX + cos(renderAngle) * i;
        float currentY = screenStartY + sin(renderAngle) * i;

        for (int w = -static_cast<int>(beamWidth / 2 * zoom); w < static_cast<int>(beamWidth / 2 * zoom); ++w) {
            int screenX = static_cast<int>(currentX - sin(renderAngle) * w);
            int screenY = static_cast<int>(currentY + cos(renderAngle) * w);

            if (screenX >= 0 && screenX < (int)canvas.getWidth() && screenY >= 0 && screenY < (int)canvas.getHeight()) {
                unsigned int srcX = static_cast<unsigned int>(i / zoom);
                unsigned int srcY = static_cast<unsigned int>((w + beamWidth / 2 * zoom) / zoom);

                if (srcX < s_laserImage.width && srcY < s_laserImage.height && s_laserImage.alphaAt(srcX, srcY) > 200) {
                    canvas.draw(screenX, screenY, s_laserImage.at(srcX, srcY));
                }
            }
        }
    }
}

bool LaserBeam::CheckCollision(NPC* npc) {
    // 只有在初始化之后才进行碰撞检测
    if (!m_isInitialized) return false;

    float npcX = npc->getX() + npc->getWidth() / 2.0f;
    float npcY = npc->getY() + npc->getHeight() / 2.0f;
    float npcRadius = (npc->getWidth() + npc->getHeight()) / 4.0f;

    float dx = m_targetX - m_actualStartX;
    float dy = m_targetY - m_actualStartY;

    float lenSq = dx * dx + dy * dy;
    if (lenSq == 0.0) return false;

    float t = ((npcX - m_actualStartX) * dx + (npcY - m_actualStartY) * dy) / lenSq;
    t = (t < 0.0f) ? 0.0f : (t > 1.0f) ? 1.0f : t;

    float closestX = m_actualStartX + t * dx;
    float closestY = m_actualStartY + t * dy;

    float distSq = (closestX - npcX) * (closestX - npcX) + (closestY - npcY) * (closestY - npcY);

    return distSq < (npcRadius * npcRadius);
}

