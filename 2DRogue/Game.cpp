#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <ctime> 
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Game.cpp's content is quite long, so I'll only show the modified/added functions for brevity.
// The rest of the file remains unchanged.

// --- Game Constructor & Destructor ---
Game::Game()
    : m_cameraX(0), m_cameraY(0), m_zoom(1.5f), m_isRunning(false),
    m_activeNpcCount(0), m_activeProjectileCount(0), m_gameTimer(0.0f),
    m_spawnPointCount(0), m_bossSpawnPointCount(0), m_playerDamageCooldown(0.0f),
    m_currentLevel(0), m_currentWave(1), m_waveInProgress(false), m_waveCooldownTimer(0.0f),
    m_level2_npcSpawnedCount(0), m_level2_spawnTimer(1.0f), m_bossSpawned(false)
{
    for (int i = 0; i < MAX_NPCS; ++i) m_npcPool[i] = nullptr;
    for (int i = 0; i < MAX_SPAWN_POINTS; ++i) {
        m_npcSpawnPoints[i] = { 0, 0 };
        m_bossSpawnPoints[i] = { 0, 0 };
    }
    srand(static_cast<unsigned int>(time(0)));
}

Game::~Game() {
    Shutdown();
}

// --- Initialize ---
bool Game::Initialize(const std::string& levelFile) {
    m_window.create(1024, 768, "2D Mech Rogue Game");

    if (levelFile.find("level1") != std::string::npos) m_currentLevel = 1;
    else if (levelFile.find("level2") != std::string::npos) m_currentLevel = 2;
    else m_currentLevel = 0;

    if (!m_level.loadFromFile(levelFile)) return false;
    m_level.setInfinite(false);
    m_level.setZoom(m_zoom);

    if (!m_player.Load()) return false;
    if (!Projectile::LoadAssets()) return false;
    if (!NPC::LoadSharedAssets()) return false; // *** ADDED: Load NPC shared assets ***

    const GameObject* objects = m_level.getGameObjects();
    bool playerSpawnPointFound = false;
    for (int i = 0; i < m_level.getObjectCount(); ++i) {
        const GameObject& obj = objects[i];
        if (obj.type == "hero_respawn") {
            m_player.SetPosition((float)obj.x, (float)obj.y);
            playerSpawnPointFound = true;
        }
        else if (obj.type == "generic_npc_respawn" && m_spawnPointCount < MAX_SPAWN_POINTS) {
            m_npcSpawnPoints[m_spawnPointCount++] = { obj.x, obj.y };
        }
        else if (obj.type == "boss_npc_respawn" && m_bossSpawnPointCount < MAX_SPAWN_POINTS) {
            m_bossSpawnPoints[m_bossSpawnPointCount++] = { obj.x, obj.y };
        }
    }
    if (!playerSpawnPointFound) {
        m_player.SetPosition(1378.0f, 2106.0f);
    }
    m_isRunning = true;
    return true;
}

// --- Shutdown ---
void Game::Shutdown() {
    for (int i = 0; i < m_activeNpcCount; ++i) {
        if (m_npcPool[i]) delete m_npcPool[i];
        m_npcPool[i] = nullptr;
    }
    m_activeNpcCount = 0;
    Projectile::FreeAssets();
    NPC::FreeSharedAssets(); // *** ADDED: Free NPC shared assets ***
    std::cout << "Game shut down, resources cleaned up." << std::endl;
}

// --- UpdateNPCs ---
void Game::UpdateNPCs(float deltaTime) {
    for (int i = 0; i < m_activeNpcCount; ++i) {
        NPC* npc = m_npcPool[i];
        if (npc) {
            npc->Update(m_level, deltaTime);
            npc->UpdateAI(m_player.getX(), m_player.getY(), deltaTime);

            // *** ADDED: Pass Level to MoveTowards for collision checking ***
            if (npc->getCurrentState() == NPC::WALKING) {
                float targetX = m_player.getX();
                float targetY = m_player.getY();

                // If shooter/sniper is too close, it should move away
                if (npc->getNPCType() == NPC::SHOOTER || npc->getNPCType() == NPC::SNIPER) {
                    float dirX = targetX - npc->getX();
                    float dirY = targetY - npc->getY();
                    float distance = sqrt(dirX * dirX + dirY * dirY);
                    if (distance < 250.0f) { // a bit less than preferred distance
                        targetX = npc->getX() - dirX;
                        targetY = npc->getY() - dirY;
                    }
                }
                npc->MoveTowards(m_level, targetX, targetY, deltaTime);
            }


            if (npc->canFire()) {
                npc->resetFireCooldown();
                float npcX = npc->getX() + npc->getWidth() / 2.0f;
                float npcY = npc->getY() + npc->getHeight() / 2.0f;
                float dirX = m_player.getX() - npcX;
                float dirY = m_player.getY() - npcY;
                float angle = atan2(-dirY, dirX);
                SpawnProjectile(npcX, npcY, angle, Projectile::ENEMY_BULLET, Projectile::ENEMY);
            }
        }
    }
    for (int i = m_activeNpcCount - 1; i >= 0; --i) {
        if (m_npcPool[i] && m_npcPool[i]->getCurrentState() == NPC::State::DEAD) {
            delete m_npcPool[i];
            m_npcPool[i] = m_npcPool[m_activeNpcCount - 1];
            m_npcPool[m_activeNpcCount - 1] = nullptr;
            m_activeNpcCount--;
            m_waveInProgress = m_activeNpcCount > 0;
        }
    }
}


// --- The rest of Game.cpp remains unchanged. Only Initialize, Shutdown, and UpdateNPCs were modified. ---
// --- To save space, the unchanged functions like Run, ProcessInput, Update, etc., are not repeated here. ---
// --- I will now add back the RenderUI function and the call in Render that I provided previously. ---

void Game::RenderUI() {
    Hero::WeaponType currentWeapon = m_player.GetCurrentWeapon();
    unsigned char r, g, b;

    if (currentWeapon == Hero::WeaponType::MACHINE_GUN) {
        r = 255; g = 255; b = 0; // Yellow
    }
    else {
        r = 255; g = 165; b = 0; // Orange
    }

    for (int y = 10; y < 40; ++y) {
        for (int x = 10; x < 40; ++x) {
            if (x < (int)m_window.getWidth() && y < (int)m_window.getHeight()) {
                m_window.draw(x, y, r, g, b);
            }
        }
    }
}

void Game::Render() {
    m_window.clear();
    m_level.render(m_window);
    for (int i = 0; i < m_activeNpcCount; ++i) if (m_npcPool[i]) m_npcPool[i]->Render(m_window, m_cameraX, m_cameraY, m_zoom);
    m_player.Render(m_window, m_cameraX, m_cameraY, m_zoom);
    for (int i = 0; i < m_activeProjectileCount; ++i) if (m_projectilePool[i].IsActive()) m_projectilePool[i].Render(m_window, m_cameraX, m_cameraY, m_zoom);
    RenderUI();
    m_window.present();
}

// Stubs for the rest of the functions to make this file complete
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
        float angle = m_player.getAimAngle();
        Projectile::Type projType = (m_player.GetCurrentWeapon() == Hero::WeaponType::MACHINE_GUN) ? Projectile::MACHINE_GUN : Projectile::CANNON;
        SpawnProjectile(m_player.GetFirePosX(), m_player.GetFirePosY(), angle, projType, Projectile::PLAYER);
    }
    static bool q_pressed = false;
    if (m_window.keyPressed('Q') && !q_pressed) {
        m_player.SwitchWeapon();
        q_pressed = true;
    }
    if (!m_window.keyPressed('Q')) {
        q_pressed = false;
    }
}
void Game::Update(float deltaTime) {
    if (!m_isRunning) return;
    if (m_playerDamageCooldown > 0) m_playerDamageCooldown -= deltaTime;
    UpdateSpawning(deltaTime);
    m_player.Update(m_level, deltaTime);
    UpdateNPCs(deltaTime);
    UpdateProjectiles(deltaTime);
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
}
void Game::CheckCollisions() {
    m_player.SetSlowed(false);
    float heroX = m_player.getX(), heroY = m_player.getY();
    float heroW = m_player.getWidth(), heroH = m_player.getHeight();
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
}
void Game::UpdateProjectiles(float deltaTime) {
    for (int i = 0; i < m_activeProjectileCount; ) {
        if (m_projectilePool[i].IsActive()) {
            m_projectilePool[i].Update(deltaTime);
            i++;
        }
        else {
            if (i < m_activeProjectileCount - 1) {
                m_projectilePool[i] = m_projectilePool[m_activeProjectileCount - 1];
            }
            m_activeProjectileCount--;
        }
    }
}
void Game::UpdateSpawning(float deltaTime) {
    if (m_spawnPointCount == 0) return;
    switch (m_currentLevel) {
    case 1: {
        if (m_currentWave > 3) return;
        if (!m_waveInProgress && m_activeNpcCount == 0) {
            if (m_waveCooldownTimer <= 0) m_waveCooldownTimer = 5.0f;
            else {
                m_waveCooldownTimer -= deltaTime;
                if (m_waveCooldownTimer <= 0) {
                    int npcsToSpawn = m_currentWave * 2;
                    for (int i = 0; i < npcsToSpawn; ++i) {
                        if (m_activeNpcCount >= MAX_NPCS) break;
                        int spawnIndex = rand() % m_spawnPointCount;
                        NPC::NPCType type = static_cast<NPC::NPCType>(rand() % 3);
                        SpawnNPC(m_npcSpawnPoints[spawnIndex].x, m_npcSpawnPoints[spawnIndex].y, type);
                    }
                    m_currentWave++;
                    m_waveInProgress = true;
                }
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
            else {
                m_bossSpawned = true;
            }
        }
        break;
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

