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
    // ��ȡ�Ƿ�Ϊ����ģʽ
    bool isInfiniteMode() const;

    // �������������������
    void setCameraPosition(int x, int y);
    // ������Ⱦ�����ż���
    void setZoom(float zoomLevel);
    // �����Ƿ�Ϊ����ģʽ
    void setInfinite(bool infinite);


private:
    int mapWidth;
    int mapHeight;
    int cameraX;
    int cameraY;
    float zoom;
    bool isInfinite; // ���������ڱ���Ƿ�Ϊ���޵�ͼ

    // �޸��ⲿ�֣�ʹ��һ���ṹ�����洢ͼ���������Ϣ
    struct Tile {
        int id = 0; // ������ͼ��ID
        bool flip_h = false; // �Ƿ�ˮƽ��ת
        bool flip_v = false; // �Ƿ�ֱ��ת
        bool flip_d = false; // �Ƿ�Խ��߷�ת (������ת)
    };

    Tile* backgroundData;
    Tile* roadData;
    Tile* obstaclesData;
    Tile* debrisData;

    GameObject* gameObjects;
    int objectCount;

    GamesEngineeringBase::Image tilesetImage;

    void cleanup();

    // ����JSON�ĸ�������
    bool findIntValueInSubstring(const std::string& content, const std::string& key, int& outValue);
    bool findStringValueInSubstring(const std::string& content, const std::string& key, std::string& outValue);
};

