#include "Level.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept> 

Level::Level() {
    mapWidth = 0;
    mapHeight = 0;
    backgroundData = nullptr;
    roadData = nullptr;
    obstaclesData = nullptr;
    debrisData = nullptr;
    gameObjects = nullptr;
    objectCount = 0;
    cameraX = 0;
    cameraY = 0;
    zoom = 1.0f;
}

Level::~Level() {
    cleanup();
}

void Level::cleanup() {
    delete[] backgroundData;
    delete[] roadData;
    delete[] obstaclesData;
    delete[] debrisData;
    delete[] gameObjects;
    backgroundData = nullptr;
    roadData = nullptr;
    obstaclesData = nullptr;
    debrisData = nullptr;
    gameObjects = nullptr;
    objectCount = 0;
}

int Level::getWidth() const { return mapWidth; }
int Level::getHeight() const { return mapHeight; }
int Level::getObjectCount() const { return objectCount; }
const GameObject* Level::getGameObjects() const { return gameObjects; }
float Level::getZoom() const { return zoom; }

bool Level::isObstacleAt(int tx, int ty) const {
    if (tx < 0 || tx >= mapWidth || ty < 0 || ty >= mapHeight) {
        return true;
    }
    if (obstaclesData) {
        return obstaclesData[ty * mapWidth + tx] != 0;
    }
    return true; // Default to obstacle if data is missing
}

void Level::setCameraPosition(int x, int y) {
    cameraX = x;
    cameraY = y;
}

void Level::setZoom(float zoomLevel) {
    if (zoomLevel > 0.0f) {
        zoom = zoomLevel;
    }
}

bool Level::loadFromFile(const std::string& filename) {
    cleanup();
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open level file: " << filename << std::endl;
        return false;
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // --- 1. 解析地图的宽度和高度 ---
    if (!findIntValueInSubstring(content, "\"width\":", this->mapWidth) ||
        !findIntValueInSubstring(content, "\"height\":", this->mapHeight)) {
        std::cerr << "Error: Failed to parse map dimensions from " << filename << std::endl;
        return false;
    }

    if (this->mapWidth <= 0 || this->mapHeight <= 0) {
        std::cerr << "Error: Invalid map dimensions (" << mapWidth << "x" << mapHeight << ") in " << filename << std::endl;
        return false;
    }

    // --- 2. 为图层数据分配内存 ---
    int totalTiles = this->mapWidth * this->mapHeight;
    this->backgroundData = new int[totalTiles];
    this->roadData = new int[totalTiles];
    this->obstaclesData = new int[totalTiles];
    this->debrisData = new int[totalTiles];

    // --- 3. 解析每一个图块图层 (background, obstacles, etc.) ---
    auto parseTileLayer = [&](const std::string& name, int* dataArray) {
        std::string nameKey = "\"name\":\"" + name + "\"";
        size_t namePos = content.find(nameKey);
        if (namePos == std::string::npos) {
            std::cout << "Info: '" << name << "' layer not found. Treating as empty." << std::endl;
            for (int i = 0; i < totalTiles; ++i) dataArray[i] = 0;
            return true;
        }
        size_t layerStart = content.rfind('{', namePos);
        size_t dataArrayStart = content.find("\"data\":[", layerStart);
        if (dataArrayStart == std::string::npos) return true;

        size_t searchPos = dataArrayStart + 7;
        for (int i = 0; i < totalTiles; ++i) {
            while (searchPos < content.length() && !isdigit(content[searchPos]) && content[searchPos] != '-') searchPos++;
            if (searchPos >= content.length() || content[searchPos] == ']') {
                std::cerr << "Error: Tile data for layer '" << name << "' is incomplete." << std::endl; return false;
            }
            size_t valueStart = searchPos;
            while (searchPos < content.length() && (isdigit(content[searchPos]) || content[searchPos] == '-')) searchPos++;
            std::string numStr = content.substr(valueStart, searchPos - valueStart);

            // --- 这是关键的修复 ---
            try {
                // 使用 stoull 读取无符号长整型，避免溢出
                unsigned long long raw_gid = std::stoull(numStr);
                // Tiled 使用最高的3位作为翻转标志，我们需要把它们去掉
                const unsigned int ALL_FLIP_FLAGS = 0xE0000000;
                dataArray[i] = static_cast<int>(raw_gid & ~ALL_FLIP_FLAGS);
            }
            catch (const std::exception& e) {
                // 如果转换失败，打印错误并返回
                std::cerr << "Error parsing GID for tile " << i << " in layer '" << name << "'. Value: '" << numStr << "'. Error: " << e.what() << std::endl;
                return false;
            }
        }
        return true;
        };

    if (!parseTileLayer("background", this->backgroundData)) { cleanup(); return false; }
    if (!parseTileLayer("road", this->roadData)) { cleanup(); return false; }
    if (!parseTileLayer("obstacles", this->obstaclesData)) { cleanup(); return false; }
    if (!parseTileLayer("debris", this->debrisData)) { cleanup(); return false; }

    // --- 4. 加载贴图文件 ---
    if (!tilesetImage.load("Resources/city_tileset.png")) {
        std::cerr << "Error: Could not load tileset image!" << std::endl;
        cleanup();
        return false;
    }

    // --- 5. 精确解析 GameObjects 图层 (这部分逻辑已经是正确的) ---
    size_t objectLayerPos = content.find("\"name\":\"GameObjects\"");
    if (objectLayerPos != std::string::npos) {
        size_t objectsArrayStart = content.find("\"objects\":[", objectLayerPos);
        if (objectsArrayStart != std::string::npos) {
            objectsArrayStart += 10;

            size_t objectsArrayEnd = objectsArrayStart;
            int bracketCount = 1;
            while (bracketCount > 0 && ++objectsArrayEnd < content.length()) {
                if (content[objectsArrayEnd] == '[') bracketCount++;
                if (content[objectsArrayEnd] == ']') bracketCount--;
            }

            this->objectCount = 0;
            size_t searchPos = objectsArrayStart;
            while (searchPos < objectsArrayEnd) {
                if (content[searchPos] == '{') this->objectCount++;
                searchPos++;
            }

            if (this->objectCount > 0) {
                this->gameObjects = new GameObject[this->objectCount];
                searchPos = objectsArrayStart;
                for (int i = 0; i < this->objectCount; ++i) {
                    size_t objectStart = content.find('{', searchPos);
                    if (objectStart == std::string::npos || objectStart >= objectsArrayEnd) break;

                    size_t objectEnd = objectStart;
                    int braceCount = 1;
                    while (braceCount > 0 && ++objectEnd < objectsArrayEnd) {
                        if (content[objectEnd] == '{') braceCount++;
                        if (content[objectEnd] == '}') braceCount--;
                    }

                    std::string objectString = content.substr(objectStart, objectEnd - objectStart + 1);

                    size_t typePropertyPos = objectString.find("\"name\":\"type\"");
                    if (typePropertyPos != std::string::npos) {
                        std::string valueKey = "\"value\":\"";
                        size_t valueStartPos = objectString.find(valueKey, typePropertyPos);
                        if (valueStartPos != std::string::npos) {
                            valueStartPos += valueKey.length();
                            size_t valueEndPos = objectString.find('"', valueStartPos);
                            if (valueEndPos != std::string::npos) {
                                gameObjects[i].type = objectString.substr(valueStartPos, valueEndPos - valueStartPos);
                            }
                        }
                    }

                    findIntValueInSubstring(objectString, "\"x\":", gameObjects[i].x);
                    findIntValueInSubstring(objectString, "\"y\":", gameObjects[i].y);

                    searchPos = objectEnd + 1;
                }
            }
        }
    }

    std::cout << "Level '" << filename << "' loaded successfully. Dimensions: " << mapWidth << "x" << mapHeight << ". Objects found: " << objectCount << std::endl;
    return true;
}
void Level::render(GamesEngineeringBase::Window& canvas) {
    if (tilesetImage.width == 0 || !backgroundData) return;

    const float tile_size = 32.0f;
    int tileset_width_in_tiles = tilesetImage.width / 32;

    // 预先计算出逆向缩放因子，避免在循环中做除法
    float inverse_zoom = 1.0f / zoom;

    // 遍历屏幕上的每一个像素
    for (int screen_y = 0; screen_y < (int)canvas.getHeight(); ++screen_y) {
        for (int screen_x = 0; screen_x < (int)canvas.getWidth(); ++screen_x) {

            // 1. 将屏幕坐标反向映射到世界坐标
            float world_x = (float)screen_x * inverse_zoom + cameraX;
            float world_y = (float)screen_y * inverse_zoom + cameraY;

            // 2. 计算出对应的图块坐标
            int tile_x = static_cast<int>(floor(world_x / tile_size));
            int tile_y = static_cast<int>(floor(world_y / tile_size));

            // 3. 边界检查，超出地图范围的像素直接跳过
            if (tile_x < 0 || tile_x >= mapWidth || tile_y < 0 || tile_y >= mapHeight) {
                continue;
            }

            // 4. 计算出在源图块中的具体像素坐标 (0-31)
            int src_pixel_x = static_cast<int>(world_x) % 32;
            int src_pixel_y = static_cast<int>(world_y) % 32;

            // --- 5. 按顺序从各图层取色并绘制 ---

            // 首先绘制背景层 (没有透明)
            int bg_tile_id = backgroundData[tile_y * mapWidth + tile_x];
            if (bg_tile_id > 0) {
                int bg_source_x = ((bg_tile_id - 1) % tileset_width_in_tiles) * 32;
                int bg_source_y = ((bg_tile_id - 1) / tileset_width_in_tiles) * 32;
                canvas.draw(screen_x, screen_y, tilesetImage.at(bg_source_x + src_pixel_x, bg_source_y + src_pixel_y));
            }

            // 然后叠加绘制道路层 (没有透明)
            if (roadData) {
                int road_tile_id = roadData[tile_y * mapWidth + tile_x];
                if (road_tile_id > 0) {
                    int road_source_x = ((road_tile_id - 1) % tileset_width_in_tiles) * 32;
                    int road_source_y = ((road_tile_id - 1) / tileset_width_in_tiles) * 32;
                    if (tilesetImage.alphaAt(road_source_x + src_pixel_x, road_source_y + src_pixel_y) > 200)
                    {
                        canvas.draw(screen_x, screen_y, tilesetImage.at(road_source_x + src_pixel_x, road_source_y + src_pixel_y));
                    }
                }
            }

            // 叠加绘制障碍物层 (检查透明度)
            if (obstaclesData) {
                int obs_tile_id = obstaclesData[tile_y * mapWidth + tile_x];
                if (obs_tile_id > 0) {
                    int obs_source_x = ((obs_tile_id - 1) % tileset_width_in_tiles) * 32;
                    int obs_source_y = ((obs_tile_id - 1) / tileset_width_in_tiles) * 32;
                    if (tilesetImage.alphaAt(obs_source_x + src_pixel_x, obs_source_y + src_pixel_y) > 200) {
                        canvas.draw(screen_x, screen_y, tilesetImage.at(obs_source_x + src_pixel_x, obs_source_y + src_pixel_y));
                    }
                }
            }

            // 叠加绘制杂物层 (检查透明度)
            if (debrisData) {
                int deb_tile_id = debrisData[tile_y * mapWidth + tile_x];
                if (deb_tile_id > 0) {
                    int deb_source_x = ((deb_tile_id - 1) % tileset_width_in_tiles) * 32;
                    int deb_source_y = ((deb_tile_id - 1) / tileset_width_in_tiles) * 32;
                    if (tilesetImage.alphaAt(deb_source_x + src_pixel_x, deb_source_y + src_pixel_y) > 200) {
                        canvas.draw(screen_x, screen_y, tilesetImage.at(deb_source_x + src_pixel_x, deb_source_y + src_pixel_y));
                    }
                }
            }
        }
    }
}

bool Level::findIntValueInSubstring(const std::string& content, const std::string& key, int& outValue) {
    size_t pos = content.find(key);
    if (pos == std::string::npos) return false;

    size_t valueStart = pos + key.length();
    while (valueStart < content.length() && (content[valueStart] < '0' || content[valueStart] > '9') && content[valueStart] != '-') {
        valueStart++;
    }

    size_t valueEnd = valueStart;
    if (valueEnd < content.length() && content[valueEnd] == '-') valueEnd++;
    while (valueEnd < content.length() && (content[valueEnd] >= '0' && content[valueEnd] <= '9')) {
        valueEnd++;
    }

    if (valueStart >= valueEnd) return false;

    try {
        outValue = std::stoi(content.substr(valueStart, valueEnd - valueStart));
    }
    catch (const std::exception&) {
        return false;
    }
    return true;
}


