#include "Hero.h"
#include "Level.h"
#include <Windows.h>
#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Define TILE_SIZE if it's not defined elsewhere, assuming 32
#ifndef TILE_SIZE
#define TILE_SIZE 32
#endif

Hero::Hero() {
    movementSpeed = 250.0f;
    maxHealth = 200;
    currentHealth = maxHealth;

    m_legDirection = 0;
    m_torsoDirection = 0;
    m_currentFrame = 0.0f;
    m_animationSpeed = 24.0f;
    m_aimAngle = 0.0f;
    m_renderScale = 2.0f;
    m_torsoOffsetX = 0.0f;
    m_torsoOffsetY = -64.0f;

    m_isMoving = false;
    m_isSlowed = false;

    m_currentWeapon = WeaponType::MACHINE_GUN;
    m_machineGunCooldown = 0.08f;
    m_cannonCooldown = 2.5f;

    m_laserMaxCooldown = 10.0f;
    m_laserCooldown = 0.0f; // 初始狀態為可用

    m_fireCooldown = 0.0f;
    m_fireMaxCooldown = m_machineGunCooldown; // --- 新增: 默認為機槍的CD ---
    m_laserCharges = 0;
}

Hero::~Hero() {}

bool Hero::Load() {
    for (int i = 0; i < 8; ++i) {
        std::string path = "Resources/Mech_Legs_Animations/legs_" + std::to_string(i) + ".png";
        if (!m_legAnimations[i].load(path)) {
            std::cerr << "Failed to load hero leg animation: " << path << std::endl;
            return false;
        }
    }
    for (int i = 0; i < 32; ++i) {
        std::string path = "Resources/Mech_upper_Animations/hero_upper_run_front_export_dir" + std::to_string(i + 1) + ".png";
        if (!m_torsoAnimations[i].load(path)) {
            std::cerr << "Failed to load hero torso animation: " << path << std::endl;
            return false;
        }
    }
    int frameWidth = m_legAnimations[0].width / 5;
    int frameHeight = m_legAnimations[0].height / 8;
    this->width = frameWidth * m_renderScale;
    this->height = frameHeight * m_renderScale;
    return true;
}

void Hero::Update(Level& level, float deltaTime) {
    if (!isAlive) return;
    UpdateEffects(deltaTime);
    if (m_fireCooldown > 0) m_fireCooldown -= deltaTime;
    if (m_laserCooldown > 0) m_laserCooldown -= deltaTime;
    HandleInput(level, deltaTime);
    if (m_isMoving) {
        m_currentFrame += m_animationSpeed * deltaTime;
        if (m_currentFrame >= 40) {
            m_currentFrame = 0;
        }
    }
    else {
        m_currentFrame = 0;
    }
}

void Hero::HandleInput(Level& level, float deltaTime) {
    float currentSpeed = movementSpeed;
    if (m_isSlowed || m_slowTimer > 0) currentSpeed /= 2.0f;
    if (m_stunTimer > 0) currentSpeed = 0;
    float newX = x, newY = y;
    bool up = (GetAsyncKeyState('W') & 0x8000);
    bool down = (GetAsyncKeyState('S') & 0x8000);
    bool left = (GetAsyncKeyState('A') & 0x8000);
    bool right = (GetAsyncKeyState('D') & 0x8000);
    m_isMoving = up || down || left || right;
    if (!m_isMoving) { m_legDirection = 0; return; }
    float moveX = 0, moveY = 0;
    if (up) moveY -= 1; if (down) moveY += 1; if (left) moveX -= 1; if (right) moveX += 1;
    if (moveX != 0 && moveY != 0) { moveX *= 0.7071f; moveY *= 0.7071f; }
    newX += moveX * currentSpeed * deltaTime;
    newY += moveY * currentSpeed * deltaTime;
    if (down && !left && !right) m_legDirection = 0;
    else if (down && left) m_legDirection = 1;
    else if (left && !up && !down) m_legDirection = 2;
    else if (left && up) m_legDirection = 3;
    else if (up && !left && !right) m_legDirection = 4;
    else if (up && right) m_legDirection = 5;
    else if (right && !up && !down) m_legDirection = 6;
    else if (right && down) m_legDirection = 7;
    CheckMapCollision(level, newX, newY);
}

void Hero::UpdateAiming(GamesEngineeringBase::Window& window, int cameraX, int cameraY, float zoom) {
    float pivotX = GetFirePosX(), pivotY = GetFirePosY();
    float screenX = (pivotX - cameraX) * zoom;
    float screenY = (pivotY - cameraY) * zoom;
    int mouseX = window.getMouseInWindowX();
    int mouseY = window.getMouseInWindowY();
    float deltaX = mouseX - screenX;
    float deltaY = mouseY - screenY;
    m_aimAngle = atan2(-deltaY, deltaX);
    float standard_degrees = m_aimAngle * 180.0f / M_PI;
    if (standard_degrees < 0) standard_degrees += 360.0f;
    float asset_degrees = fmod(270.0f - standard_degrees + 360.0f, 360.0f);
    const float anglePerDirection = 11.25f;
    m_torsoDirection = static_cast<int>((asset_degrees + (anglePerDirection / 2.0f)) / anglePerDirection) % 32;
}

void Hero::Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) {
    if (!isAlive) return;
    auto renderPart = [&](GamesEngineeringBase::Image& sheet, int fCountX, int fCountY, int fIndex, float offX, float offY) {
        int fX = fIndex % fCountX, fY = fIndex / fCountX;
        int fW = sheet.width / fCountX, fH = sheet.height / fCountY;
        int sX = static_cast<int>((x + offX - cameraX) * zoom), sY = static_cast<int>((y + offY - cameraY) * zoom);
        int rW = static_cast<int>(fW * m_renderScale * zoom), rH = static_cast<int>(fH * m_renderScale * zoom);
        for (int sy = 0; sy < rH; ++sy) for (int sx = 0; sx < rW; ++sx) {
            int cX = sX + sx, cY = sY + sy;
            if (cX >= 0 && cX < (int)canvas.getWidth() && cY >= 0 && cY < (int)canvas.getHeight()) {
                unsigned int srcX = fX * fW + static_cast<unsigned int>(sx / (m_renderScale * zoom));
                unsigned int srcY = fY * fH + static_cast<unsigned int>(sy / (m_renderScale * zoom));
                if (srcX < sheet.width && srcY < sheet.height && sheet.alphaAt(srcX, srcY) > 200) canvas.draw(cX, cY, sheet.at(srcX, srcY));
            }
        }
        };
    renderPart(m_legAnimations[m_legDirection], 5, 8, static_cast<int>(m_currentFrame), 0, 0);
    renderPart(m_torsoAnimations[m_torsoDirection], 1, 1, 0, m_torsoOffsetX, m_torsoOffsetY);
}

void Hero::CheckMapCollision(Level& level, float newX, float newY) {
    int playerTileX = static_cast<int>((newX + width / 2.0f) / TILE_SIZE);
    int playerTileY = static_cast<int>((newY + height / 2.0f) / TILE_SIZE);
    if (!level.isObstacleAt(playerTileX, playerTileY)) {
        x = newX;
        y = newY;
    }
}

void Hero::SetSlowed(bool slowed) { m_isSlowed = slowed; }

bool Hero::CanFire() {
    return m_fireCooldown <= 0;
}

void Hero::ResetFireCooldown() {
    m_fireCooldown = m_fireMaxCooldown;
}

void Hero::SwitchWeapon() {
    if (m_currentWeapon == WeaponType::MACHINE_GUN) {
        m_currentWeapon = WeaponType::CANNON;
        m_fireMaxCooldown = m_cannonCooldown;
    }
    else {
        m_currentWeapon = WeaponType::MACHINE_GUN;
        m_fireMaxCooldown = m_machineGunCooldown;
    }
}

void Hero::AddLaserCharges(int amount) {
    m_laserCharges += amount;
}

void Hero::UseLaserCharge() {
    if (m_laserCharges > 0) {
        m_laserCharges--;
    }
}

int Hero::GetLaserCharges() const {
    return m_laserCharges;
}

float Hero::GetFirePosX() const {
    return x + width / 2.0f;
}

float Hero::GetFirePosY() const {
    if (m_torsoAnimations[0].height > 0) {
        return y + m_torsoOffsetY + (m_torsoAnimations[0].height * m_renderScale) / 2.0f;
    }
    return y + height / 2.0f;
}

// --- 新增: 恢复生命值的实现 ---
void Hero::RestoreFullHealth() {
    currentHealth = maxHealth;
    isAlive = true;
}
