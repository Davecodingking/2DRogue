#pragma once
#include "GamesEngineeringBase.h"

class Level; // ǰ������

class Character {
public:
    Character() :
        x(0.0f), y(0.0f),
        width(0), height(0),
        movementSpeed(0.0f),
        currentHealth(100), maxHealth(100),
        isAlive(true) {
    }

    virtual ~Character() {}

    // ���麯��
    virtual void Update(Level& level, float deltaTime) = 0;
    virtual void Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) = 0;

    // ͨ�÷���
    virtual void TakeDamage(int damage) {
        currentHealth -= damage;
        if (currentHealth <= 0) {
            currentHealth = 0;
            isAlive = false;
        }
    }

    // --- ����: ����λ�õĺ��� ---
    virtual void SetPosition(float newX, float newY) {
        x = newX;
        y = newY;
    }

    // Getters
    float getX() const { return x; }
    float getY() const { return y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getCurrentHealth() const { return currentHealth; }
    virtual bool getIsAlive() const { return isAlive; }

protected:
    // --- x �� y ��Ϊ float ---
    float x, y;
    int width, height;
    float movementSpeed;
    int currentHealth;
    int maxHealth;
    bool isAlive;

    const int TILE_SIZE = 32;
};

