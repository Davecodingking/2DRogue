#include "Level.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <cmath> 

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
    isInfinite = false;
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
bool Level::isInfiniteMode() const { return isInfinite; }

void Level::setCameraPosition(int x, int y) {
    cameraX = x;
    cameraY = y;
}

void Level::setZoom(float zoomLevel) {
    if (zoomLevel > 0) {
        zoom = zoomLevel;
    }
}

void Level::setInfinite(bool infinite) {
    isInfinite = infinite;
}

//core function to check if a tile is an obstacle
bool Level::isObstacleAt(int tx, int ty) const {
    if (isInfinite) {
        if (!obstaclesData || mapWidth <= 0 || mapHeight <= 0) return false;
        int dataTx = (tx % mapWidth + mapWidth) % mapWidth;
        int dataTy = (ty % mapHeight + mapHeight) % mapHeight;

        if (obstaclesData[dataTy * mapWidth + dataTx].id > 0) {
            return true;
        }
    }
    else {
        if (tx < 0 || tx >= mapWidth || ty < 0 || ty >= mapHeight) {
            return true;
        }
        if (obstaclesData && obstaclesData[ty * mapWidth + tx].id > 0) {
            return true;
        }
    }
    return false;
}

void Level::render(GamesEngineeringBase::Window& canvas) {
    if (tilesetImage.width == 0) return;

    int tilesetWidthInTiles = tilesetImage.width / 32;

    int startTileX = static_cast<int>(floor(cameraX / 32.0f));
    int startTileY = static_cast<int>(floor(cameraY / 32.0f));
    int endTileX = startTileX + static_cast<int>(ceil(canvas.getWidth() / (32.0f * zoom))) + 2;
    int endTileY = startTileY + static_cast<int>(ceil(canvas.getHeight() / (32.0f * zoom))) + 2;

    if (!isInfinite) {
        if (startTileX < 0) startTileX = 0;
        if (startTileY < 0) startTileY = 0;
        if (endTileX > mapWidth) endTileX = mapWidth;
        if (endTileY > mapHeight) endTileY = mapHeight;
    }

    auto drawLayer = [&](Tile* dataArray, bool checkAlpha) {
        if (!dataArray) return;
        for (int ty = startTileY; ty < endTileY; ++ty) {
            for (int tx = startTileX; tx < endTileX; ++tx) {

                int dataTx = tx;
                int dataTy = ty;

                if (isInfinite) {
                    if (mapWidth <= 0 || mapHeight <= 0) continue;
                    dataTx = (tx % mapWidth + mapWidth) % mapWidth;
                    dataTy = (ty % mapHeight + mapHeight) % mapHeight;
                }

                Tile& tile = dataArray[dataTy * mapWidth + dataTx];
                if (tile.id > 0) {

                    float screenX_f = (tx * 32.0f - cameraX) * zoom;
                    float screenY_f = (ty * 32.0f - cameraY) * zoom;
                    float nextScreenX_f = ((tx + 1) * 32.0f - cameraX) * zoom;
                    float nextScreenY_f = ((ty + 1) * 32.0f - cameraY) * zoom;

                    int screenX_start = static_cast<int>(round(screenX_f));
                    int screenY_start = static_cast<int>(round(screenY_f));
                    int screenX_end = static_cast<int>(round(nextScreenX_f));
                    int screenY_end = static_cast<int>(round(nextScreenY_f));

                    if (screenX_start >= (int)canvas.getWidth() || screenY_start >= (int)canvas.getHeight() || screenX_end <= 0 || screenY_end <= 0) {
                        continue;
                    }
                    if (screenX_start == screenX_end || screenY_start == screenY_end) {
                        continue;
                    }

                    int sourceX_base = ((tile.id - 1) % tilesetWidthInTiles) * 32;
                    int sourceY_base = ((tile.id - 1) / tilesetWidthInTiles) * 32;

                    for (int currentScreenY = screenY_start; currentScreenY < screenY_end; ++currentScreenY) {
                        for (int currentScreenX = screenX_start; currentScreenX < screenX_end; ++currentScreenX) {
                            if (currentScreenX >= 0 && currentScreenX < (int)canvas.getWidth() && currentScreenY >= 0 && currentScreenY < (int)canvas.getHeight()) {
                                double u_double = (double)(currentScreenX - screenX_start) / (double)(screenX_end - screenX_start);
                                double v_double = (double)(currentScreenY - screenY_start) / (double)(screenY_end - screenY_start);

                                if (tile.flip_d) { std::swap(u_double, v_double); }
                                if (tile.flip_h) { u_double = 1.0 - u_double; }
                                if (tile.flip_v) { v_double = 1.0 - v_double; }

                                int srcPixelX = static_cast<int>(u_double * 32.0);
                                int srcPixelY = static_cast<int>(v_double * 32.0);

                                if (srcPixelX > 31) srcPixelX = 31; if (srcPixelY > 31) srcPixelY = 31;
                                if (srcPixelX < 0) srcPixelX = 0; if (srcPixelY < 0) srcPixelY = 0;

                                if (checkAlpha && tilesetImage.alphaAt(sourceX_base + srcPixelX, sourceY_base + srcPixelY) < 250) { continue; }
                                canvas.draw(currentScreenX, currentScreenY, tilesetImage.at(sourceX_base + srcPixelX, sourceY_base + srcPixelY));
                            }
                        }
                    }
                }
            }
        }
        };

    drawLayer(backgroundData, false);
    drawLayer(roadData, false);
    drawLayer(obstaclesData, true);
    drawLayer(debrisData, true);
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

    if (!findIntValueInSubstring(content, "\"width\":", this->mapWidth) ||
        !findIntValueInSubstring(content, "\"height\":", this->mapHeight)) {
        std::cerr << "Error: Failed to parse map dimensions from " << filename << std::endl;
        return false;
    }

    if (this->mapWidth <= 0 || this->mapHeight <= 0) {
        std::cerr << "Error: Invalid map dimensions (" << mapWidth << "x" << mapHeight << ") in " << filename << std::endl;
        return false;
    }

    int totalTiles = this->mapWidth * this->mapHeight;
    this->backgroundData = new Tile[totalTiles];
    this->roadData = new Tile[totalTiles];
    this->obstaclesData = new Tile[totalTiles];
    this->debrisData = new Tile[totalTiles];

    auto parseTileLayer = [&](const std::string& name, Tile* dataArray) {
        std::string nameKey = "\"name\":\"" + name + "\"";
        size_t namePos = content.find(nameKey);
        if (namePos == std::string::npos) {
            std::cout << "Info: '" << name << "' layer not found. Treating as empty." << std::endl;
            for (int i = 0; i < totalTiles; ++i) dataArray[i].id = 0;
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

            try {
                unsigned int raw_gid = std::stoul(numStr);

                const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
                const unsigned FLIPPED_VERTICALLY_FLAG = 0x40000000;
                const unsigned FLIPPED_DIAGONALLY_FLAG = 0x20000000;

                dataArray[i].flip_h = (raw_gid & FLIPPED_HORIZONTALLY_FLAG);
                dataArray[i].flip_v = (raw_gid & FLIPPED_VERTICALLY_FLAG);
                dataArray[i].flip_d = (raw_gid & FLIPPED_DIAGONALLY_FLAG);
                dataArray[i].id = raw_gid & ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG);
            }
            catch (const std::exception& e) {
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

    if (!tilesetImage.load("Resources/city_tileset.png")) {
        std::cerr << "Error: Could not load tileset image!" << std::endl;
        cleanup();
        return false;
    }

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
            size_t countPos = objectsArrayStart;
            while (countPos < objectsArrayEnd) {
                if (content[countPos] == '{') this->objectCount++;
                countPos++;
            }

            if (this->objectCount > 0) {
                this->gameObjects = new GameObject[this->objectCount];
                size_t searchPos = objectsArrayStart;
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

bool Level::findStringValueInSubstring(const std::string& content, const std::string& key, std::string& outValue) {
    size_t pos = content.find(key);
    if (pos == std::string::npos) return false;
    size_t valueStart = content.find('"', pos + key.length());
    if (valueStart == std::string::npos) return false;
    valueStart++;
    size_t valueEnd = content.find('"', valueStart);
    if (valueEnd == std::string::npos) return false;
    outValue = content.substr(valueStart, valueEnd - valueStart);
    return true;
}
