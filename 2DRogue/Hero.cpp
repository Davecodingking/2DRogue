#include "Hero.h"
#include "Level.h"
#include <Windows.h>
#include <cmath>
#include <iostream>

#define _USE_MATH_DEFINES // Ϊ��ʹ�� M_PI
#include <math.h>

// --- ���캯�� ---
// ��ʼ�����ж�����صı���
Hero::Hero() :
    m_currentLegDirection(0),
    m_currentFrame(0.0f),
    m_animationSpeed(20.0f), // �����ٶȿ��Ը�����Ҫ����
    m_isMoving(false),
    m_currentTorsoFrame(0),
    m_torsoOffsetY(-24),      // ������ʼ������ֵ��ʾ����ƫ��10����������
    m_frameWidth(0),
    m_frameHeight(0)
{
    movementSpeed = 200.0f; // �ƶ��ٶȣ���λ������/��
}

// --- �������� ---
Hero::~Hero() {
}

// --- ���غ��� ---
// ����������ڸ���������еĶ�����Դ
bool Hero::Load() {
    // 1. �����°������߶��� (8�����������ͼ)
    m_legAnimations.resize(8);
    for (int i = 0; i < 8; ++i) {
        std::string path = "Resources/Mech_Legs_Animations/legs_" + std::to_string(i) + ".png";
        if (!m_legAnimations[i].load(path)) {
            std::cerr << "�����Ȳ�����ʧ��: " << path << std::endl;
            return false;
        }
    }
    std::cout << "�ɹ����� 8 ��������Ȳ�������" << std::endl;

    // 2. �����ϰ�����׼ͼ�� (32������)
    m_torsoImages.resize(32);
    for (int i = 0; i < 32; ++i) {
        // �ļ����Ǵ� 1 �� 32
        std::string path = "Resources/Mech_upper_Animations/hero_upper_run_front_export_dir" + std::to_string(i + 1) + ".png";
        if (!m_torsoImages[i].load(path)) {
            std::cerr << "����������׼ͼʧ��: " << path << std::endl;
            return false;
        }
    }
    std::cout << "�ɹ����� 32 �������������׼ͼ��" << std::endl;

    // 3. ���㲢���ý�ɫ����ײ����Ⱦ�ߴ�
    // ���Ǽ������ж���֡�ĳߴ綼��һ���ģ����ڵ�һ���Ȳ�����ͼ����
    if (m_legAnimations.empty() || m_legAnimations[0].width == 0) {
        std::cerr << "�Ȳ�������ԴΪ�ջ�ߴ�����޷��趨��ɫ�ߴ硣" << std::endl;
        return false;
    }
    // ����ͼ�� 5 �� 8 ��
    m_frameWidth = m_legAnimations[0].width / 5;
    m_frameHeight = m_legAnimations[0].height / 8;
    this->width = m_frameWidth;
    this->height = m_frameHeight;

    return true;
}

// --- λ�����ú��� ---
void Hero::SetPosition(int startX, int startY) {
    this->x = startX;
    this->y = startY;
}

// --- ���º��� (���ڽ��� window ����) ---
void Hero::Update(Level& level, float deltaTime, GamesEngineeringBase::Window& window) {
    if (!isAlive) {
        return;
    }

    // 1. ����������벢���������ƶ�λ��
    int newX = x;
    int newY = y;
    float moveAmount = movementSpeed * deltaTime;

    // ʹ������������б���ƶ��ٶ�һ����
    float dx = 0.0f, dy = 0.0f;
    if (window.keyPressed('W')) dy -= 1.0f;
    if (window.keyPressed('S')) dy += 1.0f;
    if (window.keyPressed('A')) dx -= 1.0f;
    if (window.keyPressed('D')) dx += 1.0f;

    if (dx != 0.0f || dy != 0.0f) {
        float length = sqrt(dx * dx + dy * dy);
        dx /= length;
        dy /= length;
        newX += dx * moveAmount;
        newY += dy * moveAmount;
        m_isMoving = true;
    }
    else {
        m_isMoving = false;
    }

    // 2. ��ײ��Ⲣ��������λ��
    CheckMapCollision(level, newX, newY);

    // 3. ���ģ������°�����ϰ���Ķ���״̬
    // ע�⣺������Ҫ camera ��Ϣ����ȷ�������Ƕ�
    int cameraX = (int)(this->x - (window.getWidth() / 2.0f / level.getZoom()) + (this->width / 2));
    int cameraY = (int)(this->y - (window.getHeight() / 2.0f / level.getZoom()) + (this->height / 2));
    UpdateAnimations(deltaTime, window, cameraX, cameraY, level.getZoom());
}


// --- ��Ⱦ���� (�ֲ���Ⱦ) ---
void Hero::Render(GamesEngineeringBase::Window& canvas, int cameraX, int cameraY, float zoom) {
    if (!isAlive) {
        return;
    }

    // �����ɫ����Ļ�ϵĻ�����ʼ����
    int screenX = static_cast<int>(round((x - cameraX) * zoom));
    int screenY = static_cast<int>(round((y - cameraY) * zoom));

    // 1. �����°��� (�Ȳ�)
    if (!m_legAnimations.empty()) {
        RenderFrame(canvas, m_legAnimations[m_currentLegDirection], static_cast<int>(m_currentFrame), screenX, screenY, zoom);
    }

    // 2. �����ϰ��� (����)
    if (!m_torsoImages.empty()) {
        // �����������ϰ����Y���꣬Ӧ��ƫ��������������
        int torsoScreenY = screenY + static_cast<int>(round(m_torsoOffsetY * zoom));
        // ʹ���µ�Y��������Ⱦ�ϰ���
        RenderFrame(canvas, m_torsoImages[m_currentTorsoFrame], 0, screenX, torsoScreenY, zoom);
    }
}

// --- �������¸������� ---
void Hero::UpdateAnimations(float deltaTime, GamesEngineeringBase::Window& window, int cameraX, int cameraY, float zoom) {
    // --- 1. �����°����� ---
    if (m_isMoving) {
        // ���¶���֡
        m_currentFrame += m_animationSpeed * deltaTime;
        if (m_currentFrame >= 40.0f) { // �Ȳ������ܹ�40֡
            m_currentFrame -= 40.0f;
        }

        // ��ȡ����״̬����������
        bool up = window.keyPressed('W');
        bool down = window.keyPressed('S');
        bool left = window.keyPressed('A');
        bool right = window.keyPressed('D');

        // ���ݰ���������÷��� (0-7)��˳��Ϊ�����·���ʼ��ʱ��
        if (down && !left && !right) m_currentLegDirection = 0;      // S
        else if (down && left) m_currentLegDirection = 1;         // SA
        else if (left && !up && !down) m_currentLegDirection = 2;   // A
        else if (left && up) m_currentLegDirection = 3;           // WA
        else if (up && !left && !right) m_currentLegDirection = 4;    // W
        else if (up && right) m_currentLegDirection = 5;          // WD
        else if (right && !up && !down) m_currentLegDirection = 6;  // D
        else if (right && down) m_currentLegDirection = 7;        // SD
        // ���ֻ�������Գ�ļ������£��򱣳���һ������
    }
    else {
        m_currentFrame = 0; // ��ֹʱ��ʾ��һ֡
    }

    // --- 2. �����ϰ�����׼���� ---
    int mouseX = window.getMouseInWindowX();
    int mouseY = window.getMouseInWindowY();

    // �����������Ļ�ϵ����ĵ�
    float playerScreenCenterX = (x - cameraX + width / 2.0f) * zoom;
    float playerScreenCenterY = (y - cameraY + height / 2.0f) * zoom;

    // ������ҵ����ĽǶ�
    float angleRad = atan2(mouseY - playerScreenCenterY, mouseX - playerScreenCenterX);
    float angleDeg = angleRad * 180.0f / M_PI;

    // ���Ƕȴ� (-180, 180] ת���� [0, 360)
    if (angleDeg < 0) {
        angleDeg += 360.0f;
    }

    // ���Ƕ�ӳ�䵽 32 �������е�һ��
    // ÿ�����򸲸� 360 / 32 = 11.25 ��
    // ������Ҫƫ�ư���Ƕȵ�����ʹ�� 0 ���������ڵ�0��������м�
    m_currentTorsoFrame = static_cast<int>(round(angleDeg / 11.25f)) % 32;
}

// --- ��ͼ��ײ��� ---
void Hero::CheckMapCollision(Level& level, int newX, int newY) {
    // �򵥵ľ�����ײ������ɫ���ĸ���
    // ע�⣺����ֻ��һ������ʵ�֣�����ȷ����ײ��Ҫ���ǽ�ɫ����ײ��
    int leftTile = newX / TILE_SIZE;
    int rightTile = (newX + width - 1) / TILE_SIZE;
    int topTile = newY / TILE_SIZE;
    int bottomTile = (newY + height - 1) / TILE_SIZE;

    // ��� X ���ƶ�
    int tempX = x;
    if (newX != x) {
        if (newX > x) { // �����ƶ�
            if (!level.isObstacleAt(rightTile, topTile) && !level.isObstacleAt(rightTile, bottomTile)) {
                x = newX;
            }
        }
        else { // �����ƶ�
            if (!level.isObstacleAt(leftTile, topTile) && !level.isObstacleAt(leftTile, bottomTile)) {
                x = newX;
            }
        }
    }

    // ������ײ��Ƭ���꣬Ȼ���� Y ���ƶ�
    leftTile = x / TILE_SIZE;
    rightTile = (x + width - 1) / TILE_SIZE;
    topTile = newY / TILE_SIZE;
    bottomTile = (newY + height - 1) / TILE_SIZE;

    if (newY != y) {
        if (newY > y) { // �����ƶ�
            if (!level.isObstacleAt(leftTile, bottomTile) && !level.isObstacleAt(rightTile, bottomTile)) {
                y = newY;
            }
        }
        else { // �����ƶ�
            if (!level.isObstacleAt(leftTile, topTile) && !level.isObstacleAt(rightTile, topTile)) {
                y = newY;
            }
        }
    }
}


// --- ��Ⱦ��֡�������� ---
void Hero::RenderFrame(GamesEngineeringBase::Window& canvas, GamesEngineeringBase::Image& spriteSheet, int frameIndex, int screenX, int screenY, float zoom) {
    if (spriteSheet.width == 0) return;

    // �����ϰ�����׼ͼ�����ǲ�������ͼ����Ⱦ���֡���
    int sheetFrameWidth = m_frameWidth;
    int sheetColumns = spriteSheet.width / sheetFrameWidth;
    if (sheetColumns == 0) sheetColumns = 1; // �������

    // ���㵱ǰ֡������ͼ�е����Ͻ�����
    int sourceX = (frameIndex % sheetColumns) * sheetFrameWidth;
    int sourceY = (frameIndex / sheetColumns) * m_frameHeight;

    // ����ǵ���ͼƬ�������ϰ��������ĳߴ����֡�ĳߴ�
    int currentFrameWidth = (spriteSheet.width < (unsigned int)m_frameWidth) ? spriteSheet.width : m_frameWidth;
    int currentFrameHeight = (spriteSheet.height < (unsigned int)m_frameHeight) ? spriteSheet.height : m_frameHeight;

    // �������ź����Ⱦ�ߴ�
    int scaledWidth = static_cast<int>(round(currentFrameWidth * zoom));
    int scaledHeight = static_cast<int>(round(currentFrameHeight * zoom));
    int screenX_end = screenX + scaledWidth;
    int screenY_end = screenY + scaledHeight;

    if (scaledWidth <= 0 || scaledHeight <= 0) return;

    // ������Ļ���ؽ��л���
    for (int sy = screenY; sy < screenY_end; ++sy) {
        for (int sx = screenX; sx < screenX_end; ++sx) {
            if (sx >= 0 && sx < (int)canvas.getWidth() && sy >= 0 && sy < (int)canvas.getHeight()) {
                // ����Ļ����ӳ���Դͼ����
                unsigned int src_px = sourceX + static_cast<unsigned int>((double)(sx - screenX) / scaledWidth * currentFrameWidth);
                unsigned int src_py = sourceY + static_cast<unsigned int>((double)(sy - screenY) / scaledHeight * currentFrameHeight);

                // �߽���
                if (src_px >= spriteSheet.width) src_px = spriteSheet.width - 1;
                if (src_py >= spriteSheet.height) src_py = spriteSheet.height - 1;

                // ͸���ȼ��
                if (spriteSheet.alphaAt(src_px, src_py) > 200) {
                    canvas.draw(sx, sy, spriteSheet.at(src_px, src_py));
                }
            }
        }
    }
}

