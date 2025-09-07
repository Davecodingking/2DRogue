#pragma once
#include <string>
#include "GamesEngineeringBase.h" // ��������^�ļ�

// ��춃���� Tiled ���d���[�����������c���Ĕ����Y��
struct GameObject {
    std::string type;
    int x;
    int y;
};

class Level {
public:
    // ���캯���c��������
    Level();
    ~Level();

    // �� Tiled ������ .json �ļ����d�P������
    bool loadFromFile(const std::string& filename);

    // �����z��Cλ����Ⱦ�P��
    void render(GamesEngineeringBase::Window& canvas);

    // --- ������ԃ (Getters) ---
    int getWidth() const;
    int getHeight() const;
    int getObjectCount() const;
    const GameObject* getGameObjects() const;
    bool isObstacleAt(int tx, int ty) const; // �z��ָ���D�K�����Ƿ����ϵK��

    // --- �z��C���� ---
    void setCameraPosition(int x, int y);

private:
    // �؈D�ߴ磨�ԈD�K���λ��
    int mapWidth;
    int mapHeight;

    // �z��Cλ�ã������؞��λ��
    int cameraX;
    int cameraY;

    // ��춃���D�K�����ĄӑB���
    int* backgroundData;
    int* roadData;
    int* obstaclesData;
    int* debrisData;

    // ��춃����[����ĄӑB���
    GameObject* gameObjects;
    int objectCount;

    // ��춃���D�K���DƬ
    GamesEngineeringBase::Image tilesetImage;

    // ˽���o������
    void cleanup();
    bool findIntValue(const std::string& content, const std::string& key, int& outValue, size_t& searchPos);
    bool findFloatValue(const std::string& content, const std::string& key, float& outValue, size_t& searchPos);
    bool findStringValue(const std::string& content, const std::string& key, std::string& outValue, size_t& searchPos);
};

