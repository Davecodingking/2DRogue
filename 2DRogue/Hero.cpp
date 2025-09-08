#include "Hero.h"
#include "Level.h"
#include <Windows.h> // ���� GetAsyncKeyState
#include <cmath>     // ���� round()
#include <iostream>  // ���ڴ�ӡ������Ϣ

// --- ���캯�� ---
// ��ʼ��Ӣ�۵�Ĭ�����ԣ������ƶ��ٶ�����Ϊһ����ʼֵ��
Hero::Hero() {
    movementSpeed = 5.0f; // ���ֵ֮ǰ���� main.cpp �ж���� PLAYER_SPEED
}

// --- �������� ---
// Ŀǰ�ǿյġ�����Ժ�����Ҫ��̬������ڴ棬���ǻ��������ͷš�
Hero::~Hero() {
}

// --- ���غ��� ---
// ��һ���ļ�·������Ӣ�۵�ͼ��
bool Hero::Load(const std::string& filename) {
    if (!playerImage.load(filename)) {
        // �������ʧ�ܣ����������Ϣ��
        std::cerr << "����Ӣ��ͼƬʧ��: " << filename << std::endl;
        return false;
    }
    // ���سɹ�����ͼƬ�ĳߴ�������Ӣ�۵Ŀ�Ⱥ͸߶ȡ�
    // �������Ⱦ����ײ���������Ҫ��
    this->width = playerImage.width;
    this->height = playerImage.height;
    return true;
}

// --- λ�����ú��� ---
// ����Ϸ����������Ӣ�۵ĳ�ʼ���ꡣ
void Hero::SetPosition(int startX, int startY) {
    this->x = startX;
    this->y = startY;
}

// --- ���º��� (�����߼�) ---
// �������ÿһ֡���ᱻ���ã�����Ӣ�۵�������Ϊ��
void Hero::Update(Level& level, float deltaTime) {
    // ���Ӣ���Ѿ���������ִ���κθ����߼���
    if (!isAlive) {
        return;
    }

    // 1. ����������룬����������������ƶ�������λ�á�
    int newX = x;
    int newY = y;

    // ʹ�� GetAsyncKeyState �����̰����Ƿ񱻰��¡�
    if ((GetAsyncKeyState(VK_UP) & 0x8000) || (GetAsyncKeyState('W') & 0x8000))    newY -= movementSpeed;
    if ((GetAsyncKeyState(VK_DOWN) & 0x8000) || (GetAsyncKeyState('S') & 0x8000))  newY += movementSpeed;
    if ((GetAsyncKeyState(VK_LEFT) & 0x8000) || (GetAsyncKeyState('A') & 0x8000))  newX -= movementSpeed;
    if ((GetAsyncKeyState(VK_RIGHT) & 0x8000) || (GetAsyncKeyState('D') & 0x8000)) newX += movementSpeed;

    // 2. ����������λ�ô��ݸ���ײ��⺯����
    // ����������ж��Ƿ�����ƶ��������ո���Ӣ�۵�ʵ��λ�á�
    CheckMapCollision(level, newX, newY);
}

// --- ��Ⱦ���� ---
// ��Ӣ�ۻ��Ƶ���Ļ�ϡ�
void Hero::Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) {
    // ���Ӣ���������򲻽��л��ơ�
    if (!isAlive) {
        return;
    }

    // --- �����Ⱦ���뼸����ȫ�Ǵ���ԭ���� main.cpp ��Ǩ�ƹ����� ---

    // ����Ӣ�۵��������ꡢ�����λ�ú����ż��𣬼����������Ļ�ϵĻ�������
    int screen_x_start = static_cast<int>(round((x - cameraX) * zoom));
    int screen_y_start = static_cast<int>(round((y - cameraY) * zoom));
    int screen_x_end = static_cast<int>(round((x + width - cameraX) * zoom));
    int screen_y_end = static_cast<int>(round((y + height - cameraY) * zoom));

    // ������ź�ߴ�Ϊ0����������ƣ�����������
    if (screen_x_start == screen_x_end || screen_y_start == screen_y_end) {
        return;
    }

    // ������Ļ�ϵ�ÿһ�����ص������л��ơ�
    for (int screen_y = screen_y_start; screen_y < screen_y_end; ++screen_y) {
        for (int screen_x = screen_x_start; screen_x < screen_x_end; ++screen_x) {

            // ȷ��ֻ�ڴ��ڵĿ��ӷ�Χ�ڻ��ơ�
            if (screen_x >= 0 && screen_x < (int)canvas.getWidth() && screen_y >= 0 && screen_y < (int)canvas.getHeight()) {

                // ����Ļ����ӳ���ԭʼͼƬ�ϵ��������ꡣ
                unsigned int src_pixel_x = static_cast<unsigned int>((double)(screen_x - screen_x_start) / (double)(screen_x_end - screen_x_start) * width);
                unsigned int src_pixel_y = static_cast<unsigned int>((double)(screen_y - screen_y_start) / (double)(screen_y_end - screen_y_start) * height);

                // �߽��飬��ֹ��ȡͼƬ����ʱԽ�硣
                if (src_pixel_x >= playerImage.width) src_pixel_x = playerImage.width - 1;
                if (src_pixel_y >= playerImage.height) src_pixel_y = playerImage.height - 1;

                // ���͸���� (alpha)��ֻ���Ʋ�͸�������ء�
                if (playerImage.alphaAt(src_pixel_x, src_pixel_y) > 200) {
                    canvas.draw(screen_x, screen_y, playerImage.at(src_pixel_x, src_pixel_y));
                }
            }
        }
    }
}


// --- ˽�и�����������ͼ��ײ��� ---
void Hero::CheckMapCollision(Level& level, int newX, int newY) {
    // ��������λ�ö�Ӧ�ĵ�ͼͼ�����ꡣ
    int playerTileX = newX / TILE_SIZE;
    int playerTileY = newY / TILE_SIZE;

    // ���Ŀ��ͼ���Ƿ����ϰ��
    if (!level.isObstacleAt(playerTileX, playerTileY)) {
        // ��������ϰ���������ƶ�������Ӣ�۵�ʵ��λ�á�
        x = newX;
        y = newY;
    }
    // ������ϰ������ʲô��������Ӣ�۵�λ�ñ��ֲ��䡣

    // �����ͼ��������ģʽ������Ҫ���б߽��顣
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
