#pragma once

#include "../ECS/ECS.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include <SDL.h>
#include "../AssetStore/AssetStore.h"
#include <algorithm>

class RenderSystem: public System
{
public:
	RenderSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<SpriteComponent>();
	}

	void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore)
	{
		//Create vector with both sprite and transform component of all entities
		struct RenderableEntity
		{ //this groups the sprite and transform component together as we want to sort them together
			TransformComponent transformComponent;
			SpriteComponent spriteComponent;
		};
		std::vector<RenderableEntity> renderableEntities;
		for (auto entity : GetSystemEntities())
		{
			RenderableEntity renderableEntity;
			renderableEntity.spriteComponent = entity.GetComponent<SpriteComponent>();
			renderableEntity.transformComponent = entity.GetComponent<TransformComponent>();
			renderableEntities.emplace_back(renderableEntity);
		}

		//Sort the vector by z-index
		sort
		(
			renderableEntities.begin(), 
			renderableEntities.end(), 
			[](const RenderableEntity& a, const RenderableEntity& b)
			{
				return a.spriteComponent.zIndex < b.spriteComponent.zIndex;
			}
		);




		//loop all entities that the system is interested in
		for (auto entity : renderableEntities)
		{
			const auto transform = entity.transformComponent;
			const auto sprite = entity.spriteComponent;

			/*
			* This was used for testing. We want to render textures now, not rectangles
			* 
			SDL_Rect objRect =
			{
				static_cast<int>(transform.position.x),
				static_cast<int>(transform.position.y),
				sprite.width,
				sprite.height
			};
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderFillRect(renderer, &objRect);
			*/

			//Set the source rectangle of our original sprite texture
			SDL_Rect srcRect = sprite.srcRect;

			//Set the destination rectangle with the x,y psition to be rendered
			SDL_Rect dstRect =
			{
				static_cast<int>(transform.position.x),
				static_cast<int>(transform.position.y),
				static_cast<int>(sprite.width * transform.scale.x),
				static_cast<int>(sprite.height * transform.scale.y)
			};

			/* Draw the PNG texture */
			SDL_RenderCopyEx
			(
				renderer,
				assetStore->GetTexture(sprite.assetId),
				&srcRect,
				&dstRect,
				transform.rotation,
				NULL, //centre of sprite
				SDL_FLIP_NONE //whether to flip or not
			);
			//in addition to the renderer, we need to pass the asset store to void Update()

		}
	}


};