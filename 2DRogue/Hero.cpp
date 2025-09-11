#include "Hero.h"
#include "Level.h"
#include <Windows.h> 
#include <cmath>     
#include <iostream>  
#include <string>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Hero::Hero() {
    // 初始化渲染参数
    m_renderScale = 1.5f;
    m_torsoOffsetY = -20 * m_renderScale; // 躯干相对于腿部的Y轴偏移

    // 初始化动画控制变量
    m_currentLegsDirection = 0; // 默认朝下
    m_currentFrame = 0.0f;
    m_animationSpeed = 10.0f;
    m_currentTorsoFrame = 0;

    // 初始化状态
    m_isMoving = false;
    m_isSlowed = false;

    // 初始化基类属性
    movementSpeed = 200.0f; // 移动速度（单位：像素/秒）
    isAlive = true;
    maxHealth = 100;
    currentHealth = 100;
}

Hero::~Hero() {}

bool Hero::Load() {
    // 加载下半身动画 (8个方向)
    for (int i = 0; i < 8; ++i) {
        std::string path = "Resources/Mech_Legs_Animations/legs_" + std::to_string(i) + ".png";
        if (!m_legsAnimationSheets[i].load(path)) {
            std::cerr << "加载英雄下半身图片失败: " << path << std::endl;
            return false;
        }
    }

    // 加载上半身动画 (32个方向)
    for (int i = 0; i < 32; ++i) {
        std::string path = "Resources/Mech_upper_Animations/hero_upper_run_front_export_dir" + std::to_string(i + 1) + ".png";
        if (!m_torsoAnimationSheets[i].load(path)) {
            std::cerr << "加载英雄上半身图片失败: " << path << std::endl;
            return false;
        }
    }

    // 根据第一张图设置碰撞尺寸
    if (m_legsAnimationSheets[0].width > 0) {
        this->width = static_cast<int>(m_legsAnimationSheets[0].width / 5 * m_renderScale);
        this->height = static_cast<int>(m_legsAnimationSheets[0].height / 8 * m_renderScale);
    }

    return true;
}

void Hero::SetPosition(float startX, float startY) {
    this->x = startX;
    this->y = startY;
}

void Hero::SetSlowed(bool isSlowed) {
    m_isSlowed = isSlowed;
}

void Hero::Update(Level& level, float deltaTime) {
    if (!isAlive) return;

    // --- 关键修正：将level对象传递给HandleInput ---
    HandleInput(level, deltaTime);

    if (m_isMoving) {
        m_currentFrame += m_animationSpeed * deltaTime;
        if (m_currentFrame >= 40) {
            m_currentFrame -= 40;
        }
    }
    else {
        m_currentFrame = 0; // 静止时显示第一帧
    }
}

void Hero::UpdateAiming(GamesEngineeringBase::Window& window, int cameraX, int cameraY, float zoom) {
    if (!isAlive) return;

    float torsoWorldX = x;
    float torsoWorldY = y + m_torsoOffsetY;

    float torsoCenterX_world = torsoWorldX + width / 2.0f;
    float torsoCenterY_world = torsoWorldY + height / 2.0f;

    float playerScreenX = (torsoCenterX_world - cameraX) * zoom;
    float playerScreenY = (torsoCenterY_world - cameraY) * zoom;

    int mouseX = window.getMouseInWindowX();
    int mouseY = window.getMouseInWindowY();

    float deltaX = mouseX - playerScreenX;
    float deltaY = mouseY - playerScreenY;

    float angle = atan2(deltaY, deltaX) * 180.0f / M_PI;
    if (angle < 0) {
        angle += 360;
    }

    m_currentTorsoFrame = static_cast<int>((angle + 5.625f) / 11.25f) % 32;
}

void Hero::Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) {
    if (!isAlive) return;

    // 1. 绘制下半身
    GamesEngineeringBase::Image& legSheet = m_legsAnimationSheets[m_currentLegsDirection];
    int frameIndex = static_cast<int>(m_currentFrame);
    int frameWidth = legSheet.width / 5;
    int frameHeight = legSheet.height / 8;
    int frameX = (frameIndex % 5) * frameWidth;
    int frameY = (frameIndex / 5) * frameHeight;

    int screenX_start = static_cast<int>(round((x - cameraX) * zoom));
    int screenY_start = static_cast<int>(round((y - cameraY) * zoom));
    int screenX_end = static_cast<int>(round((x + width - cameraX) * zoom));
    int screenY_end = static_cast<int>(round((y + height - cameraY) * zoom));

    if (screenX_start != screenX_end && screenY_start != screenY_end) {
        for (int screenY = screenY_start; screenY < screenY_end; ++screenY) {
            for (int screenX = screenX_start; screenX < screenX_end; ++screenX) {
                if (screenX >= 0 && screenX < (int)canvas.getWidth() && screenY >= 0 && screenY < (int)canvas.getHeight()) {
                    unsigned int src_pixel_x = frameX + static_cast<unsigned int>((double)(screenX - screenX_start) / (double)(screenX_end - screenX_start) * frameWidth);
                    unsigned int src_pixel_y = frameY + static_cast<unsigned int>((double)(screenY - screenY_start) / (double)(screenY_end - screenY_start) * frameHeight);
                    if (legSheet.alphaAt(src_pixel_x, src_pixel_y) > 200) {
                        canvas.draw(screenX, screenY, legSheet.at(src_pixel_x, src_pixel_y));
                    }
                }
            }
        }
    }

    // 2. 绘制上半身
    GamesEngineeringBase::Image& torsoSheet = m_torsoAnimationSheets[m_currentTorsoFrame];

    int torsoScreenX_start = static_cast<int>(round((x - cameraX) * zoom));
    int torsoScreenY_start = static_cast<int>(round((y + m_torsoOffsetY - cameraY) * zoom));
    int torsoScreenX_end = static_cast<int>(round((x + width - cameraX) * zoom));
    int torsoScreenY_end = static_cast<int>(round((y + m_torsoOffsetY + height - cameraY) * zoom));

    if (torsoScreenX_start != torsoScreenX_end && torsoScreenY_start != torsoScreenY_end) {
        for (int screenY = torsoScreenY_start; screenY < torsoScreenY_end; ++screenY) {
            for (int screenX = torsoScreenX_start; screenX < torsoScreenX_end; ++screenX) {
                if (screenX >= 0 && screenX < (int)canvas.getWidth() && screenY >= 0 && screenY < (int)canvas.getHeight()) {
                    unsigned int src_pixel_x = static_cast<unsigned int>((double)(screenX - torsoScreenX_start) / (double)(torsoScreenX_end - torsoScreenX_start) * torsoSheet.width);
                    unsigned int src_pixel_y = static_cast<unsigned int>((double)(screenY - torsoScreenY_start) / (double)(torsoScreenY_end - torsoScreenY_start) * torsoSheet.height);
                    if (torsoSheet.alphaAt(src_pixel_x, src_pixel_y) > 200) {
                        canvas.draw(screenX, screenY, torsoSheet.at(src_pixel_x, src_pixel_y));
                    }
                }
            }
        }
    }
}

// --- 关键修正：让HandleInput接收Level对象 ---
void Hero::HandleInput(Level& level, float deltaTime) {
    bool keyW = (GetAsyncKeyState('W') & 0x8000);
    bool keyS = (GetAsyncKeyState('S') & 0x8000);
    bool keyA = (GetAsyncKeyState('A') & 0x8000);
    bool keyD = (GetAsyncKeyState('D') & 0x8000);

    float dx = 0.0f;
    float dy = 0.0f;

    if (keyW) dy -= 1.0f;
    if (keyS) dy += 1.0f;
    if (keyA) dx -= 1.0f;
    if (keyD) dx += 1.0f;

    m_isMoving = (dx != 0.0f || dy != 0.0f);

    if (m_isMoving) {
        // 更新动画方向
        if (keyS && !keyA && !keyD) m_currentLegsDirection = 0; // S
        else if (keyS && keyA) m_currentLegsDirection = 1;      // SA
        else if (keyA && !keyW && !keyS) m_currentLegsDirection = 2; // A
        else if (keyA && keyW) m_currentLegsDirection = 3;      // AW
        else if (keyW && !keyA && !keyD) m_currentLegsDirection = 4; // W
        else if (keyW && keyD) m_currentLegsDirection = 5;      // WD
        else if (keyD && !keyW && !keyS) m_currentLegsDirection = 6; // D
        else if (keyD && keyS) m_currentLegsDirection = 7;      // DS

        float length = sqrt(dx * dx + dy * dy);
        if (length > 0) {
            dx /= length;
            dy /= length;
        }

        float currentSpeed = m_isSlowed ? (movementSpeed / 2.0f) : movementSpeed;

        float newX = x + dx * currentSpeed * deltaTime;
        float newY = y + dy * currentSpeed * deltaTime;

        // 现在可以正常调用了
        CheckMapCollision(level, newX, newY);
    }
}

void Hero::CheckMapCollision(Level& level, float newX, float newY) {

    float corners[4][2] = {
        {newX, newY},
        {newX + width, newY},
        {newX, newY + height},
        {newX + width, newY + height}
    };

    bool collision = false;
    for (int i = 0; i < 4; ++i) {
        int tileX = static_cast<int>(corners[i][0] / 32);
        int tileY = static_cast<int>(corners[i][1] / 32);
        if (level.isObstacleAt(tileX, tileY)) {
            collision = true;
            break;
        }
    }

    if (!collision) {
        x = newX;
        y = newY;
    }

    if (!level.isInfiniteMode()) {
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x > level.getWidth() * 32 - width) {
            x = level.getWidth() * 32 - width;
        }
        if (y > level.getHeight() * 32 - height) {
            y = level.getHeight() * 32 - height;
        }
    }
}

