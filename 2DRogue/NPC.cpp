#include "NPC.h"
#include "Level.h"
#include <cmath>
#include <iostream>

NPC::NPC() {
    m_currentState = State::WALKING;
    m_renderScale = 0.25f; // NPC���������ű���

    m_currentFrame = 0.0f;
    m_animationSpeed = 15.0f;
    m_explodeAnimationSpeed = 10.0f; // ����һ�������ı�ը�ٶ�
    m_walkFrames = 12;
    m_explodeFrames = 7;

    movementSpeed = 100.0f;
    maxHealth = 100;
    currentHealth = 100;
}

NPC::~NPC() {}

bool NPC::Load() {
    if (!m_walkAnimationSheet.load("Resources/npc_walk.png")) {
        std::cerr << "����NPC����ͼƬʧ��: Resources/npc_walk.png" << std::endl;
        return false;
    }
    if (!m_explodeAnimationSheet.load("Resources/npc_explode.png")) {
        std::cerr << "����NPC��ըͼƬʧ��: Resources/npc_explode.png" << std::endl;
        return false;
    }

    // ʹ������ͼ�ĵ�һ֡�ߴ���ȷ����ײ��С
    if (m_walkAnimationSheet.width > 0 && m_walkFrames > 0) {
        this->width = static_cast<int>((m_walkAnimationSheet.width / m_walkFrames) * m_renderScale);
        this->height = static_cast<int>(m_walkAnimationSheet.height * m_renderScale);
    }
    return true;
}

void NPC::InitializeStats(int health, float speed) {
    this->maxHealth = health;
    this->currentHealth = health;
    this->movementSpeed = speed;
}

void NPC::SetPosition(float newX, float newY) {
    this->x = newX;
    this->y = newY;
}

void NPC::Update(Level& level, float deltaTime) {
    if (m_currentState == State::WALKING) {
        m_currentFrame += m_animationSpeed * deltaTime;
        if (m_currentFrame >= m_walkFrames) {
            m_currentFrame -= m_walkFrames;
        }
    }
    else if (m_currentState == State::DYING) {
        m_currentFrame += m_animationSpeed * deltaTime;
        if (m_currentFrame >= m_explodeFrames) {
            m_currentState = State::DEAD;
        }
    }
}

void NPC::MoveTowards(float targetX, float targetY, float deltaTime) {
    if (m_currentState != State::WALKING) return;

    float dirX = targetX - (this->x + this->width / 2.0f);
    float dirY = targetY - (this->y + this->height / 2.0f);

    float length = sqrt(dirX * dirX + dirY * dirY);

    if (length < 1.0f) return;

    dirX /= length;
    dirY /= length;

    this->x += dirX * movementSpeed * deltaTime;
    this->y += dirY * movementSpeed * deltaTime;
}

void NPC::TakeDamage(int damage) {
    if (m_currentState != State::WALKING) return;

    currentHealth -= damage;
    if (currentHealth <= 0) {
        currentHealth = 0;
        m_currentState = State::DYING;
        m_currentFrame = 0; // ���ö���֡�Ա��ͷ����
        m_animationSpeed = m_explodeAnimationSpeed; // --- �ؼ��������л�����ը�����ٶ� ---
    }
}

bool NPC::getIsAlive() const {
    return m_currentState != State::DEAD;
}

NPC::State NPC::getCurrentState() const {
    return m_currentState;
}

void NPC::Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) {
    if (m_currentState == State::DEAD) return;

    GamesEngineeringBase::Image* currentSheet = nullptr;
    int totalFrames = 0;

    if (m_currentState == State::WALKING) {
        currentSheet = &m_walkAnimationSheet;
        totalFrames = m_walkFrames;
    }
    else { // DYING
        currentSheet = &m_explodeAnimationSheet;
        totalFrames = m_explodeFrames;
    }

    if (!currentSheet || currentSheet->width == 0) return;

    int frameWidth = currentSheet->width / totalFrames;
    int frameHeight = currentSheet->height;
    int frameIndex = static_cast<int>(m_currentFrame);
    if (frameIndex >= totalFrames) frameIndex = totalFrames - 1;

    int sourceX = frameIndex * frameWidth;
    int sourceY = 0;

    int screenX_start = static_cast<int>(round((x - cameraX) * zoom));
    int screenY_start = static_cast<int>(round((y - cameraY) * zoom));
    int screenX_end = static_cast<int>(round((x + width - cameraX) * zoom));
    int screenY_end = static_cast<int>(round((y + height - cameraY) * zoom));

    if (screenX_start == screenX_end || screenY_start == screenY_end) return;

    for (int screenY = screenY_start; screenY < screenY_end; ++screenY) {
        for (int screenX = screenX_start; screenX < screenX_end; ++screenX) {
            if (screenX >= 0 && screenX < (int)canvas.getWidth() && screenY >= 0 && screenY < (int)canvas.getHeight()) {
                unsigned int src_pixel_x = sourceX + static_cast<unsigned int>((double)(screenX - screenX_start) / (double)(screenX_end - screenX_start) * frameWidth);
                unsigned int src_pixel_y = sourceY + static_cast<unsigned int>((double)(screenY - screenY_start) / (double)(screenY_end - screenY_start) * frameHeight);

                if (currentSheet->alphaAt(src_pixel_x, src_pixel_y) > 200) {
                    canvas.draw(screenX, screenY, currentSheet->at(src_pixel_x, src_pixel_y));
                }
            }
        }
    }
}

