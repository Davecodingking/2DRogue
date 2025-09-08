#include "NPC.h"
#include "Level.h"
#include <cmath>
#include <iostream>

NPC::NPC() {
    // 构造函数中可以设置一些默认值
    movementSpeed = 2.0f; // NPC 默认速度比玩家慢
    currentHealth = 50;
    maxHealth = 50;
}

NPC::~NPC() {
}

bool NPC::Load(const std::string& filename) {
    if (!npcImage.load(filename)) {
        std::cerr << "加载NPC图片失败: " << filename << std::endl;
        return false;
    }
    // 使用图片尺寸设置NPC的碰撞尺寸
    this->width = npcImage.width;
    this->height = npcImage.height;
    return true;
}

void NPC::SetPosition(int startX, int startY) {
    this->x = startX;
    this->y = startY;
}

void NPC::InitializeStats(int health, float speed) {
    this->currentHealth = health;
    this->maxHealth = health;
    this->movementSpeed = speed;
}

// Update 函数每一帧都会被调用
void NPC::Update(Level& level, float deltaTime) {
    // 如果NPC死亡，则不执行任何逻辑
    if (!isAlive) {
        return;
    }
    // 目前，NPC的自主更新逻辑是空的。
    // 它的移动将由 Game 类的 AI 逻辑部分调用 MoveTowards() 来驱动。
    // 未来我们可以在这里添加动画、状态切换等逻辑。
}

// AI 移动逻辑
void NPC::MoveTowards(int targetX, int targetY) {
    if (!isAlive) {
        return;
    }

    // 计算从NPC到目标的向量
    float dirX = (float)targetX - this->x;
    float dirY = (float)targetY - this->y;

    // 计算向量的长度（距离）
    float length = sqrt(dirX * dirX + dirY * dirY);

    // 如果距离很近，就不用移动了，避免抖动
    if (length < 1.0f) {
        return;
    }

    // 将向量单位化（长度变为1），这样移动速度就不会受距离影响
    dirX /= length;
    dirY /= length;

    // 根据方向和速度，更新NPC的位置
    // 这里我们暂时不考虑NPC之间的碰撞和与地形的碰撞
    this->x += dirX * movementSpeed;
    this->y += dirY * movementSpeed;
}

void NPC::Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) {
    if (!isAlive) {
        return;
    }

    // 这段渲染代码和 Hero::Render 中的逻辑完全一样
    int screen_x_start = static_cast<int>(round((x - cameraX) * zoom));
    int screen_y_start = static_cast<int>(round((y - cameraY) * zoom));
    int screen_x_end = static_cast<int>(round((x + width - cameraX) * zoom));
    int screen_y_end = static_cast<int>(round((y + height - cameraY) * zoom));

    if (screen_x_start == screen_x_end || screen_y_start == screen_y_end) {
        return;
    }

    for (int screen_y = screen_y_start; screen_y < screen_y_end; ++screen_y) {
        for (int screen_x = screen_x_start; screen_x < screen_x_end; ++screen_x) {
            if (screen_x >= 0 && screen_x < (int)canvas.getWidth() && screen_y >= 0 && screen_y < (int)canvas.getHeight()) {
                unsigned int src_pixel_x = static_cast<unsigned int>((double)(screen_x - screen_x_start) / (double)(screen_x_end - screen_x_start) * width);
                unsigned int src_pixel_y = static_cast<unsigned int>((double)(screen_y - screen_y_start) / (double)(screen_y_end - screen_y_start) * height);

                if (src_pixel_x >= npcImage.width) src_pixel_x = npcImage.width - 1;
                if (src_pixel_y >= npcImage.height) src_pixel_y = npcImage.height - 1;

                if (npcImage.alphaAt(src_pixel_x, src_pixel_y) > 200) {
                    canvas.draw(screen_x, screen_y, npcImage.at(src_pixel_x, src_pixel_y));
                }
            }
        }
    }
}
