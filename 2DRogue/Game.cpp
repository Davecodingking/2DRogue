#include "Game.h"
#include "LaserBeam.h"
#include <iostream>
#include <fstream> // 用于文件读写
#include <cstdlib>
#include <string>
#include <ctime>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 定义存档文件名称
const char* SAVE_FILE_NAME = "savegame.dat";

// 定义用于存档的数据结构
struct SaveData {
    int score;
    int wave;
};


// --- 核心修改: 使用 switch 語句替換 FONT_MAP ---
void DrawCharacter(GamesEngineeringBase::Window& canvas, char c, int startX, int startY, int scale) {
    bool pixels[35] = { 0 }; // Initialize all to false

    switch (c) {
    case '0': { const bool p[] = { 1,1,1, 1,0,1, 1,0,1, 1,0,1, 1,0,1, 1,0,1, 1,0,1, 1,0,1, 1,0,1, 1,0,1, 1,1,1,1,1 }; for (int i = 0; i < 35; ++i) pixels[i] = p[i]; break; }
    case '1': { const bool p[] = { 0,1,0, 0,1,1, 0,0,1, 0,0,0,1, 0,0,0,1, 0,0,0,1, 0,0,0,1, 0,0,1,1,1, 0,0,0,0,0 }; for (int i = 0; i < 35; ++i) pixels[i] = p[i]; break; }
    case '2': { const bool p[] = { 1,1,1, 0,0,0,0,1, 0,0,0,0,1, 0,0,1,1,1, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,1,1,1,1 }; for (int i = 0; i < 35; ++i) pixels[i] = p[i]; break; }
    case '3': { const bool p[] = { 1,1,1, 0,0,0,0,1, 0,0,0,0,1, 0,0,1,1,1, 0,0,0,0,1, 0,0,0,0,1, 0,0,1,1,1, 0,0 }; for (int i = 0; i < 35; ++i) pixels[i] = p[i]; break; }
    case '4': { const bool p[] = { 1,0,1, 0,0,1,0,1, 0,0,1,1,1, 1,1,0,0,1, 0,0,0,0,1, 0,0,0,0,1, 0,0,0,0,1, 0,0 }; for (int i = 0; i < 35; ++i) pixels[i] = p[i]; break; }
    case '5': { const bool p[] = { 1,1,1,1,1, 1,0,0,0,0, 1,1,1,1,0, 0,0,0,0,1, 0,0,0,0,1, 1,0,0,0,0, 1,1,1,1,0 }; for (int i = 0; i < 35; ++i) pixels[i] = p[i]; break; }
    case '6': { const bool p[] = { 0,1,1,1,0, 1,0,0,0,0, 1,1,1,1,0, 1,0,0,0,1, 1,0,0,0,1, 0,1,1,1,0, 0,0,0,0,0 }; for (int i = 0; i < 35; ++i) pixels[i] = p[i]; break; }
    case '7': { const bool p[] = { 1,1,1,1,1, 0,0,0,0,1, 0,0,0,1,0, 0,0,1,0,0, 0,1,0,0,0, 0,1,0,0,0, 0,1,0,0,0 }; for (int i = 0; i < 35; ++i) pixels[i] = p[i]; break; }
    case '8': { const bool p[] = { 1,1,1,0,0, 1,0,1,0,0, 1,1,1,0,0, 1,0,1,0,0, 1,0,1,0,0, 1,1,1,0,0, 0,0,0,0,0 }; for (int i = 0; i < 35; ++i) pixels[i] = p[i]; break; }
    case '9': { const bool p[] = { 1,1,1,1,0, 1,0,0,0,1, 1,0,0,0,1, 0,1,1,1,1, 0,0,0,0,1, 0,0,0,0,1, 0,1,1,1,0 }; for (int i = 0; i < 35; ++i) pixels[i] = p[i]; break; }
    case 'A': { const bool p[] = { 0,1,1,1,0, 1,0,0,0,1, 1,1,1,1,1, 1,0,0,0,1, 1,0,0,0,1, 0,0,0,0,0, 0,0,0,0,0 }; for (int i = 0; i < 35; ++i) pixels[i] = p[i]; break; }
    case 'D': { const bool p[] = { 1,1,1,0,0, 1,0,0,1,0, 1,0,0,0,1, 1,0,0,0,1, 1,0,0,1,0, 1,1,1,0,0, 0,0,0,0,0 }; for (int i = 0; i < 35; ++i) pixels[i] = p[i]; break; }
    case 'L': { const bool p[] = { 1,0,0,0,0, 1,0,0,0,0, 1,0,0,0,0, 1,0,0,0,0, 1,1,1,1,1, 0,0,0,0,0, 0,0,0,0,0 }; for (int i = 0; i < 35; ++i) pixels[i] = p[i]; break; }
    case 'B': { const bool p[] = { 1,1,1,1,0, 1,0,0,0,1, 1,1,1,1,0, 1,0,0,0,1, 1,1,1,1,0, 0,0,0,0,0, 0,0,0,0,0 }; for (int i = 0; i < 35; ++i) pixels[i] = p[i]; break; }
    case 'F': { const bool p[] = { 1,1,1,1,1, 1,0,0,0,0, 1,1,1,1,0, 1,0,0,0,0, 1,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0 }; for (int i = 0; i < 35; ++i) pixels[i] = p[i]; break; }
    case 'I': { const bool p[] = { 1,1,1, 0,1,0, 0,1,0, 0,1,0, 0,1,0, 0,1,0, 0,1,0, 0,1,0, 0,1,0, 1,1,1, 0,0,0,0,0 }; for (int i = 0; i < 35; ++i) pixels[i] = p[i]; break; }
    case 'G': { const bool p[] = { 0,1,1,1,0, 1,0,0,0,0, 1,0,1,1,1, 1,0,0,0,1, 0,1,1,1,0, 0,0,0,0,0, 0,0,0,0,0 }; for (int i = 0; i < 35; ++i) pixels[i] = p[i]; break; }
    case 'H': { const bool p[] = { 1,0,0,0,1, 1,0,0,0,1, 1,1,1,1,1, 1,0,0,0,1, 1,0,0,0,1, 0,0,0,0,0, 0,0,0,0,0 }; for (int i = 0; i < 35; ++i) pixels[i] = p[i]; break; }
    case 'T': { const bool p[] = { 1,1,1,1,1, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,0,0,0, 0,0,0,0,0 }; for (int i = 0; i < 35; ++i) pixels[i] = p[i]; break; }
    case 'P': { const bool p[] = { 1,1,1,1,0, 1,0,0,0,1, 1,1,1,1,0, 1,0,0,0,0, 1,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0 }; for (int i = 0; i < 35; ++i) pixels[i] = p[i]; break; }
    case 'S': { const bool p[] = { 0,1,1,1,1, 1,0,0,0,0, 0,1,1,1,0, 0,0,0,0,1, 1,1,1,1,0, 0,0,0,0,0, 0,0,0,0,0 }; for (int i = 0; i < 35; ++i) pixels[i] = p[i]; break; }
    case 'C': { const bool p[] = { 0,1,1,1,0, 1,0,0,0,1, 1,0,0,0,0, 1,0,0,0,1, 0,1,1,1,0, 0,0,0,0,0, 0,0,0,0,0 }; for (int i = 0; i < 35; ++i) pixels[i] = p[i]; break; }
    case 'O': { const bool p[] = { 0,1,1,1,0, 1,0,0,0,1, 1,0,0,0,1, 1,0,0,0,1, 0,1,1,1,0, 0,0,0,0,0, 0,0,0,0,0 }; for (int i = 0; i < 35; ++i) pixels[i] = p[i]; break; }
    case 'R': { const bool p[] = { 1,1,1,1,0, 1,0,0,0,1, 1,1,1,1,0, 1,0,1,0,0, 1,0,0,1,0, 0,0,0,0,0, 0,0,0,0,0 }; for (int i = 0; i < 35; ++i) pixels[i] = p[i]; break; }
    case 'E': { const bool p[] = { 1,1,1,1,1, 1,0,0,0,0, 1,1,1,1,0, 1,0,0,0,0, 1,1,1,1,1, 0,0,0,0,0, 0,0,0,0,0 }; for (int i = 0; i < 35; ++i) pixels[i] = p[i]; break; }
    case 'V': { const bool p[] = { 1,0,0,0,1, 1,0,0,0,1, 0,1,0,1,0, 0,1,0,1,0, 0,0,1,0,0, 0,0,0,0,0, 0,0,0,0,0 }; for (int i = 0; i < 35; ++i) pixels[i] = p[i]; break; }
    case '/': { const bool p[] = { 0,0,0,1,0, 0,0,1,0,0, 0,1,0,0,0, 0,1,0,0,0, 1,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0 }; for (int i = 0; i < 35; ++i) pixels[i] = p[i]; break; }
    case ':': { const bool p[] = { 0,0,0,0,0, 0,1,1,0,0, 0,0,0,0,0, 0,1,1,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0 }; for (int i = 0; i < 35; ++i) pixels[i] = p[i]; break; }
    default: break;
    }

    for (int y = 0; y < 7; ++y) {
        for (int x = 0; x < 5; ++x) {
            if (pixels[y * 5 + x]) {
                for (int sy = 0; sy < scale; ++sy) {
                    for (int sx = 0; sx < scale; ++sx) {
                        canvas.draw(startX + x * scale + sx, startY + y * scale + sy, 255, 255, 255);
                    }
                }
            }
        }
    }
}


void DrawText(GamesEngineeringBase::Window& canvas, const char* text, int startX, int startY, int scale) {
    int currentX = startX;
    for (int i = 0; text[i] != '\0'; ++i) {
        char c = text[i];
        if (c >= 'a' && c <= 'z') c -= 32; // Convert to uppercase
        DrawCharacter(canvas, c, currentX, startY, scale);
        currentX += (6 * scale); // 5 for char, 1 for spacing
    }
}


Game::Game()
    : m_cameraX(0), m_cameraY(0), m_zoom(1.5f), m_isRunning(false),
    m_activeNpcCount(0), m_activeProjectileCount(0), m_gameTimer(0.0f),
    m_spawnPointCount(0), m_bossSpawnPointCount(0), m_playerDamageCooldown(0.0f),
    m_currentLevel(0), m_currentWave(1), m_waveInProgress(false), m_waveCooldownTimer(0.0f),
    m_level2_npcSpawnedCount(0), m_level2_spawnTimer(1.0f), m_bossSpawned(false),
    m_activeLaserCount(0), m_npcsHitCount(0), m_pickupCount(0),
    m_playerScore(0), m_fps(0), m_frameCount(0), m_fpsTimer(0.0f),
    m_showLoadMessage(false), m_loadMessageTimer(0.0f) // 初始化新成员
{
    for (int i = 0; i < MAX_NPCS; ++i) m_npcPool[i] = nullptr;
    srand(static_cast<unsigned int>(time(0)));
}

bool Game::Initialize(const std::string& levelFile) {
    m_window.create(1024, 768, "2D Mech Rogue Game");
    if (levelFile.find("level1") != std::string::npos) m_currentLevel = 1;
    else if (levelFile.find("level2") != std::string::npos) m_currentLevel = 2;
    if (!m_level.loadFromFile(levelFile)) return false;
    m_level.setInfinite(false);
    m_level.setZoom(m_zoom);
    if (!m_player.Load()) return false;
    if (!Projectile::LoadAssets()) return false;
    if (!NPC::LoadSharedAssets()) return false;
    if (!LaserBeam::LoadAssets("Resources/laser_beam.png")) return false;
    if (!m_healthSheet.load("Resources/health.png")) {
        std::cerr << "ERROR: Cannot load health.png" << std::endl;
        return false;
    }
    if (!m_machineGunIcon.load("Resources/machinegun.png")) {
        std::cerr << "ERROR: Cannot load machinegun.png" << std::endl;
        return false;
    }
    if (!m_cannonIcon.load("Resources/plasmagun.png")) {
        std::cerr << "ERROR: Cannot load plasmagun.png" << std::endl;
        return false;
    }
    if (!m_laserGunIcon.load("Resources/lasergun.png")) {
        std::cerr << "ERROR: Cannot load lasergun.png" << std::endl;
        return false;
    }


    const GameObject* objects = m_level.getGameObjects();
    bool playerSpawnPointFound = false;
    for (int i = 0; i < m_level.getObjectCount(); ++i) {
        const GameObject& obj = objects[i];
        std::string type = obj.type;
        if (type == "hero_respawn") {
            m_player.SetPosition((float)obj.x, (float)obj.y);
            playerSpawnPointFound = true;
        }
        else if (type == "generic_npc_respawn" && m_spawnPointCount < MAX_SPAWN_POINTS) {
            m_npcSpawnPoints[m_spawnPointCount++] = { obj.x, obj.y };
        }
        else if (type == "boss_npc_respawn" && m_bossSpawnPointCount < MAX_SPAWN_POINTS) {
            m_bossSpawnPoints[m_bossSpawnPointCount++] = { obj.x, obj.y };
        }
        else if (type == "weaponA_respawn" && m_pickupCount < MAX_PICKUPS) {
            m_laserPickups[m_pickupCount++] = { obj.x, obj.y, false, (float)(rand() % 100) };
        }
    }
    if (!playerSpawnPointFound) {
        m_player.SetPosition(1378.0f, 2106.0f);
    }
    m_isRunning = true;
    return true;
}


void Game::Update(float deltaTime) {
    if (!m_isRunning) return;
    if (m_playerDamageCooldown > 0) m_playerDamageCooldown -= deltaTime;

    // 更新读档提示信息计时器
    if (m_showLoadMessage) {
        m_loadMessageTimer -= deltaTime;
        if (m_loadMessageTimer <= 0) {
            m_showLoadMessage = false;
        }
    }

    UpdateSpawning(deltaTime);
    m_player.Update(m_level, deltaTime);
    UpdateNPCs(deltaTime);
    UpdateProjectiles(deltaTime);
    UpdateLasers(deltaTime);
    CheckCollisions();
    m_cameraX = (int)(m_player.getX() - (m_window.getWidth() / 2.0f / m_zoom) + (m_player.getWidth() / 2.0f));
    m_cameraY = (int)(m_player.getY() - (m_window.getHeight() / 2.0f / m_zoom) + (m_player.getHeight() / 2.0f));
    m_player.UpdateAiming(m_window, m_cameraX, m_cameraY, m_zoom);

    if (!m_level.isInfiniteMode()) {
        int mapWidthPixels = m_level.getWidth() * 32;
        int mapHeightPixels = m_level.getHeight() * 32;
        int cameraViewWidth = static_cast<int>(m_window.getWidth() / m_zoom);
        int cameraViewHeight = static_cast<int>(m_window.getHeight() / m_zoom);
        if (m_cameraX < 0) m_cameraX = 0;
        if (m_cameraX > mapWidthPixels - cameraViewWidth) m_cameraX = mapWidthPixels - cameraViewWidth;
        if (m_cameraY < 0) m_cameraY = 0;
        if (m_cameraY > mapHeightPixels - cameraViewHeight) m_cameraY = mapHeightPixels - cameraViewHeight;
    }
    m_level.setCameraPosition(m_cameraX, m_cameraY);

    m_frameCount++;
    m_fpsTimer += deltaTime;
    if (m_fpsTimer >= 1.0f) {
        m_fps = m_frameCount;
        m_frameCount = 0;
        m_fpsTimer -= 1.0f;
    }
}


void Game::UpdateNPCs(float deltaTime) {
    for (int i = 0; i < m_activeNpcCount; ++i) {
        NPC* npc = m_npcPool[i];
        if (npc) {
            npc->Update(m_level, deltaTime);
            npc->UpdateAI(m_player.getX(), m_player.getY(), deltaTime);

            if (npc->getCurrentState() == NPC::WALKING) {
                npc->MoveTowards(m_level, m_player.getX(), m_player.getY(), deltaTime);
            }

            if (npc->canFire()) {
                npc->resetFireCooldown();
                float dirX = m_player.getX() - npc->getX(); float dirY = m_player.getY() - npc->getY();
                SpawnProjectile(npc->getX() + npc->getWidth() / 2, npc->getY() + npc->getHeight() / 2, atan2(-dirY, dirX), Projectile::ENEMY_SPHERE, Projectile::ENEMY);
            }
        }
    }
    for (int i = m_activeNpcCount - 1; i >= 0; --i) {
        NPC* npc = m_npcPool[i];
        if (npc && npc->getCurrentState() == NPC::State::DEAD) {
            switch (npc->getNPCType()) {
            case NPC::MELEE: m_playerScore += 100; break;
            case NPC::SHOOTER: m_playerScore += 100; break;
            case NPC::SNIPER: m_playerScore += 200; break;
            case NPC::BOSS_AIRCRAFT: m_playerScore += 1000; break;
            }

            delete npc;
            m_npcPool[i] = m_npcPool[m_activeNpcCount - 1];
            m_npcPool[m_activeNpcCount - 1] = nullptr;
            m_activeNpcCount--;
            m_waveInProgress = m_activeNpcCount > 0;
        }
    }
}

void Game::RenderUI() {
    auto drawIcon = [&](const GamesEngineeringBase::Image& icon, int centerX, int topY, int size) {
        if (icon.width == 0) return;
        int renderSize = size;
        int startX = centerX - renderSize / 2;
        int startY = topY;
        for (int y = 0; y < renderSize; ++y) {
            for (int x = 0; x < renderSize; ++x) {
                float scaleX = static_cast<float>(x) / renderSize;
                float scaleY = static_cast<float>(y) / renderSize;
                unsigned int srcX = static_cast<unsigned int>(scaleX * icon.width);
                unsigned int srcY = static_cast<unsigned int>(scaleY * icon.height);
                if (srcX < icon.width && srcY < icon.height && icon.alphaAt(srcX, srcY) > 200) {
                    int drawX = startX + x;
                    int drawY = startY + y;
                    if (drawX >= 0 && drawX < static_cast<int>(m_window.getWidth()) && drawY >= 0 && drawY < static_cast<int>(m_window.getHeight())) {
                        m_window.draw(drawX, drawY, icon.at(srcX, srcY));
                    }
                }
            }
        }
        };

    int mainBarX = 10, barY = 10, barW = 30, barH = 30;
    for (int y = barY; y < barY + barH; ++y) for (int x = mainBarX; x < mainBarX + barW; ++x) m_window.draw(x, y, 100, 100, 100);

    float fireProgress = 0.0f;
    if (m_player.m_fireMaxCooldown > 0) fireProgress = (m_player.m_fireMaxCooldown - m_player.m_fireCooldown) / m_player.m_fireMaxCooldown;
    if (fireProgress < 0.0f) fireProgress = 0.0f; if (fireProgress > 1.0f) fireProgress = 1.0f;

    int fireFillHeight = static_cast<int>(barH * fireProgress);
    unsigned char r, g, b;
    if (m_player.GetCurrentWeapon() == Hero::WeaponType::MACHINE_GUN) { r = 255; g = 255; b = 0; }
    else { r = 255; g = 165; b = 0; }

    for (int y = barY + barH - fireFillHeight; y < barY + barH; ++y) {
        for (int x = mainBarX; x < mainBarX + barW; ++x) {
            m_window.draw(x, y, r, g, b);
        }
    }

    int laserCharges = m_player.GetLaserCharges();
    int laserBarX = 50;
    for (int y = barY; y < barY + barH; ++y) for (int x = laserBarX; x < laserBarX + barW; ++x) m_window.draw(x, y, 100, 100, 100);

    if (laserCharges > 0) {
        float laserProgress = 0.0f;
        if (m_player.m_laserMaxCooldown > 0) laserProgress = (m_player.m_laserMaxCooldown - m_player.m_laserCooldown) / m_player.m_laserMaxCooldown;
        if (laserProgress < 0.0f) laserProgress = 0.0f; if (laserProgress > 1.0f) laserProgress = 1.0f;
        int laserFillHeight = static_cast<int>(barH * laserProgress);
        for (int y = barY + barH - laserFillHeight; y < barY + barH; ++y) {
            for (int x = laserBarX; x < laserBarX + barW; ++x) {
                m_window.draw(x, y, 0, 255, 255);
            }
        }
    }

    int iconTopY = barY + barH + 5;
    int iconSize = 30;
    if (m_player.GetCurrentWeapon() == Hero::WeaponType::MACHINE_GUN) {
        drawIcon(m_machineGunIcon, mainBarX + barW / 2, iconTopY, iconSize);
    }
    else {
        drawIcon(m_cannonIcon, mainBarX + barW / 2, iconTopY, iconSize);
    }

    if (laserCharges > 0) {
        drawIcon(m_laserGunIcon, laserBarX + barW / 2, iconTopY, iconSize);
    }

    int triangleSize = 8;
    int spacing = 4;
    int startDrawX = laserBarX + barW + 10;

    for (int i = 0; i < laserCharges; ++i) {
        int currentTriangleX = startDrawX + i * (triangleSize + spacing);
        int centerY = barY + barH / 2;
        for (int y = 0; y < triangleSize; ++y) {
            for (int x = 0; x < y * 2; ++x) {
                int drawX = currentTriangleX - y + x;
                int drawY = centerY - triangleSize / 2 + y;
                if (drawX >= 0 && drawX < (int)m_window.getWidth() && drawY >= 0 && drawY < (int)m_window.getHeight()) {
                    m_window.draw(drawX, drawY, 0, 150, 255);
                }
            }
        }
    }

    if (m_healthSheet.width > 0) {
        int barPosX = 10, barPosY = 80;
        int frameWidth = m_healthSheet.width / 7;
        int frameHeight = m_healthSheet.height;
        float healthBarScale = 2.0f;
        int renderWidth = static_cast<int>(frameWidth * healthBarScale);
        int renderHeight = static_cast<int>(frameHeight * healthBarScale);

        float healthPercent = (float)m_player.getHealth() / m_player.getMaxHealth();
        if (healthPercent < 0) healthPercent = 0;
        int healthFrameIndex = 1 + (int)((1.0f - healthPercent) * 5.99f);
        if (healthFrameIndex > 6) healthFrameIndex = 6;
        if (m_player.getHealth() <= 0) healthFrameIndex = 6;
        if (healthPercent >= 1.0f) healthFrameIndex = 1;

        for (int y = 0; y < renderHeight; ++y) {
            for (int x = 0; x < renderWidth; ++x) {
                unsigned int srcX = static_cast<unsigned int>(x / healthBarScale);
                unsigned int srcY = static_cast<unsigned int>(y / healthBarScale);
                if (m_healthSheet.alphaAt(srcX, srcY) > 200) {
                    m_window.draw(barPosX + x, barPosY + y, m_healthSheet.at(srcX, srcY));
                }
            }
        }
        for (int y = 0; y < renderHeight; ++y) {
            for (int x = 0; x < renderWidth; ++x) {
                unsigned int srcX = static_cast<unsigned int>(x / healthBarScale) + frameWidth * healthFrameIndex;
                unsigned int srcY = static_cast<unsigned int>(y / healthBarScale);
                if (m_healthSheet.alphaAt(srcX, srcY) > 200) {
                    m_window.draw(barPosX + x, barPosY + y, m_healthSheet.at(srcX, srcY));
                }
            }
        }
    }

    char scoreBuffer[32];
    sprintf_s(scoreBuffer, "SCORE:%d", m_playerScore);
    DrawText(m_window, scoreBuffer, m_window.getWidth() - 200, 10, 2);

    char fpsBuffer[16];
    sprintf_s(fpsBuffer, "FPS:%d", m_fps);
    DrawText(m_window, fpsBuffer, m_window.getWidth() - 200, 34, 2);

    if (m_currentLevel == 1 && m_currentWave <= 3) {
        char waveBuffer[16];
        sprintf_s(waveBuffer, "WAVE %d/3", m_currentWave);
        int textWidth = 0;
        for (int i = 0; waveBuffer[i] != '\0'; ++i) textWidth += (6 * 3);
        DrawText(m_window, waveBuffer, m_window.getWidth() / 2 - textWidth / 2, 20, 3);
    }
    else if (m_currentLevel == 2 && m_bossSpawned) {
        const char* bossText = "BOSS FIGHT";
        int textWidth = 0;
        for (int i = 0; bossText[i] != '\0'; ++i) textWidth += (6 * 5);
        DrawText(m_window, bossText, m_window.getWidth() / 2 - textWidth / 2, 20, 5);
    }

    // 绘制读档提示信息
    if (m_showLoadMessage) {
        const char* loadText = "SAVE LOADED";
        int textWidth = 0;
        for (int i = 0; loadText[i] != '\0'; ++i) textWidth += (6 * 4);
        DrawText(m_window, loadText, m_window.getWidth() / 2 - textWidth / 2, 60, 4);
    }
}

Game::~Game() {
    Shutdown();
}
void Game::Shutdown() {
    for (int i = 0; i < m_activeNpcCount; ++i) { if (m_npcPool[i]) delete m_npcPool[i]; }
    Projectile::FreeAssets();
    NPC::FreeSharedAssets();
    LaserBeam::FreeAssets();
}
void Game::Run() {
    GamesEngineeringBase::Timer timer;
    while (m_isRunning) {
        float deltaTime = timer.dt();
        if (deltaTime > 0.1f) deltaTime = 0.1f;
        m_gameTimer += deltaTime;
        ProcessInput();
        Update(deltaTime);
        Render();
    }
}
void Game::ProcessInput() {
    m_window.checkInput();
    if (m_window.keyPressed(VK_ESCAPE)) m_isRunning = false;
    int wheelDelta = m_window.getMouseWheel();
    if (wheelDelta != 0) {
        m_zoom += wheelDelta * 0.001f;
        if (m_zoom < 0.5f) m_zoom = 0.5f;
        if (m_zoom > 3.0f) m_zoom = 3.0f;
        m_level.setZoom(m_zoom);
    }
    if (m_window.mouseButtonPressed(GamesEngineeringBase::MouseButton::MouseLeft) && m_player.CanFire()) {
        m_player.ResetFireCooldown();
        Projectile::Type projType = (m_player.GetCurrentWeapon() == Hero::WeaponType::MACHINE_GUN) ? Projectile::MACHINE_GUN : Projectile::CANNON;
        SpawnProjectile(m_player.GetFirePosX(), m_player.GetFirePosY(), m_player.getAimAngle(), projType, Projectile::PLAYER);
    }
    static bool l_pressed = false;
    if (m_window.keyPressed('L') && !l_pressed) {
        if (m_player.GetLaserCharges() > 0 && m_player.m_laserCooldown <= 0) {
            FireLaser();
        }
        l_pressed = true;
    }
    if (!m_window.keyPressed('L')) {
        l_pressed = false;
    }
    static bool q_pressed = false;
    if (m_window.keyPressed('Q') && !q_pressed) {
        m_player.SwitchWeapon();
        q_pressed = true;
    }
    if (!m_window.keyPressed('Q')) q_pressed = false;

    // --- 新增: 存档与读档的按键处理 ---
    static bool f5_pressed = false;
    if (m_window.keyPressed(VK_F5) && !f5_pressed) {
        SaveGame();
        f5_pressed = true;
    }
    if (!m_window.keyPressed(VK_F5)) f5_pressed = false;

    static bool f9_pressed = false;
    if (m_window.keyPressed(VK_F9) && !f9_pressed) {
        LoadGame();
        f9_pressed = true;
    }
    if (!m_window.keyPressed(VK_F9)) f9_pressed = false;
}

void Game::Render() {
    m_window.clear();
    m_level.render(m_window);
    RenderPickups(m_gameTimer);
    for (int i = 0; i < m_activeNpcCount; ++i) if (m_npcPool[i]) m_npcPool[i]->Render(m_window, m_cameraX, m_cameraY, m_zoom);
    m_player.Render(m_window, m_cameraX, m_cameraY, m_zoom);
    for (int i = 0; i < m_activeProjectileCount; ++i) if (m_projectilePool[i].IsActive()) m_projectilePool[i].Render(m_window, m_cameraX, m_cameraY, m_zoom);
    for (int i = 0; i < m_activeLaserCount; ++i) if (m_laserPool[i].IsActive()) m_laserPool[i].Render(m_window, m_cameraX, m_cameraY, m_zoom);
    RenderUI();
    m_window.present();
}
void Game::UpdateProjectiles(float deltaTime) {
    for (int i = 0; i < m_activeProjectileCount; ) {
        if (m_projectilePool[i].IsActive()) {
            m_projectilePool[i].Update(deltaTime);
            i++;
        }
        else {
            m_projectilePool[i] = m_projectilePool[m_activeProjectileCount - 1];
            m_activeProjectileCount--;
        }
    }
}
void Game::UpdateSpawning(float deltaTime) {
    if (m_spawnPointCount == 0) return;
    switch (m_currentLevel) {
    case 1: {
        if (!m_waveInProgress && m_activeNpcCount == 0) {
            if (m_currentWave > 3) return;
            if (m_waveCooldownTimer <= 0) {
                int npcsToSpawn = m_currentWave * 2;
                for (int i = 0; i < npcsToSpawn; ++i) {
                    if (m_activeNpcCount >= MAX_NPCS) break;
                    int spawnIndex = rand() % m_spawnPointCount;
                    NPC::NPCType type = static_cast<NPC::NPCType>(rand() % 3);
                    SpawnNPC(m_npcSpawnPoints[spawnIndex].x, m_npcSpawnPoints[spawnIndex].y, type);
                }
                //if(m_currentWave < 3) m_currentWave++; // Buggy line removed
                m_waveInProgress = true;

            }
            else {
                m_waveCooldownTimer -= deltaTime;
            }
        }
        else if (m_activeNpcCount == 0 && m_waveInProgress) {
            m_waveInProgress = false;
            if (m_currentWave < 3) {
                m_currentWave++; // Correctly increment wave after it's cleared
                m_waveCooldownTimer = 5.0f;
            }
            else {
                m_currentWave = 4; // Mark that all waves are done
            }
        }
        break;
    }
    case 2: {
        if (m_level2_npcSpawnedCount < 20) {
            m_level2_spawnTimer -= deltaTime;
            if (m_level2_spawnTimer <= 0) {
                int spawnIndex = rand() % m_spawnPointCount;
                NPC::NPCType type = static_cast<NPC::NPCType>(rand() % 3);
                SpawnNPC(m_npcSpawnPoints[spawnIndex].x, m_npcSpawnPoints[spawnIndex].y, type);
                m_level2_npcSpawnedCount++;
                m_level2_spawnTimer = 1.0f;
            }
        }
        else if (!m_bossSpawned) {
            if (m_bossSpawnPointCount > 0) {
                int spawnIndex = rand() % m_bossSpawnPointCount;
                SpawnNPC(m_bossSpawnPoints[spawnIndex].x, m_bossSpawnPoints[spawnIndex].y, NPC::BOSS_AIRCRAFT);
                m_bossSpawned = true;
            }
            else { m_bossSpawned = true; }
        }
        break;
    }
    }
}
void Game::CheckCollisions() {
    float heroX = m_player.getX(), heroY = m_player.getY();
    float heroW = m_player.getWidth(), heroH = m_player.getHeight();
    m_player.SetSlowed(false);
    for (int i = 0; i < m_activeNpcCount; i++) {
        NPC* npc = m_npcPool[i];
        if (npc && npc->getCurrentState() != NPC::DEAD && npc->getCurrentState() != NPC::DYING) {
            float npcX = npc->getX(), npcY = npc->getY();
            float npcW = npc->getWidth(), npcH = npc->getHeight();
            if (heroX < npcX + npcW && heroX + heroW > npcX && heroY < npcY + npcH && heroY + heroH > npcY) {
                m_player.SetSlowed(true);
            }
        }
    }
    for (int i = 0; i < m_activeProjectileCount; i++) {
        Projectile& proj = m_projectilePool[i];
        if (proj.GetState() != Projectile::FLYING) continue;
        float projX = proj.getX(), projY = proj.getY();
        float projW = proj.getWidth(), projH = proj.getHeight();
        if (proj.getOwner() == Projectile::PLAYER) {
            for (int j = 0; j < m_activeNpcCount; j++) {
                NPC* npc = m_npcPool[j];
                if (npc && npc->getCurrentState() != NPC::DYING && npc->getCurrentState() != NPC::DEAD) {
                    float npcX = npc->getX(), npcY = npc->getY();
                    float npcW = npc->getWidth(), npcH = npc->getHeight();
                    if (projX < npcX + npcW && projX + projW > npcX && projY < npcY + npcH && projY + projH > npcY) {
                        npc->TakeDamage(proj.getType() == Projectile::CANNON ? 50 : 10);
                        if (proj.getType() == Projectile::MACHINE_GUN) npc->ApplySlow(1.5f);
                        if (proj.getType() == Projectile::CANNON) npc->ApplyStun(1.0f);
                        if (proj.getType() == Projectile::CANNON) proj.StartExplosion();
                        else proj.Deactivate();
                        break;
                    }
                }
            }
        }
        else {
            if (m_playerDamageCooldown <= 0) {
                if (projX < heroX + heroW && projX + projW > heroX && projY < heroY + heroH && projY + projH > heroY) {
                    m_player.TakeDamage(10);
                    m_playerDamageCooldown = 0.5f;
                    proj.Deactivate();
                }
            }
        }
    }
    for (int i = 0; i < m_pickupCount; ++i) {
        if (!m_laserPickups[i].isCollected) {
            float pickupW = 32.0f, pickupH = 32.0f;
            float pickupX = (float)m_laserPickups[i].x - pickupW / 2;
            float pickupY = (float)m_laserPickups[i].y - pickupH / 2;
            if (heroX < pickupX + pickupW && heroX + heroW > pickupX && heroY < pickupY + pickupH && heroY + heroH > pickupY) {
                m_laserPickups[i].isCollected = true;
                m_player.AddLaserCharges(3);
            }
        }
    }
}
void Game::UpdateLasers(float deltaTime) {
    m_npcsHitCount = 0;
    for (int i = 0; i < m_activeLaserCount; ) {
        bool shouldDamage = m_laserPool[i].Update(deltaTime);
        if (shouldDamage) {
            for (int j = 0; j < m_activeNpcCount; ++j) {
                NPC* npc = m_npcPool[j];
                if (npc && npc->getIsAlive()) {
                    bool alreadyHit = false;
                    for (int k = 0; k < m_npcsHitCount; k++) {
                        if (m_npcsHitThisFrame[k] == npc) { alreadyHit = true; break; }
                    }
                    if (!alreadyHit && m_laserPool[i].CheckCollision(npc)) {
                        npc->TakeDamage(80);
                        if (m_npcsHitCount < MAX_HIT_NPCS) m_npcsHitThisFrame[m_npcsHitCount++] = npc;
                    }
                }
            }
        }
        if (m_laserPool[i].IsActive()) {
            i++;
        }
        else {
            m_laserPool[i] = m_laserPool[m_activeLaserCount - 1];
            m_activeLaserCount--;
        }
    }
}
void Game::FireLaser() {
    m_player.UseLaserCharge();
    m_player.m_laserCooldown = m_player.m_laserMaxCooldown;
    NPC* onScreenNpcs[MAX_NPCS];
    int onScreenCount = 0;
    for (int i = 0; i < m_activeNpcCount; ++i) {
        NPC* npc = m_npcPool[i];
        if (npc && npc->getIsAlive()) {
            float sX = (npc->getX() - m_cameraX) * m_zoom, sY = (npc->getY() - m_cameraY) * m_zoom;
            if (sX + npc->getWidth() > 0 && sX < m_window.getWidth() && sY + npc->getHeight() > 0 && sY < m_window.getHeight()) {
                if (onScreenCount < MAX_NPCS) onScreenNpcs[onScreenCount++] = npc;
            }
        }
    }
    if (onScreenCount == 0) { std::cout << "Laser: No enemies on screen." << std::endl; return; }
    for (int i = 0; i < onScreenCount - 1; i++) for (int j = 0; j < onScreenCount - i - 1; j++) {
        if (onScreenNpcs[j]->getHealth() < onScreenNpcs[j + 1]->getHealth()) {
            NPC* temp = onScreenNpcs[j]; onScreenNpcs[j] = onScreenNpcs[j + 1]; onScreenNpcs[j + 1] = temp;
        }
    }
    int targets = (onScreenCount < 3) ? onScreenCount : 3;
    for (int i = 0; i < targets; ++i) {
        NPC* target = onScreenNpcs[i];
        float tX = target->getX() + target->getWidth() / 2, tY = target->getY() + target->getHeight() / 2;
        if (m_activeLaserCount < MAX_LASERS) m_laserPool[m_activeLaserCount++].Activate(&m_player, tX, tY, 0.0f);
        if (m_activeLaserCount < MAX_LASERS) m_laserPool[m_activeLaserCount++].Activate(&m_player, tX, tY, 0.7f);
    }
}
void Game::RenderPickups(float gameTime) {
    for (int i = 0; i < m_pickupCount; ++i) {
        if (!m_laserPickups[i].isCollected) {
            float worldX = (float)m_laserPickups[i].x;
            m_laserPickups[i].floatOffset = sin(gameTime * 4.0f + worldX) * 5.0f;
            float worldY = (float)m_laserPickups[i].y + m_laserPickups[i].floatOffset;
            float sX = (worldX - m_cameraX) * m_zoom;
            float sY = (worldY - m_cameraY) * m_zoom;
            bool isOnScreen = sX > 0 && sX < m_window.getWidth() && sY > 0 && sY < m_window.getHeight();
            if (isOnScreen) {
                float size = 72 * m_zoom;
                for (int k = 0; k < size; ++k) for (int j = 0; j <= k; ++j) {
                    int pX = (int)(sX - (size / 2) + j), pY = (int)(sY + (size / 2) - k);
                    if (pX >= 0 && pX < (int)m_window.getWidth() && pY >= 0 && pY < (int)m_window.getHeight()) {
                        m_window.draw(pX, pY, 100, 100, 255);
                    }
                }
            }
            else {
                float playerScreenX = (m_player.getX() + m_player.getWidth() / 2 - m_cameraX) * m_zoom;
                float playerScreenY = (m_player.getY() + m_player.getHeight() / 2 - m_cameraY) * m_zoom;
                float border = 30.0f;
                float winWidth = (float)m_window.getWidth();
                float winHeight = (float)m_window.getHeight();
                float clampedX = (sX > winWidth - border) ? (winWidth - border) : sX;
                clampedX = (border > clampedX) ? border : clampedX;
                float clampedY = (sY > winHeight - border) ? (winHeight - border) : sY;
                clampedY = (border > clampedY) ? border : clampedY;
                float angle = atan2(sY - clampedY, sX - clampedX);
                float arrowSize = 45.0f;
                float x1 = clampedX, y1 = clampedY;
                float x2 = clampedX - arrowSize * cos(angle + 0.5f);
                float y2 = clampedY - arrowSize * sin(angle + 0.5f);
                float x3 = clampedX - arrowSize * cos(angle - 0.5f);
                float y3 = clampedY - arrowSize * sin(angle - 0.5f);
                for (int j = 0; j < 20; ++j) {
                    m_window.draw((int)(x1 + (x2 - x1) * j / 20.0f), (int)(y1 + (y2 - y1) * j / 20.0f), 255, 255, 0);
                    m_window.draw((int)(x1 + (x3 - x1) * j / 20.0f), (int)(y1 + (y3 - y1) * j / 20.0f), 255, 255, 0);
                }
            }
        }
    }
}
void Game::SpawnNPC(int x, int y, NPC::NPCType type) {
    if (m_activeNpcCount >= MAX_NPCS) return;
    NPC* newNpc = new NPC(type);
    if (newNpc->Load()) {
        newNpc->SetPosition((float)x, (float)y);
        m_npcPool[m_activeNpcCount++] = newNpc;
    }
    else {
        delete newNpc;
    }
}
void Game::SpawnProjectile(float startX, float startY, float angle, Projectile::Type type, Projectile::Owner owner) {
    if (m_activeProjectileCount >= MAX_PROJECTILES) return;
    m_projectilePool[m_activeProjectileCount].Activate(startX, startY, angle, type, owner);
    m_activeProjectileCount++;
}

// --- 新增的存档/读档/重置函数 ---

void Game::SaveGame() {
    if (m_currentLevel != 1) { // 仅为 Level 1 保存
        std::cout << "Save is only available for Level 1." << std::endl;
        return;
    }

    std::ofstream saveFile(SAVE_FILE_NAME, std::ios::binary);
    if (!saveFile.is_open()) {
        std::cerr << "Error: Could not create save file." << std::endl;
        return;
    }

    SaveData data;
    data.score = m_playerScore;
    data.wave = m_currentWave;

    saveFile.write(reinterpret_cast<const char*>(&data), sizeof(SaveData));
    saveFile.close();
    std::cout << "Game saved. Score: " << data.score << ", Wave: " << data.wave << std::endl;
}

void Game::LoadGame() {
    if (m_currentLevel != 1) { // 仅为 Level 1 加载
        std::cout << "Load is only available for Level 1." << std::endl;
        return;
    }

    std::ifstream saveFile(SAVE_FILE_NAME, std::ios::binary);
    if (!saveFile.is_open()) {
        std::cerr << "Error: No save file found." << std::endl;
        return;
    }

    SaveData data;
    saveFile.read(reinterpret_cast<char*>(&data), sizeof(SaveData));
    saveFile.close();

    if (saveFile.gcount() != sizeof(SaveData)) {
        std::cerr << "Error: Save file is corrupted." << std::endl;
        return;
    }

    // 重置关卡状态
    ResetLevelState();

    // 应用加载的数据
    m_playerScore = data.score;
    m_currentWave = data.wave;

    // 触发UI提示
    m_showLoadMessage = true;
    m_loadMessageTimer = 3.0f; // 显示3秒

    std::cout << "Game loaded. Score: " << m_playerScore << ", Wave: " << m_currentWave << std::endl;
}

void Game::ResetLevelState() {
    // 移除所有NPC
    for (int i = 0; i < m_activeNpcCount; ++i) {
        delete m_npcPool[i];
        m_npcPool[i] = nullptr;
    }
    m_activeNpcCount = 0;

    // 移除所有子弹
    m_activeProjectileCount = 0;

    // 重置玩家状态
    m_player.SetPosition(1378.0f, 2106.0f); // 重置到初始位置
    m_player.RestoreFullHealth(); // *** 修改: 使用公有函数恢复生命值 ***

    // 重置波次逻辑，以便下一帧的UpdateSpawning可以生成新一波
    m_waveInProgress = false;
    m_waveCooldownTimer = 0.0f;
}

