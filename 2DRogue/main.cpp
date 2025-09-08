#include "Game.h"

int main(int argc, char* argv[])
{
    // 创建一个游戏对象
    Game game;

    // 初始化游戏，如果成功则开始运行
    if (game.Initialize()) {
        game.Run();
    }

    // 游戏结束后，执行清理工作
    game.Shutdown();

    return 0;
}

