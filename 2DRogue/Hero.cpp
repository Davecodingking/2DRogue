#include "Hero.h"
#include "Level.h"
#include <Windows.h>
#include <cmath>
#include <iostream>

#define _USE_MATH_DEFINES // 为了使用 M_PI
#include <math.h>

// --- 构造函数 ---
// 初始化所有动画相关的变量
Hero::Hero() :
    m_currentLegDirection(0),
    m_currentFrame(0.0f),
    m_animationSpeed(20.0f), // 动画速度可以根据需要调整
    m_isMoving(false),
    m_currentTorsoFrame(0),
    m_torsoOffsetY(-24),      // 新增初始化：负值表示向上偏移10个世界像素
    m_frameWidth(0),
    m_frameHeight(0)
{
    movementSpeed = 200.0f; // 移动速度，单位：像素/秒
}

// --- 析构函数 ---
Hero::~Hero() {
}

// --- 加载函数 ---
// 这个函数现在负责加载所有的动画资源
bool Hero::Load() {
    // 1. 加载下半身行走动画 (8个方向的序列图)
    m_legAnimations.resize(8);
    for (int i = 0; i < 8; ++i) {
        std::string path = "Resources/Mech_Legs_Animations/legs_" + std::to_string(i) + ".png";
        if (!m_legAnimations[i].load(path)) {
            std::cerr << "加载腿部动画失败: " << path << std::endl;
            return false;
        }
    }
    std::cout << "成功加载 8 个方向的腿部动画。" << std::endl;

    // 2. 加载上半身瞄准图像 (32个方向)
    m_torsoImages.resize(32);
    for (int i = 0; i < 32; ++i) {
        // 文件名是从 1 到 32
        std::string path = "Resources/Mech_upper_Animations/hero_upper_run_front_export_dir" + std::to_string(i + 1) + ".png";
        if (!m_torsoImages[i].load(path)) {
            std::cerr << "加载躯干瞄准图失败: " << path << std::endl;
            return false;
        }
    }
    std::cout << "成功加载 32 个方向的躯干瞄准图。" << std::endl;

    // 3. 计算并设置角色的碰撞和渲染尺寸
    // 我们假设所有动画帧的尺寸都是一样的，基于第一张腿部序列图计算
    if (m_legAnimations.empty() || m_legAnimations[0].width == 0) {
        std::cerr << "腿部动画资源为空或尺寸错误，无法设定角色尺寸。" << std::endl;
        return false;
    }
    // 序列图是 5 列 8 行
    m_frameWidth = m_legAnimations[0].width / 5;
    m_frameHeight = m_legAnimations[0].height / 8;
    this->width = m_frameWidth;
    this->height = m_frameHeight;

    return true;
}

// --- 位置设置函数 ---
void Hero::SetPosition(int startX, int startY) {
    this->x = startX;
    this->y = startY;
}

// --- 更新函数 (现在接收 window 引用) ---
void Hero::Update(Level& level, float deltaTime, GamesEngineeringBase::Window& window) {
    if (!isAlive) {
        return;
    }

    // 1. 处理键盘输入并计算期望移动位置
    int newX = x;
    int newY = y;
    float moveAmount = movementSpeed * deltaTime;

    // 使用向量来处理斜向移动速度一致性
    float dx = 0.0f, dy = 0.0f;
    if (window.keyPressed('W')) dy -= 1.0f;
    if (window.keyPressed('S')) dy += 1.0f;
    if (window.keyPressed('A')) dx -= 1.0f;
    if (window.keyPressed('D')) dx += 1.0f;

    if (dx != 0.0f || dy != 0.0f) {
        float length = sqrt(dx * dx + dy * dy);
        dx /= length;
        dy /= length;
        newX += dx * moveAmount;
        newY += dy * moveAmount;
        m_isMoving = true;
    }
    else {
        m_isMoving = false;
    }

    // 2. 碰撞检测并更新最终位置
    CheckMapCollision(level, newX, newY);

    // 3. 核心：更新下半身和上半身的动画状态
    // 注意：这里需要 camera 信息来正确计算鼠标角度
    int cameraX = (int)(this->x - (window.getWidth() / 2.0f / level.getZoom()) + (this->width / 2));
    int cameraY = (int)(this->y - (window.getHeight() / 2.0f / level.getZoom()) + (this->height / 2));
    UpdateAnimations(deltaTime, window, cameraX, cameraY, level.getZoom());
}


// --- 渲染函数 (分层渲染) ---
void Hero::Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) {
    if (!isAlive) {
        return;
    }

    // 计算角色在屏幕上的绘制起始坐标
    int screenX = static_cast<int>(round((x - cameraX) * zoom));
    int screenY = static_cast<int>(round((y - cameraY) * zoom));

    // 1. 绘制下半身 (腿部)
    if (!m_legAnimations.empty()) {
        RenderFrame(canvas, m_legAnimations[m_currentLegDirection], static_cast<int>(m_currentFrame), screenX, screenY, zoom);
    }

    // 2. 绘制上半身 (躯干)
    if (!m_torsoImages.empty()) {
        // 新增：计算上半身的Y坐标，应用偏移量并进行缩放
        int torsoScreenY = screenY + static_cast<int>(round(m_torsoOffsetY * zoom));
        // 使用新的Y坐标来渲染上半身
        RenderFrame(canvas, m_torsoImages[m_currentTorsoFrame], 0, screenX, torsoScreenY, zoom);
    }
}

// --- 动画更新辅助函数 ---
void Hero::UpdateAnimations(float deltaTime, GamesEngineeringBase::Window& window, int cameraX, int cameraY, float zoom) {
    // --- 1. 更新下半身动画 ---
    if (m_isMoving) {
        // 更新动画帧
        m_currentFrame += m_animationSpeed * deltaTime;
        if (m_currentFrame >= 40.0f) { // 腿部动画总共40帧
            m_currentFrame -= 40.0f;
        }

        // 获取键盘状态来决定方向
        bool up = window.keyPressed('W');
        bool down = window.keyPressed('S');
        bool left = window.keyPressed('A');
        bool right = window.keyPressed('D');

        // 根据按键组合设置方向 (0-7)，顺序为从正下方开始逆时针
        if (down && !left && !right) m_currentLegDirection = 0;      // S
        else if (down && left) m_currentLegDirection = 1;         // SA
        else if (left && !up && !down) m_currentLegDirection = 2;   // A
        else if (left && up) m_currentLegDirection = 3;           // WA
        else if (up && !left && !right) m_currentLegDirection = 4;    // W
        else if (up && right) m_currentLegDirection = 5;          // WD
        else if (right && !up && !down) m_currentLegDirection = 6;  // D
        else if (right && down) m_currentLegDirection = 7;        // SD
        // 如果只有两个对冲的键被按下，则保持上一个方向
    }
    else {
        m_currentFrame = 0; // 静止时显示第一帧
    }

    // --- 2. 更新上半身瞄准方向 ---
    int mouseX = window.getMouseInWindowX();
    int mouseY = window.getMouseInWindowY();

    // 计算玩家在屏幕上的中心点
    float playerScreenCenterX = (x - cameraX + width / 2.0f) * zoom;
    float playerScreenCenterY = (y - cameraY + height / 2.0f) * zoom;

    // 计算玩家到鼠标的角度
    float angleRad = atan2(mouseY - playerScreenCenterY, mouseX - playerScreenCenterX);
    float angleDeg = angleRad * 180.0f / M_PI;

    // 将角度从 (-180, 180] 转换到 [0, 360)
    if (angleDeg < 0) {
        angleDeg += 360.0f;
    }

    // 将角度映射到 32 个方向中的一个
    // 每个方向覆盖 360 / 32 = 11.25 度
    // 我们需要偏移半个角度的量，使得 0 度正好落在第0个区间的中间
    m_currentTorsoFrame = static_cast<int>(round(angleDeg / 11.25f)) % 32;
}

// --- 地图碰撞检测 ---
void Hero::CheckMapCollision(Level& level, int newX, int newY) {
    // 简单的矩形碰撞，检查角色的四个角
    // 注意：这里只是一个基础实现，更精确的碰撞需要考虑角色的碰撞盒
    int leftTile = newX / TILE_SIZE;
    int rightTile = (newX + width - 1) / TILE_SIZE;
    int topTile = newY / TILE_SIZE;
    int bottomTile = (newY + height - 1) / TILE_SIZE;

    // 检查 X 轴移动
    int tempX = x;
    if (newX != x) {
        if (newX > x) { // 向右移动
            if (!level.isObstacleAt(rightTile, topTile) && !level.isObstacleAt(rightTile, bottomTile)) {
                x = newX;
            }
        }
        else { // 向左移动
            if (!level.isObstacleAt(leftTile, topTile) && !level.isObstacleAt(leftTile, bottomTile)) {
                x = newX;
            }
        }
    }

    // 更新碰撞瓦片坐标，然后检查 Y 轴移动
    leftTile = x / TILE_SIZE;
    rightTile = (x + width - 1) / TILE_SIZE;
    topTile = newY / TILE_SIZE;
    bottomTile = (newY + height - 1) / TILE_SIZE;

    if (newY != y) {
        if (newY > y) { // 向下移动
            if (!level.isObstacleAt(leftTile, bottomTile) && !level.isObstacleAt(rightTile, bottomTile)) {
                y = newY;
            }
        }
        else { // 向上移动
            if (!level.isObstacleAt(leftTile, topTile) && !level.isObstacleAt(rightTile, topTile)) {
                y = newY;
            }
        }
    }
}


// --- 渲染单帧辅助函数 ---
void Hero::RenderFrame(GamesEngineeringBase::Window& canvas, GamesEngineeringBase::Image& spriteSheet, int frameIndex, int screenX, int screenY, float zoom) {
    if (spriteSheet.width == 0) return;

    // 对于上半身瞄准图，它们不是序列图，宽度就是帧宽度
    int sheetFrameWidth = m_frameWidth;
    int sheetColumns = spriteSheet.width / sheetFrameWidth;
    if (sheetColumns == 0) sheetColumns = 1; // 避免除零

    // 计算当前帧在序列图中的左上角坐标
    int sourceX = (frameIndex % sheetColumns) * sheetFrameWidth;
    int sourceY = (frameIndex / sheetColumns) * m_frameHeight;

    // 如果是单张图片（比如上半身），它的尺寸就是帧的尺寸
    int currentFrameWidth = (spriteSheet.width < (unsigned int)m_frameWidth) ? spriteSheet.width : m_frameWidth;
    int currentFrameHeight = (spriteSheet.height < (unsigned int)m_frameHeight) ? spriteSheet.height : m_frameHeight;

    // 计算缩放后的渲染尺寸
    int scaledWidth = static_cast<int>(round(currentFrameWidth * zoom));
    int scaledHeight = static_cast<int>(round(currentFrameHeight * zoom));
    int screenX_end = screenX + scaledWidth;
    int screenY_end = screenY + scaledHeight;

    if (scaledWidth <= 0 || scaledHeight <= 0) return;

    // 遍历屏幕像素进行绘制
    for (int sy = screenY; sy < screenY_end; ++sy) {
        for (int sx = screenX; sx < screenX_end; ++sx) {
            if (sx >= 0 && sx < (int)canvas.getWidth() && sy >= 0 && sy < (int)canvas.getHeight()) {
                // 将屏幕坐标映射回源图坐标
                unsigned int src_px = sourceX + static_cast<unsigned int>((double)(sx - screenX) / scaledWidth * currentFrameWidth);
                unsigned int src_py = sourceY + static_cast<unsigned int>((double)(sy - screenY) / scaledHeight * currentFrameHeight);

                // 边界检查
                if (src_px >= spriteSheet.width) src_px = spriteSheet.width - 1;
                if (src_py >= spriteSheet.height) src_py = spriteSheet.height - 1;

                // 透明度检查
                if (spriteSheet.alphaAt(src_px, src_py) > 200) {
                    canvas.draw(sx, sy, spriteSheet.at(src_px, src_py));
                }
            }
        }
    }
}

