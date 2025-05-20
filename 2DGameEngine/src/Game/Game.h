#pragma once
//in header files, don't elaborate and make the functions, only call functions from the cpp file

/*
*
* pragma once and indef define endif can be interchangable
*
* however, sometime's it's not best to do so as it may not work depending on the compiler,
* more often than not it will work fine though.
*
* pragma once also allows us to not have to rename something
*
* has the benefit of us not having to remember to change the
* name of the definition in every .h file we create
*
*/

//	***protection guard***
//#ifndef GAME_H
//#define GAME_H
//#endif //this resides after the final bracket
//this makes the preprocessor only include a header file once

#include <SDL.h>
#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"

const int FPS = 240; //framerate we want to run the game
const int MILLISECS_PER_FRAME = 1000 / FPS; //target frametime. 1000=1second, determins how many miliseconds are per frame

class Game
{
private:
	bool isRunning;
	int millisecsPreviousFrame = 0;
	SDL_Window* window;
	SDL_Renderer* renderer;

	//Registry* registry; //Replaced with smart pointer
	std::unique_ptr<Registry> registry;
	std::unique_ptr<AssetStore> assetStore;

public:
	Game(); //constructor
	~Game(); //destructor

	void Initialize();
	void Run();
	void Setup();
	void LoadLevel(int level);
	void ProcessInput();
	void Update();
	void Render();
	void Destroy();

	int windowWidth;
	int windowHeight;

};

