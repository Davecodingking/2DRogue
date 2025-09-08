#include "NPC.h"
#include "Level.h"
#include <cmath>
#include <iostream>

NPC::NPC() {
    // ���캯���п�������һЩĬ��ֵ
    movementSpeed = 2.0f; // NPC Ĭ���ٶȱ������
    currentHealth = 50;
    maxHealth = 50;
}

NPC::~NPC() {
}

bool NPC::Load(const std::string& filename) {
    if (!npcImage.load(filename)) {
        std::cerr << "����NPCͼƬʧ��: " << filename << std::endl;
        return false;
    }
    // ʹ��ͼƬ�ߴ�����NPC����ײ�ߴ�
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

// Update ����ÿһ֡���ᱻ����
void NPC::Update(Level& level, float deltaTime) {
    // ���NPC��������ִ���κ��߼�
    if (!isAlive) {
        return;
    }
    // Ŀǰ��NPC�����������߼��ǿյġ�
    // �����ƶ����� Game ��� AI �߼����ֵ��� MoveTowards() ��������
    // δ�����ǿ�����������Ӷ�����״̬�л����߼���
}

// AI �ƶ��߼�
void NPC::MoveTowards(int targetX, int targetY) {
    if (!isAlive) {
        return;
    }

    // �����NPC��Ŀ�������
    float dirX = (float)targetX - this->x;
    float dirY = (float)targetY - this->y;

    // ���������ĳ��ȣ����룩
    float length = sqrt(dirX * dirX + dirY * dirY);

    // �������ܽ����Ͳ����ƶ��ˣ����ⶶ��
    if (length < 1.0f) {
        return;
    }

    // ��������λ�������ȱ�Ϊ1���������ƶ��ٶȾͲ����ܾ���Ӱ��
    dirX /= length;
    dirY /= length;

    // ���ݷ�����ٶȣ�����NPC��λ��
    // ����������ʱ������NPC֮�����ײ������ε���ײ
    this->x += dirX * movementSpeed;
    this->y += dirY * movementSpeed;
}

void NPC::Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) {
    if (!isAlive) {
        return;
    }

    // �����Ⱦ����� Hero::Render �е��߼���ȫһ��
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
