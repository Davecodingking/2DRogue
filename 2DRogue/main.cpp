#include "Game.h"

int main(int argc, char* argv[])
{
    // ����һ����Ϸ����
    Game game;

    // ��ʼ����Ϸ������ɹ���ʼ����
    if (game.Initialize()) {
        game.Run();
    }

    // ��Ϸ������ִ��������
    game.Shutdown();

    return 0;
}

