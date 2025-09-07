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
    Level level1;
    if (!level1.loadFromFile("Resources/level1.json")) {
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
    const GameObject* objects = level1.getGameObjects();
    bool spawnPointFound = false;
    for (int i = 0; i < level1.getObjectCount(); ++i) {
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

    // --- 3. ���[��ѭ�h ---
    while (running)
    {
        canvas.checkInput();

        // --- ����߉݋ ---
        float dt = timer.dt();
        int move_amount = static_cast<int>(MOVE_SPEED * dt);
        if (move_amount < 1) move_amount = 1;

        // =======================================================================
        // !! ���،����Ƅ��c��ײ߉݋ !!
        // =======================================================================

        // --- ̎��ˮƽ�Ƅ� ---
        int dx = 0;
        if (canvas.keyPressed('A')) { dx -= move_amount; }
        if (canvas.keyPressed('D')) { dx += move_amount; }

        if (dx != 0) {
            int nextX = planeX + dx;
            // �����Ƅӷ��򣬙z������е���Ȼ��҂�߅��
            int collisionTileX = (dx > 0) ? (nextX + playerImage.width) / 32 : nextX / 32;
            int topTileY = planeY / 32;
            int bottomTileY = (planeY + playerImage.height - 1) / 32;

            // ֻҪ�Ƅ�·���ϵ��κ�һ���D�K���ϵK��Ͳ��Ƅ�
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

        // --- ̎��ֱ�Ƅ� ---
        int dy = 0;
        if (canvas.keyPressed('W')) { dy -= move_amount; }
        if (canvas.keyPressed('S')) { dy += move_amount; }

        if (dy != 0) {
            int nextY = planeY + dy;
            // �����Ƅӷ��򣬙z������е�픲���ײ�߅��
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

        // --- �z��C���� ---
        int cameraTargetX = planeX + (playerImage.width / 2) - (canvas.getWidth() / 2);
        int cameraTargetY = planeY + (playerImage.height / 2) - (canvas.getHeight() / 2);

        int worldWidthPixels = level1.getWidth() * 32;
        int worldHeightPixels = level1.getHeight() * 32;
        if (cameraTargetX < 0) cameraTargetX = 0;
        if (cameraTargetY < 0) cameraTargetY = 0;
        if (cameraTargetX > worldWidthPixels - canvas.getWidth()) cameraTargetX = worldWidthPixels - canvas.getWidth();
        if (cameraTargetY > worldHeightPixels - canvas.getHeight()) cameraTargetY = worldHeightPixels - canvas.getHeight();

        level1.setCameraPosition(cameraTargetX, cameraTargetY);


        // --- ��Ⱦ߉݋ ---
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

