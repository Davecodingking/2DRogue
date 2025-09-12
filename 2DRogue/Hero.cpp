#include "Hero.h"
#include "Level.h"
#include <Windows.h>
#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Hero::Hero() {
    movementSpeed = 200.0f;
    maxHealth = 100;
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
    m_machineGunCooldown = 0.15f;
    m_cannonCooldown = 0.8f;
    m_fireCooldown = 0.0f;
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

    // Set width/height based on a single frame and scale
    int frameWidth = m_legAnimations[0].width / 5;
    int frameHeight = m_legAnimations[0].height / 8;
    this->width = frameWidth * m_renderScale;
    this->height = frameHeight * m_renderScale;

    return true;
}

void Hero::SetPosition(float startX, float startY) {
    this->x = startX;
    this->y = startY;
}

void Hero::TakeDamage(int damage) {
    Character::TakeDamage(damage);
    std::cout << "Hero took " << damage << " damage! Current health: " << currentHealth << std::endl;
}


void Hero::Update(Level& level, float deltaTime) {
    if (!isAlive) return;

    UpdateEffects(deltaTime);

    if (m_fireCooldown > 0) {
        m_fireCooldown -= deltaTime;
    }

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

    if (m_isSlowed || m_slowTimer > 0) {
        currentSpeed /= 2.0f;
    }
    if (m_stunTimer > 0) {
        currentSpeed = 0;
    }

    float newX = x;
    float newY = y;

    bool up = (GetAsyncKeyState('W') & 0x8000);
    bool down = (GetAsyncKeyState('S') & 0x8000);
    bool left = (GetAsyncKeyState('A') & 0x8000);
    bool right = (GetAsyncKeyState('D') & 0x8000);

    m_isMoving = up || down || left || right;

    if (!m_isMoving) {
        m_legDirection = 0;
        return;
    }

    float moveX = 0, moveY = 0;
    if (up) moveY -= 1;
    if (down) moveY += 1;
    if (left) moveX -= 1;
    if (right) moveX += 1;

    if (moveX != 0 && moveY != 0) {
        moveX *= 0.7071f;
        moveY *= 0.7071f;
    }

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
    // 1. 获取角色在屏幕上的中心点坐标
    float characterCenterX = x + width / 2.0f;
    float characterCenterY = y + height / 2.0f;
    float screenX = (characterCenterX - cameraX) * zoom;
    float screenY = (characterCenterY - cameraY) * zoom;

    // 2. 获取鼠标在窗口内的坐标
    int mouseX = window.getMouseInWindowX();
    int mouseY = window.getMouseInWindowY();

    // 3. 计算从角色中心到鼠标的向量
    float deltaX = mouseX - screenX;
    float deltaY = mouseY - screenY;

    // 关键修正: 反转Y轴以匹配屏幕坐标系(Y向下为正)和标准数学坐标系(Y向上为正)
    m_aimAngle = atan2(-deltaY, deltaX);

    // 4. 将标准数学角度(弧度)转换为0-360度的标准角度
    float standard_degrees = m_aimAngle * 180.0f / M_PI;
    if (standard_degrees < 0) {
        standard_degrees += 360.0f;
    }

    // 5. 将标准角度转换为美术资源的坐标系角度
    // 标准坐标系: 0度朝右(East), 90度朝上(North), 逆时针增长
    // 美术坐标系: 0度朝下(South), 顺时针增长
    // 正确的转换公式: asset_degrees = (270 - standard_degrees + 360) % 360
    float asset_degrees = fmod(270.0f - standard_degrees + 360.0f, 360.0f);

    // 6. 根据美术坐标系角度，计算出对应的动画帧索引(0-31)
    const float anglePerDirection = 11.25f; // 360 / 32
    m_torsoDirection = static_cast<int>((asset_degrees + (anglePerDirection / 2.0f)) / anglePerDirection) % 32;
}


void Hero::Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) {
    if (!isAlive) return;

    auto renderPart = [&](GamesEngineeringBase::Image& sheet, int frameCountX, int frameCountY, int frameIndex, float offsetX, float offsetY) {
        int frameX = frameIndex % frameCountX;
        int frameY = frameIndex / frameCountX;

        int frameWidth = sheet.width / frameCountX;
        int frameHeight = sheet.height / frameCountY;

        int screenX = static_cast<int>((x + offsetX - cameraX) * zoom);
        int screenY = static_cast<int>((y + offsetY - cameraY) * zoom);
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
                    unsigned int srcX = frameX * frameWidth + static_cast<unsigned int>(sx / (m_renderScale * zoom));
                    unsigned int srcY = frameY * frameHeight + static_cast<unsigned int>(sy / (m_renderScale * zoom));

                    if (srcX < sheet.width && srcY < sheet.height) {
                        if (sheet.alphaAt(srcX, srcY) > 200) {
                            canvas.draw(canvasX, canvasY, sheet.at(srcX, srcY));
                        }
                    }
                }
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

void Hero::SetSlowed(bool slowed) {
    m_isSlowed = slowed;
}

bool Hero::CanFire() {
    return m_fireCooldown <= 0;
}

void Hero::ResetFireCooldown() {
    m_fireCooldown = (m_currentWeapon == WeaponType::MACHINE_GUN) ? m_machineGunCooldown : m_cannonCooldown;
}

void Hero::SwitchWeapon() {
    m_currentWeapon = (m_currentWeapon == WeaponType::MACHINE_GUN) ? WeaponType::CANNON : WeaponType::MACHINE_GUN;
    std::cout << "Switched to " << ((m_currentWeapon == WeaponType::MACHINE_GUN) ? "Machine Gun" : "Cannon") << std::endl;
}

float Hero::GetFirePosX() const {
    return x + width / 2.0f;
}

float Hero::GetFirePosY() const {
    return y + height / 2.0f;
}

float Hero::GetTorsoFireAngle() const {
    // 1. 根据上半身动画帧索引，获取其在美术坐标系下的中心角度
    const float anglePerDirection = 11.25f; // 360 / 32
    float asset_degrees = m_torsoDirection * anglePerDirection;

    // 2. 将美术坐标系角度，逆向转换回0-360度的标准数学角度
    // 正确的转换公式: standard_degrees = (270 - asset_degrees + 360) % 360
    float standard_degrees = fmod(270.0f - asset_degrees + 360.0f, 360.0f);

    // 3. 将标准角度转换为弧度，并取反以匹配Y轴向下的游戏坐标系
    return -(standard_degrees * M_PI / 180.0f);
}

