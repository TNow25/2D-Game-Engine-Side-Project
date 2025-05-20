#pragma once

#include "../ECS/ECS.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/TransformComponent.h"


class MovementSystem: public System
{
public:
	MovementSystem()
	{
		//TODO: 
		//RequireComponent<TransformComponent>();
		//RequireComponent(...);

		RequireComponent<TransformComponent>();
		RequireComponent<RigidBodyComponent>();

	}

	void Update(double deltaTime)
	{
		//TODO:
		//update entitiy position based on velocity
		// loop all entities that the system is interested in
		// for(auto entity: GetEntities(){}
		//every frame of the game loop

		for (auto entity : GetSystemEntities())
		{
			//update entity position based on velocity
			auto& transform = entity.GetComponent<TransformComponent>();
			const auto rigidBody = entity.GetComponent<RigidBodyComponent>();

			transform.position.x += rigidBody.velocity.x * deltaTime;
			transform.position.y += rigidBody.velocity.y * deltaTime;
		
			/*
			Logger::Log
			(
				"Entity id = " + 
				std::to_string(entity.GetId()) + 
				" position is now (" + 
				std::to_string(transform.position.x) + 
				", " + 
				std::to_string(transform.position.x) + ")"
			);
			*/
		}



	}
};