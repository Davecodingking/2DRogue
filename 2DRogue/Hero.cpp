#include "Hero.h"
#include "Level.h"
#include <Windows.h> // 用于 GetAsyncKeyState
#include <cmath>     // 用于 round()
#include <iostream>  // 用于打印调试信息

// --- 构造函数 ---
// 初始化英雄的默认属性，并将移动速度设置为一个初始值。
Hero::Hero() {
    movementSpeed = 5.0f; // 这个值之前是在 main.cpp 中定义的 PLAYER_SPEED
}

// --- 析构函数 ---
// 目前是空的。如果以后有需要动态分配的内存，我们会在这里释放。
Hero::~Hero() {
}

// --- 加载函数 ---
// 从一个文件路径加载英雄的图像。
bool Hero::Load(const std::string& filename) {
    if (!playerImage.load(filename)) {
        // 如果加载失败，输出错误信息。
        std::cerr << "加载英雄图片失败: " << filename << std::endl;
        return false;
    }
    // 加载成功后，用图片的尺寸来设置英雄的宽度和高度。
    // 这对于渲染和碰撞检测至关重要。
    this->width = playerImage.width;
    this->height = playerImage.height;
    return true;
}

// --- 位置设置函数 ---
// 在游戏世界中设置英雄的初始坐标。
void Hero::SetPosition(int startX, int startY) {
    this->x = startX;
    this->y = startY;
}

// --- 更新函数 (核心逻辑) ---
// 这个函数每一帧都会被调用，处理英雄的所有行为。
void Hero::Update(Level& level, float deltaTime) {
    // 如果英雄已经死亡，则不执行任何更新逻辑。
    if (!isAlive) {
        return;
    }

    // 1. 处理玩家输入，并计算出“期望”移动到的新位置。
    int newX = x;
    int newY = y;

    // 使用 GetAsyncKeyState 检查键盘按键是否被按下。
    if ((GetAsyncKeyState(VK_UP) & 0x8000) || (GetAsyncKeyState('W') & 0x8000))    newY -= movementSpeed;
    if ((GetAsyncKeyState(VK_DOWN) & 0x8000) || (GetAsyncKeyState('S') & 0x8000))  newY += movementSpeed;
    if ((GetAsyncKeyState(VK_LEFT) & 0x8000) || (GetAsyncKeyState('A') & 0x8000))  newX -= movementSpeed;
    if ((GetAsyncKeyState(VK_RIGHT) & 0x8000) || (GetAsyncKeyState('D') & 0x8000)) newX += movementSpeed;

    // 2. 将期望的新位置传递给碰撞检测函数。
    // 这个函数会判断是否可以移动，并最终更新英雄的实际位置。
    CheckMapCollision(level, newX, newY);
}

// --- 渲染函数 ---
// 将英雄绘制到屏幕上。
void Hero::Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) {
    // 如果英雄死亡，则不进行绘制。
    if (!isAlive) {
        return;
    }

    // --- 这段渲染代码几乎完全是从你原来的 main.cpp 中迁移过来的 ---

    // 根据英雄的世界坐标、摄像机位置和缩放级别，计算出它在屏幕上的绘制区域。
    int screen_x_start = static_cast<int>(round((x - cameraX) * zoom));
    int screen_y_start = static_cast<int>(round((y - cameraY) * zoom));
    int screen_x_end = static_cast<int>(round((x + width - cameraX) * zoom));
    int screen_y_end = static_cast<int>(round((y + height - cameraY) * zoom));

    // 如果缩放后尺寸为0，则无需绘制，避免除零错误。
    if (screen_x_start == screen_x_end || screen_y_start == screen_y_end) {
        return;
    }

    // 遍历屏幕上的每一个像素点来进行绘制。
    for (int screen_y = screen_y_start; screen_y < screen_y_end; ++screen_y) {
        for (int screen_x = screen_x_start; screen_x < screen_x_end; ++screen_x) {

            // 确保只在窗口的可视范围内绘制。
            if (screen_x >= 0 && screen_x < (int)canvas.getWidth() && screen_y >= 0 && screen_y < (int)canvas.getHeight()) {

                // 将屏幕坐标映射回原始图片上的纹理坐标。
                unsigned int src_pixel_x = static_cast<unsigned int>((double)(screen_x - screen_x_start) / (double)(screen_x_end - screen_x_start) * width);
                unsigned int src_pixel_y = static_cast<unsigned int>((double)(screen_y - screen_y_start) / (double)(screen_y_end - screen_y_start) * height);

                // 边界检查，防止读取图片数据时越界。
                if (src_pixel_x >= playerImage.width) src_pixel_x = playerImage.width - 1;
                if (src_pixel_y >= playerImage.height) src_pixel_y = playerImage.height - 1;

                // 检查透明度 (alpha)，只绘制不透明的像素。
                if (playerImage.alphaAt(src_pixel_x, src_pixel_y) > 200) {
                    canvas.draw(screen_x, screen_y, playerImage.at(src_pixel_x, src_pixel_y));
                }
            }
        }
    }
}


// --- 私有辅助函数：地图碰撞检测 ---
void Hero::CheckMapCollision(Level& level, int newX, int newY) {
    // 计算期望位置对应的地图图块坐标。
    int playerTileX = newX / TILE_SIZE;
    int playerTileY = newY / TILE_SIZE;

    // 检查目标图块是否是障碍物。
    if (!level.isObstacleAt(playerTileX, playerTileY)) {
        // 如果不是障碍物，则允许移动，更新英雄的实际位置。
        x = newX;
        y = newY;
    }
    // 如果是障碍物，我们什么都不做，英雄的位置保持不变。

    // 如果地图不是无限模式，还需要进行边界检查。
    if (!level.isInfiniteMode()) {
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x > level.getWidth() * TILE_SIZE - width) {
            x = level.getWidth() * TILE_SIZE - width;
        }
        if (y > level.getHeight() * TILE_SIZE - height) {
            y = level.getHeight() * TILE_SIZE - height;
        }
    }
}
