#pragma once
#pragma once
#include "Character.h"
#include "GamesEngineeringBase.h"
#include <string>

// Hero 类继承自 Character 类
// "public Character" 意味着 Character 类中所有的 public 成员在 Hero 类中也是 public，
// protected 成员在 Hero 中也是 protected。
class Hero : public Character {
public:
    // 构造函数
    Hero();
    // 析构函数
    ~Hero();

    // --- 核心功能 ---
    // 加载英雄所需的资源（例如图片）
    bool Load(const std::string& filename);

    // 设置英雄的初始位置
    void SetPosition(int startX, int startY);

    // --- 重写 (Override) 基类的虚函数 ---
    // override 关键字告诉编译器，我们正在特意重写基类中的同名虚函数，
    // 如果基类中没有这个函数或者函数签名不匹配，编译器会报错，这能防止很多错误。
    void Update(Level& level, float deltaTime) override;
    void Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) override;

private:
    // --- 私有成员变量 ---
    // 英雄的外观图片
    GamesEngineeringBase::Image playerImage;

    // 图块的大小，用于碰撞检测计算
    const int TILE_SIZE = 32;

    // --- 私有辅助函数 ---
    // 专门处理玩家输入
    void HandleInput();
    // 专门处理与地图的碰撞
    void CheckMapCollision(Level& level, int newX, int newY);
};
