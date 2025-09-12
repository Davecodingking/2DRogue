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
    m_spawnPointCount(0), m_waveSpawnTimer(WAVE_INTERVAL),
    m_npcsToSpawn(2), m_playerDamageCooldown(0.0f)
{
    for (int i = 0; i < MAX_NPCS; ++i) m_npcPool[i] = nullptr;
    for (int i = 0; i < MAX_PROJECTILES; ++i) m_projectilePool[i] = Projectile();
    for (int i = 0; i < MAX_SPAWN_POINTS; ++i) m_npcSpawnPoints[i] = { 0, 0 };
    srand(static_cast<unsigned int>(time(0)));
}

Game::~Game() {
    Shutdown();
}

bool Game::Initialize() {
    m_window.create(1024, 768, "2D Mech Rogue Game");
    std::cout << "Window created successfully." << std::endl;


    std::string levelFile = "Resources/level1.json";
    if (!m_level.loadFromFile(levelFile)) {
        // Error message is now inside level loader
        system("pause");
        return false;
    }
    m_level.setInfinite(false);
    m_level.setZoom(m_zoom);

    if (!m_player.Load()) {
        // Error message is now inside hero loader
        system("pause");
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
            std::cout << "NPC Spawn Point registered at (" << obj.x << ", " << obj.y << ")" << std::endl;
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
        if (m_zoom > 1.0f) m_zoom = 1.0f;
        m_level.setZoom(m_zoom);
    }

    // Player Shooting
    if (m_window.mouseButtonPressed(GamesEngineeringBase::MouseButton::MouseLeft) && m_player.CanFire()) {
        m_player.ResetFireCooldown();
        // 关键修正: 使用与上半身方向完全一致的量化角度进行射击
        float angle = m_player.GetTorsoFireAngle();
        Projectile::Type projType = (m_player.GetCurrentWeapon() == Hero::WeaponType::MACHINE_GUN) ? Projectile::MACHINE_GUN : Projectile::CANNON;

        SpawnProjectile(m_player.GetFirePosX(), m_player.GetFirePosY(), cos(angle), sin(angle), projType, Projectile::PLAYER);
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
    // Player vs NPC (Pushback and Slow)
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

    // Projectile vs Characters
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
                        npc->TakeDamage(10); // Example damage
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
                    m_playerDamageCooldown = 0.5f; // 0.5 seconds invulnerability
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

            if (npc->getNPCType() == NPC::SHOOTER && npc->canFire()) {
                npc->resetFireCooldown();
                float npcX = npc->getX() + npc->getWidth() / 2.0f;
                float npcY = npc->getY() + npc->getHeight() / 2.0f;
                float dirX = m_player.getX() - npcX;
                float dirY = m_player.getY() - npcY;
                SpawnProjectile(npcX, npcY, dirX, dirY, Projectile::ENEMY_BULLET, Projectile::ENEMY);
            }
        }
    }
    // Cleanup dead NPCs
    for (int i = m_activeNpcCount - 1; i >= 0; --i) {
        if (m_npcPool[i] && m_npcPool[i]->getCurrentState() == NPC::State::DEAD) {
            delete m_npcPool[i];
            m_npcPool[i] = m_npcPool[m_activeNpcCount - 1];
            m_npcPool[m_activeNpcCount - 1] = nullptr;
            m_activeNpcCount--;
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

    m_waveSpawnTimer -= deltaTime;
    if (m_waveSpawnTimer <= 0) {
        for (int i = 0; i < m_npcsToSpawn; ++i) {
            if (m_activeNpcCount >= MAX_NPCS) break;
            int spawnIndex = rand() % m_spawnPointCount;
            int spawnX = m_npcSpawnPoints[spawnIndex].x;
            int spawnY = m_npcSpawnPoints[spawnIndex].y;

            NPC::NPCType type = (rand() % 3 == 0) ? NPC::SHOOTER : NPC::MELEE;
            SpawnNPC(spawnX, spawnY, type, 100, 80.0f, 0.3f);
        }
        if (m_npcsToSpawn < 8) m_npcsToSpawn++;
        m_waveSpawnTimer = WAVE_INTERVAL;
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

void Game::SpawnNPC(int x, int y, NPC::NPCType type, int health, float speed, float renderScale) {
    if (m_activeNpcCount >= MAX_NPCS) {
        std::cout << "NPC池已满，无法生成新的NPC。" << std::endl;
        return;
    }
    NPC* newNpc = new NPC(type);
    if (newNpc->Load()) {
        newNpc->SetPosition((float)x, (float)y);
        newNpc->InitializeStats(health, speed, renderScale);
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

