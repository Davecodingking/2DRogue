#pragma once
#include "Hero.h"
#include "Level.h"
#include "NPC.h"
#include "Projectile.h"
#include "LaserBeam.h"
#include "GamesEngineeringBase.h"
#include <string>
#include <windows.h> // For RECT

const int MAX_PICKUPS = 10;
const int MAX_HIT_NPCS = 50;

class Game {
public:
    Game();
    ~Game();
    bool Initialize(const std::string& levelFile);
    void Run();
    void Shutdown();

private:
    // --- 游戏状态管理 ---
    enum class GameState { MAIN_MENU, PLAYING, PAUSED };
    GameState m_gameState;

    // ======================================================================
    // ==                     新增代码: 游戏子状态管理                   ==
    // ======================================================================
    // 用于处理游戏中的特殊过渡阶段，如死亡和胜利
    enum class PlayingState { NORMAL, PLAYER_DYING, VICTORY };
    PlayingState m_playingState;
    float m_endGameTimer; // 用于控制结束画面的计时器
    // ======================================================================

    // --- 状态处理函数 ---
    void ProcessInput();
    void Update(float deltaTime);
    void Render();

    void ProcessInputMainMenu();
    void UpdateMainMenu(float deltaTime);
    void RenderMainMenu();

    void ProcessInputPlaying(float deltaTime);
    void UpdatePlaying(float deltaTime);
    void RenderPlaying();

    void ProcessInputPaused();
    void UpdatePaused(float deltaTime);
    void RenderPaused();

    // --- 游戏逻辑与辅助函数 ---
    void UpdateNPCs(float deltaTime);
    void UpdateProjectiles(float deltaTime);
    void UpdateSpawning(float deltaTime);
    void UpdateLasers(float deltaTime);
    void CheckCollisions();
    void SpawnNPC(int x, int y, NPC::NPCType type);
    void SpawnProjectile(float startX, float startY, float angle, Projectile::Type type, Projectile::Owner owner);
    void FireLaser();
    void RenderPickups(float deltaTime);
    void RenderUI();
    void UpdateCamera();

    // ======================================================================
    // ==                    新增代码: 新增辅助函数声明                    ==
    // ======================================================================
    void DrawFullscreenOverlay(int r, int g, int b, float alpha);
    void ReturnToMainMenu();
    // ======================================================================

    // --- 存档/读档/关卡管理 ---
    void SaveGame();
    void LoadGame();
    void ResetLevelState();
    void StartNewGame();
    void LoadLevel(const std::string& levelFile);

    // --- 游戏世界对象 ---
    GamesEngineeringBase::Window m_window;
    Level m_level;
    Hero m_player;

    // --- 对象池 ---
    static const int MAX_NPCS = 50;
    NPC* m_npcPool[MAX_NPCS];
    int m_activeNpcCount;

    static const int MAX_PROJECTILES = 100;
    Projectile m_projectilePool[MAX_PROJECTILES];
    int m_activeProjectileCount;

    static const int MAX_LASERS = 10;
    LaserBeam m_laserPool[MAX_LASERS];
    int m_activeLaserCount;
    NPC* m_npcsHitThisFrame[MAX_HIT_NPCS];
    int m_npcsHitCount;

    // --- 拾取物 ---
    struct Pickup {
        int x, y;
        bool isCollected = false;
        float floatOffset = 0.0f;
    };
    Pickup m_laserPickups[MAX_PICKUPS];
    int m_pickupCount;

    // --- 出生点 ---
    struct SpawnPoint { int x; int y; };
    static const int MAX_SPAWN_POINTS = 10;
    SpawnPoint m_npcSpawnPoints[MAX_SPAWN_POINTS];
    int m_spawnPointCount;
    SpawnPoint m_bossSpawnPoints[MAX_SPAWN_POINTS];
    int m_bossSpawnPointCount;

    // --- 计时器与状态变量 ---
    float m_gameTimer;
    float m_playerDamageCooldown;
    int m_currentLevel;
    int m_currentWave;
    bool m_waveInProgress;
    float m_waveCooldownTimer;
    int m_level2_npcSpawnedCount;
    float m_level2_spawnTimer;
    bool m_bossSpawned;
    int m_cameraX;
    int m_cameraY;
    float m_zoom;
    bool m_isRunning;

    // --- UI 相关 ---
    GamesEngineeringBase::Image m_healthSheet;
    GamesEngineeringBase::Image m_machineGunIcon;
    GamesEngineeringBase::Image m_cannonIcon;
    GamesEngineeringBase::Image m_laserGunIcon;
    GamesEngineeringBase::Image m_logoImage;
    GamesEngineeringBase::Image m_menuImage;
    GamesEngineeringBase::Image m_menuBackgroundImage;
    GamesEngineeringBase::Image m_settingImage;
    GamesEngineeringBase::Image m_savequitImage;
    GamesEngineeringBase::Image m_tutorialImage;
    bool m_showTutorial;

    RECT m_startButtonRect;
    RECT m_loadButtonRect;
    RECT m_newGameButtonRect;
    RECT m_settingButtonRect;
    RECT m_pauseSaveButtonRect;
    RECT m_pauseQuitButtonRect;

    int m_playerScore;
    int m_fps;
    int m_frameCount;
    float m_fpsTimer;
    bool m_showLoadMessage;
    float m_loadMessageTimer;

    GamesEngineeringBase::SoundManager* m_soundManager;

    std::string m_machineGunSoundFile;
    std::string m_plasmaGunSoundFile;
    std::string m_laserSoundFile;
};

