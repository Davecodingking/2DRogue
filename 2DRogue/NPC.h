#pragma once
#include "Character.h"
#include "GamesEngineeringBase.h"
#include <string>

// NPC 类，同样继承自 Character
class NPC : public Character {
public:
    // 构造函数
    NPC();
    // 析构函数
    ~NPC();

    // 加载NPC所需资源（外观图片）
    bool Load(const std::string& filename);

    // 设置NPC的初始位置
    void SetPosition(int startX, int startY);

    // 初始化NPC的属性（生命值、速度等）
    void InitializeStats(int health, float speed);

    // --- 重写 (Override) 基类的虚函数 ---
    void Update(Level& level, float deltaTime) override;
    void Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) override;

    // --- NPC 独有的行为 ---
    // AI 逻辑：让 NPC 朝着一个目标坐标移动
    void MoveTowards(int targetX, int targetY);

private:
    // NPC的外观图片
    GamesEngineeringBase::Image npcImage;
    const int TILE_SIZE = 32;
};
