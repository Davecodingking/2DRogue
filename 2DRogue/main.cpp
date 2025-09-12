#include "Game.h"
#include <iostream>

int main(int argc, char* argv[])
{
    // 创建一个游戏对象
    Game game;

    // 初始化游戏，如果成功则开始运行
    // 您可以在这里更改加载的关卡文件，例如改为 "Resources/level2.json" 来测试第二关
    if (game.Initialize("Resources/level2.json")) {
        game.Run();
    }
    else {
        std::cout << "Game initialization failed. Press Enter to exit." << std::endl;
        std::cin.get();
    }

    // 游戏结束后，执行清理工作
    game.Shutdown();

    return 0;
}

