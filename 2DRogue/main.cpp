#include "GamesEngineeringBase.h" // ��������^�ļ�
#include "Level.h"                // �����҂��µ� Level �
#include <iostream>               // ��춴�ӡ���I

int main(int argc, char* argv[])
{
    // --- 1. ��ʼ�� ---
    GamesEngineeringBase::Window canvas;
    canvas.create(1024, 768, "2D Rogue Game"); // ����ҕ��
    bool running = true;

    // ���� Level ����K���d�؈D�ļ�
    Level level;
    // ���@�e���Ğ�����Ҫ�yԇ���P���ļ�
    if (!level.loadFromFile("Resources/level1.json")) {
        std::cerr << "Failed to load level, exiting." << std::endl;
        system("pause"); // ���˳�ǰ��ͣ���Ա㿴���e�`ӍϢ
        return -1;
    }

    // ���d��҈DƬ
    GamesEngineeringBase::Image playerImage;
    if (!playerImage.load("Resources/A.png")) {
        std::cerr << "Failed to load player image." << std::endl;
        system("pause");
        return -1;
    }

    // ������������е��������� (ʹ�� int ��͸��`��)
    int planeX = 0;
    int planeY = 0;

    // --- 2. �O����ҳ�ʼλ�� ---
    const GameObject* objects = level.getGameObjects();
    bool spawnPointFound = false;
    for (int i = 0; i < level.getObjectCount(); ++i) {
        if (objects[i].type == "hero_respawn") {
            planeX = objects[i].x;
            planeY = objects[i].y;
            spawnPointFound = true;
            std::cout << "Player spawn point found at (" << planeX << ", " << planeY << ")" << std::endl;
            break; // �ҵ�����˳�ѭ�h
        }
    }
    if (!spawnPointFound) {
        std::cerr << "Warning: 'hero_respawn' object not found in map. Placing player at (100, 100)." << std::endl;
        planeX = 100;
        planeY = 100;
    }

    GamesEngineeringBase::Timer timer;
    float MOVE_SPEED = 200.0f;

    // --- �������s�ſ���׃�� ---
    float currentZoom = 0.5f; // ��ʼ�s�Ş� 0.5x
    level.setZoom(currentZoom);
    bool plus_pressed = false;  // ��춷�ֹ���I�B�l
    bool minus_pressed = false; // ��춷�ֹ���I�B�l

    // --- 3. ���[��ѭ�h ---
    while (running)
    {
        canvas.checkInput();

        // --- ����߉݋ ---
        float dt = timer.dt();

        // --- �������� 0.5x �� 1.0x ֮�g�ГQ�Ŀs��߉݋ ---
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

        // --- ����Ƅ��c��ײ߉݋ ---
        int move_amount = static_cast<int>(MOVE_SPEED * dt);
        if (move_amount < 1 && dt > 0.001) move_amount = 1; // ��֤��С�ƶ��ٶ�

        // ˮƽ�Ƅ�
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

        // ��ֱ�Ƅ�
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

        // --- �z��C���� (�Ѹ������m���s��) ---
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


        // --- ��Ⱦ߉݋ ---
        canvas.clear();

        level.render(canvas);

        // --- ���������棺��Ⱦ��� (ʹ������ӳ��) ---
        // 1. �����������Ļ�ϵ���ʾ����
        int player_screen_x_start = static_cast<int>(round((planeX - cameraTargetX) * currentZoom));
        int player_screen_y_start = static_cast<int>(round((planeY - cameraTargetY) * currentZoom));
        int player_screen_x_end = static_cast<int>(round((planeX + playerImage.width - cameraTargetX) * currentZoom));
        int player_screen_y_end = static_cast<int>(round((planeY + playerImage.height - cameraTargetY) * currentZoom));

        // 2. ���������Ļ�����ÿһ������
        for (int screen_y = player_screen_y_start; screen_y < player_screen_y_end; ++screen_y) {
            for (int screen_x = player_screen_x_start; screen_x < player_screen_x_end; ++screen_x) {

                // ��׶�ü�
                if (screen_x >= 0 && screen_x < (int)canvas.getWidth() && screen_y >= 0 && screen_y < (int)canvas.getHeight()) {

                    // 3. ����ȷ�������Ļ���ض�ӦԴͼƬ(A.png)���ĸ�����
                    unsigned int src_pixel_x = static_cast<unsigned int>((float)(screen_x - player_screen_x_start) / (player_screen_x_end - player_screen_x_start) * playerImage.width);
                    unsigned int src_pixel_y = static_cast<unsigned int>((float)(screen_y - player_screen_y_start) / (player_screen_y_end - player_screen_y_start) * playerImage.height);

                    // �߽籣��
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

