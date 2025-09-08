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
    Level level;
    // 將這裡更改為您想要測試的關卡文件
    if (!level.loadFromFile("Resources/level1.json")) {
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
    const GameObject* objects = level.getGameObjects();
    bool spawnPointFound = false;
    for (int i = 0; i < level.getObjectCount(); ++i) {
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

    // --- 新增：縮放控制變量 ---
    float currentZoom = 0.5f; // 初始縮放為 0.5x
    level.setZoom(currentZoom);
    bool plus_pressed = false;  // 用於防止按鍵連發
    bool minus_pressed = false; // 用於防止按鍵連發

    // --- 3. 主遊戲循環 ---
    while (running)
    {
        canvas.checkInput();

        // --- 更新邏輯 ---
        float dt = timer.dt();

        // --- 新增：在 0.5x 和 1.0x 之間切換的縮放邏輯 ---
        if (canvas.keyPressed(VK_OEM_PLUS) && !plus_pressed) {
            currentZoom = 1.0f;
            level.setZoom(currentZoom);
            plus_pressed = true;
        }
        if (!canvas.keyPressed(VK_OEM_PLUS)) {
            plus_pressed = false;
        }

        if (canvas.keyPressed(VK_OEM_MINUS) && !minus_pressed) {
            currentZoom = 0.5f;
            level.setZoom(currentZoom);
            minus_pressed = true;
        }
        if (!canvas.keyPressed(VK_OEM_MINUS)) {
            minus_pressed = false;
        }

        // --- 玩家移動與碰撞邏輯 ---
        int move_amount = static_cast<int>(MOVE_SPEED * dt);
        if (move_amount < 1 && dt > 0.001) move_amount = 1; // 保证最小移动速度

        // 水平移動
        int dx = 0;
        if (canvas.keyPressed('A')) { dx -= move_amount; }
        if (canvas.keyPressed('D')) { dx += move_amount; }
        if (dx != 0) {
            int nextX = planeX + dx;
            int collisionTileX = (dx > 0) ? (nextX + playerImage.width - 1) / 32 : nextX / 32;
            int topTileY = planeY / 32;
            int bottomTileY = (planeY + playerImage.height - 1) / 32;
            bool collision = false;
            for (int ty = topTileY; ty <= bottomTileY; ++ty) {
                if (level.isObstacleAt(collisionTileX, ty)) {
                    collision = true;
                    break;
                }
            }
            if (!collision) { planeX = nextX; }
        }

        // 垂直移動
        int dy = 0;
        if (canvas.keyPressed('W')) { dy -= move_amount; }
        if (canvas.keyPressed('S')) { dy += move_amount; }
        if (dy != 0) {
            int nextY = planeY + dy;
            int collisionTileY = (dy > 0) ? (nextY + playerImage.height - 1) / 32 : nextY / 32;
            int leftTileX = planeX / 32;
            int rightTileX = (planeX + playerImage.width - 1) / 32;
            bool collision = false;
            for (int tx = leftTileX; tx <= rightTileX; ++tx) {
                if (level.isObstacleAt(tx, collisionTileY)) {
                    collision = true;
                    break;
                }
            }
            if (!collision) { planeY = nextY; }
        }

        // --- 攝像機控制 (已更新以適應縮放) ---
        int viewWidthInWorld = static_cast<int>(canvas.getWidth() / currentZoom);
        int viewHeightInWorld = static_cast<int>(canvas.getHeight() / currentZoom);

        int cameraTargetX = planeX + (playerImage.width / 2) - (viewWidthInWorld / 2);
        int cameraTargetY = planeY + (playerImage.height / 2) - (viewHeightInWorld / 2);

        int worldWidthPixels = level.getWidth() * 32;
        int worldHeightPixels = level.getHeight() * 32;
        if (cameraTargetX < 0) cameraTargetX = 0;
        if (cameraTargetY < 0) cameraTargetY = 0;
        if (cameraTargetX > worldWidthPixels - viewWidthInWorld) cameraTargetX = worldWidthPixels - viewWidthInWorld;
        if (cameraTargetY > worldHeightPixels - viewHeightInWorld) cameraTargetY = worldHeightPixels - viewHeightInWorld;

        level.setCameraPosition(cameraTargetX, cameraTargetY);


        // --- 渲染邏輯 ---
        canvas.clear();

        level.render(canvas);

        // --- 最终修正版：渲染玩家 (使用逆向映射) ---
        // 1. 计算玩家在屏幕上的显示区域
        int player_screen_x_start = static_cast<int>(round((planeX - cameraTargetX) * currentZoom));
        int player_screen_y_start = static_cast<int>(round((planeY - cameraTargetY) * currentZoom));
        int player_screen_x_end = static_cast<int>(round((planeX + playerImage.width - cameraTargetX) * currentZoom));
        int player_screen_y_end = static_cast<int>(round((planeY + playerImage.height - cameraTargetY) * currentZoom));

        // 2. 遍历这个屏幕区域的每一个像素
        for (int screen_y = player_screen_y_start; screen_y < player_screen_y_end; ++screen_y) {
            for (int screen_x = player_screen_x_start; screen_x < player_screen_x_end; ++screen_x) {

                // 视锥裁剪
                if (screen_x >= 0 && screen_x < (int)canvas.getWidth() && screen_y >= 0 && screen_y < (int)canvas.getHeight()) {

                    // 3. 反向精确计算该屏幕像素对应源图片(A.png)的哪个像素
                    unsigned int src_pixel_x = static_cast<unsigned int>((float)(screen_x - player_screen_x_start) / (player_screen_x_end - player_screen_x_start) * playerImage.width);
                    unsigned int src_pixel_y = static_cast<unsigned int>((float)(screen_y - player_screen_y_start) / (player_screen_y_end - player_screen_y_start) * playerImage.height);

                    // 边界保护
                    if (src_pixel_x >= playerImage.width) src_pixel_x = playerImage.width - 1;
                    if (src_pixel_y >= playerImage.height) src_pixel_y = playerImage.height - 1;

                    if (playerImage.alphaAt(src_pixel_x, src_pixel_y) > 200) {
                        canvas.draw(screen_x, screen_y, playerImage.at(src_pixel_x, src_pixel_y));
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

