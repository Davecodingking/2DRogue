#include "Game.h"
#include "Character.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <ctime> 
#include <cmath>

Game::Game()
    : m_cameraX(0), m_cameraY(0), m_zoom(1.5f), m_isRunning(false),
    m_activeNpcCount(0), m_activeProjectileCount(0), m_gameTimer(0.0f),
    m_spawnPointCount(0), m_bossSpawnPointCount(0), m_playerDamageCooldown(0.0f),
    m_currentLevel(0), m_currentWave(1), m_waveInProgress(false), m_waveCooldownTimer(0.0f),
    m_level2_npcSpawnedCount(0), m_level2_spawnTimer(1.0f), m_bossSpawned(false)
{
    for (int i = 0; i < MAX_NPCS; ++i) m_npcPool[i] = nullptr;
    for (int i = 0; i < MAX_PROJECTILES; ++i) m_projectilePool[i] = Projectile();
    for (int i = 0; i < MAX_SPAWN_POINTS; ++i) {
        m_npcSpawnPoints[i] = { 0, 0 };
        m_bossSpawnPoints[i] = { 0, 0 };
    }
    srand(static_cast<unsigned int>(time(0)));
}

Game::~Game() {
    Shutdown();
}

bool Game::Initialize(const std::string& levelFile) {
    m_window.create(1024, 768, "2D Mech Rogue Game");
    std::cout << "Window created successfully." << std::endl;

    if (levelFile.find("level1") != std::string::npos) {
        m_currentLevel = 1;
        std::cout << "Loading Level 1..." << std::endl;
    }
    else if (levelFile.find("level2") != std::string::npos) {
        m_currentLevel = 2;
        std::cout << "Loading Level 2..." << std::endl;
    }
    else {
        m_currentLevel = 0; // Unknown level
        std::cout << "Loading unknown level..." << std::endl;
    }


    if (!m_level.loadFromFile(levelFile)) {
        return false;
    }
    m_level.setInfinite(false);
    m_level.setZoom(m_zoom);

    if (!m_player.Load()) {
        return false;
    }

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
        std::cout << "No hero respawn found in map, setting to default position." << std::endl;
    }

    m_isRunning = true;
    return true;
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

void Game::Shutdown() {
    for (int i = 0; i < m_activeNpcCount; ++i) {
        if (m_npcPool[i]) {
            delete m_npcPool[i];
            m_npcPool[i] = nullptr;
        }
    }
    m_activeNpcCount = 0;
    std::cout << "游戏关闭，NPC资源已清理。" << std::endl;
}

void Game::ProcessInput() {
    m_window.checkInput();
    if (m_window.keyPressed(VK_ESCAPE)) m_isRunning = false;

    // Zoom
    int wheelDelta = m_window.getMouseWheel();
    if (wheelDelta != 0) {
        m_zoom += wheelDelta * 0.001f;
        if (m_zoom < 0.5f) m_zoom = 0.5f;
        if (m_zoom > 3.0f) m_zoom = 3.0f;
        m_level.setZoom(m_zoom);
    }

    // Player Shooting
    if (m_window.mouseButtonPressed(GamesEngineeringBase::MouseButton::MouseLeft) && m_player.CanFire()) {
        m_player.ResetFireCooldown();
        float angle = m_player.getAimAngle();
        Projectile::Type projType = (m_player.GetCurrentWeapon() == Hero::WeaponType::MACHINE_GUN) ? Projectile::MACHINE_GUN : Projectile::CANNON;

        // ** 关键修正 **
        // 1. 使用修正后的 GetFirePosX/Y 从上半身发射
        // 2. 将 sin(angle) 取反 (-sin(angle)) 来校正Y轴，确保射击方向和鼠标一致
        SpawnProjectile(m_player.GetFirePosX(), m_player.GetFirePosY(), cos(angle), -sin(angle), projType, Projectile::PLAYER);
    }

    // Weapon Switch
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

    if (m_playerDamageCooldown > 0) {
        m_playerDamageCooldown -= deltaTime;
    }

    UpdateSpawning(deltaTime);
    m_player.Update(m_level, deltaTime);
    UpdateNPCs(deltaTime);
    UpdateProjectiles(deltaTime);
    CheckCollisions();

    // Camera Update
    m_cameraX = (int)(m_player.getX() - (m_window.getWidth() / 2.0f / m_zoom) + (m_player.getWidth() / 2.0f));
    m_cameraY = (int)(m_player.getY() - (m_window.getHeight() / 2.0f / m_zoom) + (m_player.getHeight() / 2.0f));
    m_player.UpdateAiming(m_window, m_cameraX, m_cameraY, m_zoom);

    // Camera bounds check
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
        if (!proj.IsActive()) continue;

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
                        proj.Deactivate();
                        break;
                    }
                }
            }
        }
        else { // ENEMY bullet
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


void Game::UpdateNPCs(float deltaTime) {
    for (int i = 0; i < m_activeNpcCount; ++i) {
        NPC* npc = m_npcPool[i];
        if (npc) {
            npc->Update(m_level, deltaTime);
            npc->UpdateAI(m_player.getX(), m_player.getY(), deltaTime);

            if (npc->canFire()) {
                npc->resetFireCooldown();
                float npcX = npc->getX() + npc->getWidth() / 2.0f;
                float npcY = npc->getY() + npc->getHeight() / 2.0f;
                float dirX = m_player.getX() - npcX;
                float dirY = m_player.getY() - npcY;
                SpawnProjectile(npcX, npcY, dirX, dirY, Projectile::ENEMY_BULLET, Projectile::ENEMY);
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
        if (m_currentWave > 3) return; // Level 1 finished

        if (!m_waveInProgress && m_activeNpcCount == 0) {
            if (m_waveCooldownTimer <= 0) {
                m_waveCooldownTimer = 5.0f; // Start 5 second countdown
            }
            else {
                m_waveCooldownTimer -= deltaTime;
                if (m_waveCooldownTimer <= 0) {
                    int npcsToSpawn = m_currentWave * 2;
                    std::cout << "Starting Wave " << m_currentWave << " with " << npcsToSpawn << " NPCs." << std::endl;
                    for (int i = 0; i < npcsToSpawn; ++i) {
                        if (m_activeNpcCount >= MAX_NPCS) break;
                        int spawnIndex = rand() % m_spawnPointCount;
                        NPC::NPCType type = static_cast<NPC::NPCType>(rand() % 3); // Melee, Shooter, Sniper
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
        // Phase 1: Spawn 20 minions
        if (m_level2_npcSpawnedCount < 20) {
            m_level2_spawnTimer -= deltaTime;
            if (m_level2_spawnTimer <= 0) {
                int spawnIndex = rand() % m_spawnPointCount;
                NPC::NPCType type = static_cast<NPC::NPCType>(rand() % 3); // Melee, Shooter, Sniper
                SpawnNPC(m_npcSpawnPoints[spawnIndex].x, m_npcSpawnPoints[spawnIndex].y, type);
                m_level2_npcSpawnedCount++;
                m_level2_spawnTimer = 1.0f;
                std::cout << "Spawned minion " << m_level2_npcSpawnedCount << "/20." << std::endl;
            }
        }
        // Phase 2: Spawn Boss
        else if (!m_bossSpawned) {
            if (m_bossSpawnPointCount > 0) {
                int spawnIndex = rand() % m_bossSpawnPointCount;
                SpawnNPC(m_bossSpawnPoints[spawnIndex].x, m_bossSpawnPoints[spawnIndex].y, NPC::BOSS_AIRCRAFT);
                m_bossSpawned = true;
                std::cout << "BOSS HAS SPAWNED!" << std::endl;
            }
            else {
                std::cerr << "Error: Level 2 trying to spawn boss but no boss_npc_respawn point found!" << std::endl;
                m_bossSpawned = true; // Prevent trying again
            }
        }
        break;
    }
    }
}


void Game::Render() {
    m_window.clear();
    m_level.render(m_window);
    for (int i = 0; i < m_activeProjectileCount; ++i) if (m_projectilePool[i].IsActive()) m_projectilePool[i].Render(m_window, m_cameraX, m_cameraY, m_zoom);
    for (int i = 0; i < m_activeNpcCount; ++i) if (m_npcPool[i]) m_npcPool[i]->Render(m_window, m_cameraX, m_cameraY, m_zoom);
    m_player.Render(m_window, m_cameraX, m_cameraY, m_zoom);
    m_window.present();
}

void Game::SpawnNPC(int x, int y, NPC::NPCType type) {
    if (m_activeNpcCount >= MAX_NPCS) {
        std::cout << "NPC pool is full." << std::endl;
        return;
    }
    NPC* newNpc = new NPC(type);
    if (newNpc->Load()) {
        newNpc->SetPosition((float)x, (float)y);
        m_npcPool[m_activeNpcCount++] = newNpc;
    }
    else {
        delete newNpc;
        std::cerr << "SpawnNPC failed because resource loading failed." << std::endl;
    }
}

void Game::SpawnProjectile(float startX, float startY, float dirX, float dirY, Projectile::Type type, Projectile::Owner owner) {
    if (m_activeProjectileCount >= MAX_PROJECTILES) return;

    m_projectilePool[m_activeProjectileCount].Activate(startX, startY, dirX, dirY, type, owner);
    m_activeProjectileCount++;
}

