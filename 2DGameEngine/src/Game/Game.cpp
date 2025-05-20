#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>
#include "Game.h"
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Systems/MovementSystem.h"
#include "../Components/SpriteComponent.h"
#include "../Systems/RenderSystem.h"	
#include "../Components/AnimationComponent.h"
#include <fstream>

//use "" when the file being included is in the same folder, otherwise use <> as this signifies for the compiler to search for the file in the dependencies

/* SDL uses a double buffer system
*  This means that when drawing things on the screen, it renders
*  things bit by bit on one buffer, sometimes jumping from item
*  to item, and then once everything has been drawn, the front
*  and back buffers are swapped out. This prevents any weird
*  looking drawing from being displayed while things are being
*  drawn to the screen.
*/

//Constructor
Game::Game() // "Game::" is the scope resolution, or, who owns the method. In this case, "Game" owns the method Game()
{
	isRunning = false;
	//registry = new Registry(); //Replace this with smart pointer
	registry = std::make_unique<Registry>(); //unqiue smart pointer
	assetStore = std::make_unique<AssetStore>();
	Logger::Log("game constructor called");
}

Game::~Game()
{
	Logger::Log("game destructor called");
}

void Game::Initialize()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) //try to initialise. 
	{ //if cannot initialise, give error message
		Logger::Err("Error initialising SDL");
		return;
	}

	//entire section below creates a window
	// 
	//next 4 lines are for full screen (borderless full screen, not true fullscreen
	SDL_DisplayMode displayMode; //for helping to fullscreen
	SDL_GetCurrentDisplayMode(0, &displayMode);
	windowWidth = displayMode.w;
	windowHeight = displayMode.h;

	window = SDL_CreateWindow   //a * means it's a pointerpointer
	(
		NULL /*"My Game Engine"*/, 	//can add title to window if wanted (see commented below)
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		windowWidth,
		windowHeight,
		//SDL_WINDOW_FULLSCREEN
		SDL_WINDOW_BORDERLESS //no special flags like borders or anything fancy
	);
	if (!window)
	{
		Logger::Err("Error creating SDL window");
		return;
	}
	// "-1" means to get the default.  Is asking what display number will this
	// renderer belong to. Could have multiple drivers or monitors.
	renderer = SDL_CreateRenderer
	(
		window,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
		//using hardware acceleration when possible and Vsync
	); //no special flags if 0
	if (!renderer)
	{
		Logger::Err("Error creating SDL renderer");
		return;
	}

	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN); //changes video mode to true full screen

	isRunning = true; //was able to do everything correctly if this is reached
}

void Game::ProcessInput()
{
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent)) // & means reference
	{ //while 
		switch (sdlEvent.type)
		{
		case SDL_QUIT:
			isRunning = false;
			break; //always add after switch case

		case SDL_KEYDOWN:
			if (sdlEvent.key.keysym.sym == SDLK_ESCAPE)
			{ //if escape key pressed
				isRunning = false;
			}
			break;
		}
	}
}

//glm::vec2 playerPosition;
//glm::vec2 playerVelocity;
//
// This below was originally in Setup, partly for testing purposes
//playerPosition = glm::vec2(10.0, 20.0); //starting position
//playerVelocity = glm::vec2(200.0, 0.0); //how much to travel per frame



void Game::LoadLevel(int level)
{
	// TODO:
	// Entitiy tank = registry.CreateEntity();
	// tank.AddComponent<TransformComponent>();
	// tank.AddComponent<BoxColliderComponent>();
	// tank.AddComponent<SpriteComponent>("./assets/images/tank.png");


	//Add the systems that need to be processed in the game
	registry->AddSystem<MovementSystem>();
	registry->AddSystem<RenderSystem>();


	//Adding assets to the asset store
	assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
	assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
	assetStore->AddTexture(renderer, "tilemap-image", "./assets/tilemaps/jungle.png");

	// Load the tilemap
	// We need to load the tilemap texture from ./assets/tilemaps/jungle.png
	// We need to load the file ./assets/tilemaps/jungle.map 
	int tileSize = 32;
	double tileScale = 2.0;
	int mapNumCols = 25;
	int mapNumRows = 20;

	std::fstream mapFile;
	mapFile.open("./assets/tilemaps/jungle.map");

	for (int y = 0; y < mapNumRows; y++)
	{
		for (int x = 0; x < mapNumCols; x++)
		{ //there are two digits for each tilemap sprite in the mapFile e.g.21,08,13
			char ch;
			mapFile.get(ch);
			int srcRectY = std::atoi(&ch) * tileSize; //atoi = ascii to integer
			mapFile.get(ch);
			int srcRectX = std::atoi(&ch) * tileSize;
			mapFile.ignore(); //skip comma

			Entity tile = registry->CreateEntity(); 
			tile.AddComponent<TransformComponent>(glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)), glm::vec2(tileScale, tileScale), 0.0); //add transform component to tile entity, based on x and y column and row position
			tile.AddComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, 0, srcRectX, srcRectY); //add sprite based on tilemap in assetstore, with size (32,32), and where in the png is the subsection for the source rectangle
			//tiles are alawys going to be rendered first so to not be above other assets, therefore we pass zIndex 0
		}
	}
	mapFile.close();  

	//create an entity
	Entity tank = registry->CreateEntity();

	//Add some components to that entity
	//registry->AddComponent<TranformComponent>(tank, glm::vec2(10.0, 30.0), glm::vec2(1.0, 1.0), 0.0);
	//registry->AddComponent<RigidBodyComponent>(tank, glm::vec2(50.0, 0.0));
	//The two lines above are an older way to add things.
	//Now we just ask to add the entity itself, without asking the registry ourselves
	tank.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0); //starting position, scale, rotation
	tank.AddComponent<RigidBodyComponent>(glm::vec2(20.0, 0.0)); //velocity
	tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 2); //image name, size in pixels, size in pixels, zIndex

	//Remove a component from the entity
	//tank.RemoveComponent<TranformComponent>();


	Entity truck = registry->CreateEntity();
	truck.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0); //position, scale, rotation
	truck.AddComponent<RigidBodyComponent>(glm::vec2(25.0, 0.0)); //velocity
	truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 1); //Size


	//helicopter
	Entity helicopter = registry->CreateEntity();
	helicopter.AddComponent<TransformComponent>(glm::vec2(50.0, 10.0), glm::vec2(1.0, 1.0), 0.0); //starting position, scale, rotation
	helicopter.AddComponent<RigidBodyComponent>(glm::vec2(20.0, 0.0)); //velocity
	helicopter.AddComponent<SpriteComponent>("chopper-image", 32, 32, 2); //image name, size in pixels, size in pixels, zIndex
	helicopter.AddComponent<AnimationComponent>();


}




void Game::Setup()
{
/*
* All of this has been moeved to LoadLevel()
* 
	// TODO:
	// Entitiy tank = registry.CreateEntity();
	// tank.AddComponent<TransformComponent>();
	// tank.AddComponent<BoxColliderComponent>();
	// tank.AddComponent<SpriteComponent>("./assets/images/tank.png");


	//Add the systems that need to be processed in the game
	registry->AddSystem<MovementSystem>();
	registry->AddSystem<RenderSystem>();


	//Adding assets to the asset store
	assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
	assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");


	//create an entity
	Entity tank = registry->CreateEntity();

	//Add some components to that entity
	//registry->AddComponent<TranformComponent>(tank, glm::vec2(10.0, 30.0), glm::vec2(1.0, 1.0), 0.0);
	//registry->AddComponent<RigidBodyComponent>(tank, glm::vec2(50.0, 0.0));
	//The two lines above are an older way to add things.
	//Now we just ask to add the entity itself, without asking the registry ourselves
	tank.AddComponent<TranformComponent>(glm::vec2(10.0, 0.0), glm::vec2(1.0, 1.0), 0.0); //starting position, scale, rotation
	tank.AddComponent<RigidBodyComponent>(glm::vec2(20.0, 0.0)); //velocity
	tank.AddComponent<SpriteComponent>("tank-image", 32, 32);
	
	//Remove a component from the entity
	//tank.RemoveComponent<TranformComponent>();
	

	Entity truck = registry->CreateEntity();

	truck.AddComponent<TranformComponent>(glm::vec2(20.0, 20.0), glm::vec2(1.0, 1.0), 0.0); //position, scale, rotation
	truck.AddComponent<RigidBodyComponent>(glm::vec2(40.0, 0.0)); //velocity
	truck.AddComponent<SpriteComponent>("truck-image", 32, 32); //Size
	
	
*/ //all of this has been moved to LoadLevel()
	
	LoadLevel(1);
}

void Game::Update()
{
	//TODO: If we're too fast, waste some time until we reach MILLISECS_PER_FRAME
	//while (!SDL_TICKS_PASSED(SDL_GetTicks(), millisecsPreviousFrame + MILLISECS_PER_FRAME));
	int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecsPreviousFrame);
	if ((timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME))
	{
		SDL_Delay(timeToWait);
	}
	//if i want an uncapped framerate, I can comment out the if statement above

	//the difference in ticks since the last frame, converted to seconds
	double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0f;

	//store the current frame time
	millisecsPreviousFrame = SDL_GetTicks();

	//these two lines below no longer needed, this will be done in the MovementSystem
	//playerPosition.x += playerVelocity.x * deltaTime;
	//playerPosition.y += playerVelocity.y * deltaTime;

	/*
	* TODO:
	* MovementSystem.Update();
	* CollisionSystem.Update();
	* DamageSystem.Update();
	
	*/


	//Update the registry to process the entities that are waiting to be created/deleted
	registry->Update();

	//Invoke all systems that need to update
	registry->GetSystem<MovementSystem>().Update(deltaTime);
	//registry->GetSystem<CollisionSystem>().Update();

	

}

void Game::Render()
{
	//set colour, clear the renderer, present the renderer

	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255); //colour and transparency. R,G,B,T
	//this above is the background colour, which is grey
	SDL_RenderClear(renderer);

/*
* 
	
	//draw rectangle
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); //need to have this to change the draw colour, otherwise it would stay the same from the previous colour (like the window colour)
	SDL_Rect player = { 10,10,20,20 }; //x pos, y pos, width, height
	//need to ask to render the object now, so:
	SDL_RenderFillRect(renderer, &player); //is looking for pointer to the SDL_Rect, so we reference (in this example) the player
	

	//render png
	SDL_Surface* surface = IMG_Load("./assets/images/tank-tiger-right.png"); //loads png in this file directory
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface); //loads the image from the surface we created into the texture
	SDL_FreeSurface(surface); //surface is no longer needed so we delete it to free up space

	//what is the destination rectangle we want to place the texture? It's this
	SDL_Rect dtsRect = {
		static_cast<int>(playerPosition.x), //start position
		static_cast<int>(playerPosition.y), //static_case<int> converts the float to an int
		32, //size
		32
	}; //position and size of what we want to use in the destination
	SDL_RenderCopy(renderer, texture, NULL, &dtsRect); //SDL is looking for the source rectangle and destination rectangle. Basically, it's asking how much of the image do we want to use. NULL can be used to say we want to use all of the PNG. Need to reference the position of the rectangle
	SDL_DestroyTexture(texture); //free up the space used for the texture

*/

	//TODO: Render game objects...

	registry->GetSystem<RenderSystem>().Update(renderer, assetStore);

	SDL_RenderPresent(renderer);


}

void Game::Run()
{
	Setup();
	while (isRunning)
	{
		ProcessInput();
		Update();
		Render();
	}
}

void Game::Destroy()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	//delete registry;
}