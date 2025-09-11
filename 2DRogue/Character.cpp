#include "Character.h"

Character::Character()
    : x(0), y(0), width(0), height(0),
    movementSpeed(0.0f), currentHealth(100), maxHealth(100),
    isAlive(true), m_slowTimer(0.0f), m_stunTimer(0.0f)
{
}

void Character::TakeDamage(int damage) {
    currentHealth -= damage;
    if (currentHealth <= 0) {
        currentHealth = 0;
        isAlive = false;
    }
}

void Character::SetPosition(float startX, float startY) {
    this->x = startX;
    this->y = startY;
}

void Character::ApplySlow(float duration) {
    if (duration > m_slowTimer) {
        m_slowTimer = duration;
    }
}

void Character::ApplyStun(float duration) {
    if (duration > m_stunTimer) {
        m_stunTimer = duration;
    }
}

void Character::UpdateEffects(float deltaTime) {
    if (m_slowTimer > 0) {
        m_slowTimer -= deltaTime;
    }
    if (m_stunTimer > 0) {
        m_stunTimer -= deltaTime;
    }
}
