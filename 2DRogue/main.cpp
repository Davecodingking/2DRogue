#include "GamesEngineeringBase.h" // 包含框架頭文件
#include "Level.h"                // 包含我們新的 Level 類
#include <iostream>               // 用於打印日誌

int main(int argc, char* argv[])
{
    // --- 1. 初始化 ---
    GamesEngineeringBase::Window canvas;
    canvas.create(1024, 768, "2D Rogue Game"); // 創建視窗
    bool running = true;

    // 創建 Level 對象並加載地圖文件
    Level level1;
    if (!level1.loadFromFile("Resources/level1.json")) {
        std::cerr << "Failed to load level, exiting." << std::endl;
        system("pause"); // 在退出前暫停，以便看到錯誤訊息
        return -1;
    }

    // 加載玩家圖片
    GamesEngineeringBase::Image playerImage;
    if (!playerImage.load("Resources/A.png")) {
        std::cerr << "Failed to load player image." << std::endl;
        system("pause");
        return -1;
    }

    // 聲明玩家在世界中的像素座標 (使用 int 類型更靈活)
    int planeX = 0;
    int planeY = 0;

    // --- 2. 設置玩家初始位置 ---
    const GameObject* objects = level1.getGameObjects();
    bool spawnPointFound = false;
    for (int i = 0; i < level1.getObjectCount(); ++i) {
        if (objects[i].type == "hero_respawn") {
            planeX = objects[i].x;
            planeY = objects[i].y;
            spawnPointFound = true;
            std::cout << "Player spawn point found at (" << planeX << ", " << planeY << ")" << std::endl;
            break; // 找到後就退出循環
        }
    }
    if (!spawnPointFound) {
        std::cerr << "Warning: 'hero_respawn' object not found in map. Placing player at (100, 100)." << std::endl;
        planeX = 100;
        planeY = 100;
    }

    GamesEngineeringBase::Timer timer;
    float MOVE_SPEED = 200.0f;

    // --- 3. 主遊戲循環 ---
    while (running)
    {
        canvas.checkInput();

        // --- 更新邏輯 ---
        float dt = timer.dt();
        int move_amount = static_cast<int>(MOVE_SPEED * dt);
        if (move_amount < 1) move_amount = 1;

        // =======================================================================
        // !! 已重寫的移動與碰撞邏輯 !!
        // =======================================================================

        // --- 處理水平移動 ---
        int dx = 0;
        if (canvas.keyPressed('A')) { dx -= move_amount; }
        if (canvas.keyPressed('D')) { dx += move_amount; }

        if (dx != 0) {
            int nextX = planeX + dx;
            // 根據移動方向，檢查包圍盒的左側或右側邊緣
            int collisionTileX = (dx > 0) ? (nextX + playerImage.width) / 32 : nextX / 32;
            int topTileY = planeY / 32;
            int bottomTileY = (planeY + playerImage.height - 1) / 32;

            // 只要移動路徑上的任何一個圖塊是障礙物，就不移動
            bool collision = false;
            for (int ty = topTileY; ty <= bottomTileY; ++ty) {
                if (level1.isObstacleAt(collisionTileX, ty)) {
                    collision = true;
                    break;
                }
            }
            if (!collision) {
                planeX = nextX;
            }
        }

        // --- 處理垂直移動 ---
        int dy = 0;
        if (canvas.keyPressed('W')) { dy -= move_amount; }
        if (canvas.keyPressed('S')) { dy += move_amount; }

        if (dy != 0) {
            int nextY = planeY + dy;
            // 根據移動方向，檢查包圍盒的頂部或底部邊緣
            int collisionTileY = (dy > 0) ? (nextY + playerImage.height) / 32 : nextY / 32;
            int leftTileX = planeX / 32;
            int rightTileX = (planeX + playerImage.width - 1) / 32;

            bool collision = false;
            for (int tx = leftTileX; tx <= rightTileX; ++tx) {
                if (level1.isObstacleAt(tx, collisionTileY)) {
                    collision = true;
                    break;
                }
            }
            if (!collision) {
                planeY = nextY;
            }
        }

        // --- 攝像機控制 ---
        int cameraTargetX = planeX + (playerImage.width / 2) - (canvas.getWidth() / 2);
        int cameraTargetY = planeY + (playerImage.height / 2) - (canvas.getHeight() / 2);

        int worldWidthPixels = level1.getWidth() * 32;
        int worldHeightPixels = level1.getHeight() * 32;
        if (cameraTargetX < 0) cameraTargetX = 0;
        if (cameraTargetY < 0) cameraTargetY = 0;
        if (cameraTargetX > worldWidthPixels - canvas.getWidth()) cameraTargetX = worldWidthPixels - canvas.getWidth();
        if (cameraTargetY > worldHeightPixels - canvas.getHeight()) cameraTargetY = worldHeightPixels - canvas.getHeight();

        level1.setCameraPosition(cameraTargetX, cameraTargetY);


        // --- 渲染邏輯 ---
        canvas.clear();

        level1.render(canvas);

        int playerScreenX = planeX - cameraTargetX;
        int playerScreenY = planeY - cameraTargetY;

        for (unsigned int i = 0; i < playerImage.height; i++) {
            for (unsigned int n = 0; n < playerImage.width; n++) {
                int targetX = playerScreenX + n;
                int targetY = playerScreenY + i;
                if (targetX >= 0 && targetX < canvas.getWidth() && targetY >= 0 && targetY < canvas.getHeight()) {
                    if (playerImage.alphaAt(n, i) > 200) {
                        canvas.draw(targetX, targetY, playerImage.at(n, i));
                    }
                }
            }
        }

        canvas.present();

        if (canvas.keyPressed(VK_ESCAPE)) {
            running = false;
        }
    }
    return 0;
}

