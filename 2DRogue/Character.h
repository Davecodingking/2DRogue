#pragma once

// --- ǰ������ (Forward Declarations) ---
class Level;
namespace GamesEngineeringBase {
    class Window;
}

class Character {
public:
    Character() : x(0), y(0), width(0), height(0), movementSpeed(0.0f), currentHealth(100), maxHealth(100), isAlive(true) {}
    virtual ~Character() {}

    // --- ���麯�� ---
    virtual void Update(Level& level, float deltaTime) = 0;
    virtual void Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) = 0;

    // --- ͨ�÷��� ---
    void TakeDamage(int damage) {
        currentHealth -= damage;
        if (currentHealth <= 0) {
            currentHealth = 0;
            isAlive = false;
        }
    }

    // --- Getters (������) ---
    int getX() const { return x; }
    int getY() const { return y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    bool getIsAlive() const { return isAlive; } // <-- �����������ⲿ�����״̬

protected:
    // --- ͨ������ ---
    int x, y;
    int width, height;
    float movementSpeed;
    int currentHealth;
    int maxHealth;
    bool isAlive;
};
