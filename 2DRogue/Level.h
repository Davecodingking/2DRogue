#pragma once
#include <string>
#include "GamesEngineeringBase.h"

// GameObject �ṹ�壬���ڴ洢 Tiled �еĶ�����Ϣ
struct GameObject {
    std::string type;
    int x;
    int y;
};

class Level {
public:
    Level();
    ~Level();

    // �� Tiled ������ .json �ļ����عؿ�����
    bool loadFromFile(const std::string& filename);
    // ��Ⱦ�ؿ�
    void render(GamesEngineeringBase::Window& canvas);

    // ��ȡ��ͼ��ȣ���λ��ͼ�飩
    int getWidth() const;
    // ��ȡ��ͼ�߶ȣ���λ��ͼ�飩
    int getHeight() const;
    // ��ȡ��Ϸ��������
    int getObjectCount() const;
    // ��ȡ��Ϸ���������ָ��
    const GameObject* getGameObjects() const;
    // ���ָ�������ͼ���Ƿ�Ϊ�ϰ���
    bool isObstacleAt(int tx, int ty) const;

    // ��ȡ��ǰ�����ż���
    float getZoom() const;

    // �������������������
    void setCameraPosition(int x, int y);
    // ������Ⱦ�����ż���
    void setZoom(float zoomLevel);

private:
    int mapWidth;
    int mapHeight;
    int cameraX;
    int cameraY;
    float zoom; // ���ű���

    // ��ͼ���ͼ������
    int* backgroundData;
    int* roadData;
    int* obstaclesData;
    int* debrisData;

    // ��Ϸ��������
    GameObject* gameObjects;
    int objectCount;

    // ͼ�鼯ͼ��
    GamesEngineeringBase::Image tilesetImage;

    // �ͷ��ڴ�ĸ�������
    void cleanup();

    // ����JSON�ĸ�������
    bool findIntValueInSubstring(const std::string& content, const std::string& key, int& outValue);
    bool findStringValueInSubstring(const std::string& content, const std::string& key, std::string& outValue);
};

