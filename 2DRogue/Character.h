#pragma once

class Level;
namespace GamesEngineeringBase {
    class Window;
}

class Character {
public:
    Character();
    virtual ~Character() {}

    virtual void Update(Level& level, float deltaTime) = 0;
    virtual void Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) = 0;

    virtual void TakeDamage(int damage);

    // --- State Effects ---
    void ApplySlow(float duration);
    void ApplyStun(float duration);

    float getX() const { return x; }
    float getY() const { return y; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    bool getIsAlive() const { return isAlive; }

    virtual void SetPosition(float startX, float startY);

protected:
    void UpdateEffects(float deltaTime);

    float x, y;
    float width, height;
    float movementSpeed;
    int currentHealth;
    int maxHealth;
    bool isAlive;

    // Effect timers
    float m_slowTimer;
    float m_stunTimer;
};

