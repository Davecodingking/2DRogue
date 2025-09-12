#include "Game.h"
#include <iostream>

int main(int argc, char* argv[])
{
    // ����һ����Ϸ����
    Game game;

    // ��ʼ����Ϸ������ɹ���ʼ����
    // ��������������ļ��صĹؿ��ļ��������Ϊ "Resources/level2.json" �����Եڶ���
    if (game.Initialize("Resources/level2.json")) {
        game.Run();
    }
    else {
        std::cout << "Game initialization failed. Press Enter to exit." << std::endl;
        std::cin.get();
    }

    // ��Ϸ������ִ��������
    game.Shutdown();

    return 0;
}

