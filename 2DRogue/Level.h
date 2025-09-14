#pragma once
#include <string>
#include "GamesEngineeringBase.h"

// GameObject structure for storing Tiled object info
struct GameObject {
    std::string type;
    int x;
    int y;
};

class Level {
public:
    Level();
    ~Level();

    // Load level from Tiled .json file
    bool loadFromFile(const std::string& filename);
    // Render the level
    void render(GamesEngineeringBase::Window& canvas);

    // Get map width (in tiles)
    int getWidth() const;
    // Get map height (in tiles)
    int getHeight() const;
    // Get number of game objects
    int getObjectCount() const;
    // Get pointer to game objects
    const GameObject* getGameObjects() const;
    // Check if a tile is an obstacle
    bool isObstacleAt(int tx, int ty) const;

    // Get current zoom
    float getZoom() const;
    // Check if infinite mode is enabled
    bool isInfiniteMode() const;

    // Set camera position
    void setCameraPosition(int x, int y);
    // Set zoom level for rendering
    void setZoom(float zoomLevel);
    // Set infinite mode
    void setInfinite(bool infinite);


private:
    int mapWidth;
    int mapHeight;
    int cameraX;
    int cameraY;
    float zoom;
    bool isInfinite; // Indicates if the map is infinite

    // Tile structure for storing tile info
    struct Tile {
        int id = 0; // Tile ID
        bool flip_h = false; // Horizontal flip
        bool flip_v = false; // Vertical flip
        bool flip_d = false; // Diagonal flip (rotation)
    };

    Tile* backgroundData;
    Tile* roadData;
    Tile* obstaclesData;
    Tile* debrisData;

    GameObject* gameObjects;
    int objectCount;

    GamesEngineeringBase::Image tilesetImage;

    void cleanup();

    // JSON parsing helpers
    bool findIntValueInSubstring(const std::string& content, const std::string& key, int& outValue);
    bool findStringValueInSubstring(const std::string& content, const std::string& key, std::string& outValue);
};

