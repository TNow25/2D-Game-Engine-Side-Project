#pragma once

#include <glm/glm.hpp>
#include <string>
#include <SDL.h>

struct SpriteComponent
{
	std::string assetId;
	int width;
	int height;
	int zIndex;
	SDL_Rect srcRect;

	//Always have a default. This will prevent fatal build errors
	SpriteComponent(std::string assetId = "", int width = 0, int height = 0, int zIndex = 0, int srcRectX = 0, int srcRectY = 0)
	{
		this->assetId = assetId;
		this->width = width;
		this->height = height;
		this->zIndex = zIndex;
		this->srcRect = { srcRectX, srcRectY, width, height };
	}
};