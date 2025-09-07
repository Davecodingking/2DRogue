#include "Level.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept> // 用於 std::stoi/stof 的異常捕獲

// --- 構造函數、析構函數、Getters 等保持不變 ---
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

bool Level::isObstacleAt(int tx, int ty) const {
    if (tx < 0 || tx >= mapWidth || ty < 0 || ty >= mapHeight) {
        return true;
    }
    if (obstaclesData) {
        return obstaclesData[ty * mapWidth + tx] != 0;
    }
    return true;
}

void Level::setCameraPosition(int x, int y) {
    cameraX = x;
    cameraY = y;
}

// --- 核心加載函數 ---
bool Level::loadFromFile(const std::string& filename) {
    cleanup();
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open level file: " << filename << std::endl;
        return false;
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    size_t tempSearchPos = 0;
    if (!findIntValue(content, "\"width\":", this->mapWidth, tempSearchPos) ||
        !findIntValue(content, "\"height\":", this->mapHeight, tempSearchPos)) {
        std::cerr << "Error: Failed to parse map dimensions from " << filename << std::endl;
        return false;
    }

    if (this->mapWidth <= 0 || this->mapHeight <= 0) {
        std::cerr << "Error: Invalid map dimensions (" << mapWidth << "x" << mapHeight << ") in " << filename << std::endl;
        return false;
    }

    int totalTiles = this->mapWidth * this->mapHeight;
    this->backgroundData = new int[totalTiles];
    this->roadData = new int[totalTiles];
    this->obstaclesData = new int[totalTiles];
    this->debrisData = new int[totalTiles];

    size_t layersStartPos = content.find("\"layers\":[");
    if (layersStartPos == std::string::npos) {
        std::cerr << "Error: Could not find 'layers' array in JSON." << std::endl;
        cleanup();
        return false;
    }

    auto parseTileLayer = [&](const std::string& name, int* dataArray) {
        std::string nameKey = "\"name\":\"" + name + "\"";
        size_t namePos = content.find(nameKey, layersStartPos);

        if (namePos == std::string::npos) {
            std::cout << "Info: '" << name << "' layer not found. Treating as empty." << std::endl;
            for (int i = 0; i < totalTiles; ++i) dataArray[i] = 0;
            return true;
        }

        size_t layerStart = content.rfind('{', namePos);
        if (layerStart == std::string::npos) {
            std::cerr << "Error: Malformed JSON, could not find start of '" << name << "' layer object." << std::endl;
            return false;
        }

        size_t dataPos = content.find("\"data\":[", layerStart);
        size_t layerEnd = content.find('}', namePos);
        if (dataPos == std::string::npos || dataPos > layerEnd) {
            std::cerr << "Error: '" << name << "' data array not found." << std::endl; return false;
        }

        size_t searchPos = dataPos + 7;
        for (int i = 0; i < totalTiles; ++i) {
            while (searchPos < content.length() && (content[searchPos] < '0' || content[searchPos] > '9') && content[searchPos] != '-') { searchPos++; }
            if (searchPos >= content.length()) { std::cerr << "Error: " << name << " tile data is incomplete." << std::endl; return false; }
            size_t valueStart = searchPos;
            while (searchPos < content.length() && ((content[searchPos] >= '0' && content[searchPos] <= '9') || content[searchPos] == '-')) { searchPos++; }
            try { dataArray[i] = std::stoi(content.substr(valueStart, searchPos - valueStart)); }
            catch (...) { std::cerr << "Error: Invalid number in " << name << " data near tile index " << i << std::endl; return false; }
        }
        return true;
        };

    if (!parseTileLayer("background", this->backgroundData)) { cleanup(); return false; }
    if (!parseTileLayer("road", this->roadData)) { cleanup(); return false; }
    if (!parseTileLayer("obstacles", this->obstaclesData)) { cleanup(); return false; }
    if (!parseTileLayer("debris", this->debrisData)) { cleanup(); return false; }

    size_t objectLayerPos = content.find("\"name\":\"GameObjects\"", layersStartPos);
    if (objectLayerPos != std::string::npos) {
        size_t objectsArrayStart = content.find("\"objects\":[", objectLayerPos);
        if (objectsArrayStart != std::string::npos) {
            objectsArrayStart += 10;
            size_t objectsArrayEnd = objectsArrayStart;
            int bracketCount = 1;
            while (objectsArrayEnd < content.length() - 1 && bracketCount > 0) {
                objectsArrayEnd++;
                if (content[objectsArrayEnd] == '[') bracketCount++;
                else if (content[objectsArrayEnd] == ']') bracketCount--;
            }
            if (bracketCount != 0) { std::cerr << "Error: Malformed JSON, unbalanced brackets in objects array." << std::endl; cleanup(); return false; }

            size_t countPos = objectsArrayStart;
            while (countPos < objectsArrayEnd) {
                size_t nextObjectStart = content.find('{', countPos);
                if (nextObjectStart != std::string::npos && nextObjectStart < objectsArrayEnd) {
                    this->objectCount++;
                    size_t nextObjectEnd = nextObjectStart + 1;
                    int braceCount = 1;
                    while (nextObjectEnd < objectsArrayEnd && braceCount > 0) {
                        if (content[nextObjectEnd] == '{') braceCount++;
                        else if (content[nextObjectEnd] == '}') braceCount--;
                        nextObjectEnd++;
                    }
                    countPos = nextObjectEnd;
                }
                else { break; }
            }

            if (this->objectCount > 0) {
                this->gameObjects = new GameObject[this->objectCount];
                size_t currentObjectStart = objectsArrayStart;
                for (int i = 0; i < this->objectCount; ++i) {
                    currentObjectStart = content.find('{', currentObjectStart);

                    size_t currentObjectEnd = currentObjectStart + 1;
                    int braceCount = 1;
                    while (currentObjectEnd < objectsArrayEnd && braceCount > 0) {
                        if (content[currentObjectEnd] == '{') braceCount++;
                        else if (content[currentObjectEnd] == '}') braceCount--;
                        currentObjectEnd++;
                    }
                    currentObjectEnd--;

                    std::string objectString = content.substr(currentObjectStart, currentObjectEnd - currentObjectStart + 1);

                    // **最終修復**: 正確解析嵌套的 "type" 屬性
                    size_t typePos = objectString.find("\"name\":\"type\"");
                    if (typePos != std::string::npos) {
                        size_t valuePos = objectString.find("\"value\":\"", typePos);
                        if (valuePos != std::string::npos) {
                            size_t valueStart = valuePos + 9; // length of "\"value\":\""
                            size_t valueEnd = objectString.find('"', valueStart);
                            if (valueEnd != std::string::npos) {
                                gameObjects[i].type = objectString.substr(valueStart, valueEnd - valueStart);
                            }
                        }
                    }
                    if (gameObjects[i].type.empty()) { std::cerr << "Error: Could not parse 'type' for object " << i << std::endl; cleanup(); return false; }

                    size_t xPos = objectString.find("\"x\":");
                    if (xPos == std::string::npos) { std::cerr << "Error: Could not find 'x' for object " << i << std::endl; cleanup(); return false; }
                    size_t xValueStart = xPos + 4;
                    size_t xValueEnd = objectString.find_first_of(",}", xValueStart);
                    try { gameObjects[i].x = static_cast<int>(std::stof(objectString.substr(xValueStart, xValueEnd - xValueStart))); }
                    catch (...) { std::cerr << "Error: Could not parse 'x' value for object " << i << std::endl; cleanup(); return false; }

                    size_t yPos = objectString.find("\"y\":");
                    if (yPos == std::string::npos) { std::cerr << "Error: Could not find 'y' for object " << i << std::endl; cleanup(); return false; }
                    size_t yValueStart = yPos + 4;
                    size_t yValueEnd = objectString.find_first_of(",}", yValueStart);
                    try { gameObjects[i].y = static_cast<int>(std::stof(objectString.substr(yValueStart, yValueEnd - yValueStart))); }
                    catch (...) { std::cerr << "Error: Could not parse 'y' value for object " << i << std::endl; cleanup(); return false; }

                    currentObjectStart = currentObjectEnd + 1;
                }
            }
        }
    }

    if (!tilesetImage.load("Resources/city_tileset.png")) {
        std::cerr << "Error: Could not load tileset image 'Resources/city_tileset.png'!" << std::endl;
        cleanup();
        return false;
    }
    std::cout << "Level '" << filename << "' loaded successfully. Dimensions: " << mapWidth << "x" << mapHeight << ". Objects: " << objectCount << std::endl;
    return true;
}

// --- 渲染函數 ---
void Level::render(GamesEngineeringBase::Window& canvas) {
    if (tilesetImage.width == 0) return;

    int tilesetWidthInTiles = tilesetImage.width / 32;
    int startTileX = cameraX / 32;
    int startTileY = cameraY / 32;
    int endTileX = (cameraX + canvas.getWidth()) / 32 + 2;
    int endTileY = (cameraY + canvas.getHeight()) / 32 + 2;
    if (startTileX < 0) startTileX = 0;
    if (startTileY < 0) startTileY = 0;
    if (endTileX > mapWidth) endTileX = mapWidth;
    if (endTileY > mapHeight) endTileY = mapHeight;

    auto drawLayer = [&](int* dataArray, bool checkAlpha) {
        if (!dataArray) return;
        for (int ty = startTileY; ty < endTileY; ++ty) {
            for (int tx = startTileX; tx < endTileX; ++tx) {
                int tile_id = dataArray[ty * mapWidth + tx];
                if (tile_id > 0) {
                    int screenX = tx * 32 - cameraX;
                    int screenY = ty * 32 - cameraY;
                    int sourceX = ((tile_id - 1) % tilesetWidthInTiles) * 32;
                    int sourceY = ((tile_id - 1) / tilesetWidthInTiles) * 32;

                    for (int pixelY = 0; pixelY < 32; ++pixelY) {
                        for (int pixelX = 0; pixelX < 32; ++pixelX) {
                            if (checkAlpha && tilesetImage.alphaAt(sourceX + pixelX, sourceY + pixelY) < 255) {
                                continue;
                            }
                            int targetX = screenX + pixelX;
                            int targetY = screenY + pixelY;
                            if (targetX >= 0 && targetX < canvas.getWidth() && targetY >= 0 && targetY < canvas.getHeight()) {
                                unsigned char* pixelColor = tilesetImage.at(sourceX + pixelX, sourceY + pixelY);
                                canvas.draw(targetX, targetY, pixelColor);
                            }
                        }
                    }
                }
            }
        }
        };

    drawLayer(backgroundData, false);
    drawLayer(roadData, true);
    drawLayer(obstaclesData, true);
    drawLayer(debrisData, true);
}


// --- 私有輔助函數的實現 ---
bool Level::findIntValue(const std::string& content, const std::string& key, int& outValue, size_t& searchPos) {
    size_t pos = content.find(key, searchPos);
    if (pos == std::string::npos) return false;
    size_t valueStart = pos + key.length();
    size_t valueEnd = content.find_first_of(",}", valueStart);
    if (valueEnd == std::string::npos) return false;
    try { outValue = std::stoi(content.substr(valueStart, valueEnd - valueStart)); }
    catch (const std::exception&) { return false; }
    searchPos = valueEnd;
    return true;
}

bool Level::findFloatValue(const std::string& content, const std::string& key, float& outValue, size_t& searchPos) {
    size_t pos = content.find(key, searchPos);
    if (pos == std::string::npos) return false;
    size_t valueStart = pos + key.length();
    size_t valueEnd = content.find_first_of(",}", valueStart);
    if (valueEnd == std::string::npos) return false;
    try { outValue = std::stof(content.substr(valueStart, valueEnd - valueStart)); }
    catch (const std::exception& e) { std::cerr << "Error converting string to float for key '" << key << "'. Content: '" << content.substr(valueStart, valueEnd - valueStart) << "'" << std::endl; return false; }
    searchPos = valueEnd;
    return true;
}

bool Level::findStringValue(const std::string& content, const std::string& key, std::string& outValue, size_t& searchPos) {
    size_t pos = content.find(key, searchPos);
    if (pos == std::string::npos) return false;
    size_t valueStart = pos + key.length();
    size_t valueEnd = content.find('"', valueStart);
    if (valueEnd == std::string::npos) return false;
    outValue = content.substr(valueStart, valueEnd - valueStart);
    searchPos = valueEnd;
    return true;
}

