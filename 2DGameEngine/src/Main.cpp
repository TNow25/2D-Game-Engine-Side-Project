#include <iostream>
#include "./Game/game.h"


int main(int argc, char* argv[]) //arguement character, array of argument values
{
    Game game; //not using "new" means it will be stored in the stack and then destroyed when the scope ends

    game.Initialize();
    game.Run();
    game.Destroy();


    return 0;
}
