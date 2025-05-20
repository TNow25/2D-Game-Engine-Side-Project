#include "ECS.h"
#include "../Logger/Logger.h"
#include <algorithm>

int IComponent::nextId = 0; //need to define the static member variable that was declared in the header file 


int Entity::GetId() const
{
	return id;
}

void System::AddEntityToSystem(Entity entity)
{
	entities.push_back(entity);
}

void System::RemoveEntityFromSystem(Entity entity)
{	//we are removing FROM "entities.erase" to "entities.end"

	//we are removing the range starting FROM the inumerator
	//returned from remove_if until the end of the vector.
	//We do this because remove_if() doesn't remove anything.
	//It just swaps elements around inside the vector in order
	//to put all elements that don't match the lambda criteria 
	//towards the front of the container.
	//After the remove_ifswapped all the elements around keeping
	//their order, it will then return an iterator to the first
	//element that matched the lambda criteria
	//So, to remove all elements matching the criteria, we need
	//to remove all elements FROM this iterator pointer UNTIL
	//the end of the vector.
	//this is how the "remove-erase" idiom works.
	entities.erase
	(
		std::remove_if
		(
			entities.begin(),
			entities.end(),

			[&entity](Entity other)
			{
				//can use operator overloading here.
				//this is where we can just use the values to compare without
				//worrying about adding, for example, the ".GetId()". Overloading
				//lets us just simply tell what to compare against, without having
				//to tell the compiler each time how to compare, as we do it once
				//in the header file and then it's done for all other comparisons.
				
				/* 1 - before operator overloading*/ 
				//return entity.GetId() == other.GetId(); //lambda function, returns true or false
				/* 2 - after operator overloading*/ 
				return entity == other;
			}
		),

		entities.end()
	);
}

std::vector<Entity> System::GetSystemEntities() const
{
	return entities;
}

const Signature& System::GetComponentSignature() const
{
	return componentSignature;
}


Entity Registry::CreateEntity()
{
	int entityId;

	entityId = numEntities++;

	Entity entity(entityId);
	entity.registry = this;
	entitiesToBeAdded.insert(entity); //here we flag that we have a new entity that's to be added in the next pass of the update before we end the frame
	
	// Make sure the entityComponentSignatures vector can accomodate the new entity
	if (entityId >= entityComponentSignatures.size())
	{
		entityComponentSignatures.resize(entityId + 1);
	}
	
	Logger::Log("Entity created with ID = " + std::to_string(entityId));
	//Here we are trying to add an integer to a string.
	//To fix issues, we need to set the integer to be a string
	//as C++ doesn't like adding an int to a string
	//so instead of " + entityId", we need to do this: "+ std::to_string(entityId)"

	return entity;
}

//this is responsible for getting the entity, comparing the component signature
//of the entity with the systems of the game, and trying to match
//all the systems that require components, if the components the system
// requires match the component of this entity, then add this entity to the system
void Registry::AddEntityToSystems(Entity entity)
{
	const auto entityId = entity.GetId();

	//Match entityComponentSignatures <--> systemComponentSignature
	const auto entityComponentSignature = entityComponentSignatures[entityId];

	//Loop all the systems
	for (auto& system : systems)
	{
		const auto& systemComponentSignature = system.second->GetComponentSignature();

		//true or false for if component signatures in system match component signatures that the entity carries
		bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature; //bitwise & (AND) operator (comparison)

		if (isInterested)
		{
			//Add entity to system
			system.second->AddEntityToSystem(entity);
		}
	}

}



void Registry::Update()
{
	//TODO: Add the entities that are waiting to be created	to the active Systems
	
	for (auto entity : entitiesToBeAdded)
	{
		AddEntityToSystems(entity);
	}
	
	entitiesToBeAdded.clear();
	
	
	
	
	//TODO: Remove the entities that are waiting to be killed from the active Systems



}
