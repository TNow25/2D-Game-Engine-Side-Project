#pragma once

#include <glm/glm.hpp>

struct TransformComponent
{
	glm::vec2 position;
	glm::vec2 scale;
	double rotation;

	//need constructor
	//if nothing is passed, there are default values,
	//which here are everything following an "=" sign
	TransformComponent(glm::vec2 position = glm::vec2(0,0), glm::vec2 scale = glm::vec2(1,1), double rotation = 0.0)
	{
		this->position = position;
		// "this" is used to differentiate between the member variable
		// called position and the parametre called position.
		// "this->positon is member variable of this struct
		this->scale = scale;
		this->rotation = rotation;
	}
	//struct now has data (stuff at top) and a way of initialising a transform component usin a constructor method, the TransformComponent()
};

