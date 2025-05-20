#pragma once

#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <memory>
#include "../Logger/Logger.h"

const unsigned int MAX_COMPONENTS = 32;

////////////////////////////////////////////////////////////////////////
// Signature
////////////////////////////////////////////////////////////////////////
// We use a bitset (1s and 0s) to keep track of which components an entity has
// and also keep track of which entities a system is interested in
////////////////////////////////////////////////////////////////////////
typedef std::bitset<MAX_COMPONENTS> Signature;


struct IComponent //interface
{
protected:
	static int nextId;
};

//this is used to assign a unique ID to a component type
template <typename T>
class Component: public IComponent
{
public:
	//returns the unique ID of Component<T>
	static int GetId()
	{
		static auto id = nextId++;
		return id;
		//all components, box collider, transform, etc, will have a unique ID
	}
};

//class Registry; //This is to let the compiler know we are going to make a class called Registry, so in class Entity we don't have an error where the compiler doesn't know what Registry is
//This farward declares a class. Tells compiler there is a class
//before we have implemented the class fully. 
//Without this, anything coming before the class implementation
//trying to access the class won't know what to do as it doesn't exist

class Entity
{
private:
	int id;

public:
	Entity(int id) : id(id) {}; 
	// "id(id) makes sure parametre id is passed and initialises the member variable id of the class
	int GetId() const;

	Entity(const Entity& entity) = default;
	 

	//don't overuse operator overloading, can get messy 
	//and difficult to read for those who don't know the
	//full extent of the overloading used
	Entity& operator =(const Entity& other) = default;
	bool operator ==(const Entity& other) const { return id == other.id; }
	bool operator !=(const Entity& other) const { return id != other.id; }
	bool operator <(const Entity& other) const { return id < other.id; }
	bool operator >(const Entity& other) const { return id > other.id; }

	template <typename TComponent, typename ...TArgs> void AddComponent(TArgs&& ...args);
	template <typename TComponent> void RemoveComponent();
	template <typename TComponent> bool HasComponent() const;
	template <typename TComponent> TComponent& GetComponent() const;

	//Hold a pointer to the entity's owner registry
	class Registry* registry; //see line 42 for description
};


////////////////////////////////////////////////////////////////////////
// System
////////////////////////////////////////////////////////////////////////
// The system processes entities that contain a specific signature
////////////////////////////////////////////////////////////////////////

class System
{
private:
	Signature componentSignature;
	std::vector<Entity> entities;

public:
	System() = default;
	~System() = default;

	void AddEntityToSystem(Entity entity);
	void RemoveEntityFromSystem(Entity entity);
	std::vector<Entity> GetSystemEntities() const;
	const Signature& GetComponentSignature() const;

	//Defines the component type that enities must have to be considered by the system
	template <typename TComponent> void RequireComponent();
	//TComponent = type of component
};


////////////////////////////////////////////////////////////////////////
// Pool
////////////////////////////////////////////////////////////////////////
// A pool is just a vector (contiguous data) of objects of type T
/////////////////////////////////////////////////////////////////////
class IPool //this class is only abstract
{
public:
	virtual ~IPool() {}
}; //forcing the destructor IPool to be virtual,
   //you're forcing the class to be only abstract


template <typename T>
class Pool: public IPool //template class
{ //inherit from IPool
private: 
	std::vector<T> data;

public: 
	Pool(int size = 100)
	{
		data.resize(size); //resize is from <vector> which is included
	}
	
	virtual ~Pool() = default;

	bool isEmpty() const
	{
		return data.empty();
	}

	int GetSize() const
	{
		return data.size();
	}

	void Resize(int n)
	{
		data.resize(n);
	}

	void Clear()
	{
		data.clear();
	}

	void Add(T object)
	{
		data.push_back(object);
	}

	void Set(int index, T object)
	{
		data[index] = object;
	}

	T& Get(int index)
	{
		return static_cast<T&>(data[index]);
	}

	T& operator [](unsigned int index)
	{
		return data[index];
	}

};


////////////////////////////////////////////////////////////////////////
// Registry
////////////////////////////////////////////////////////////////////////
// the registry manages the creation and destruction of entities,
// add systems, and components.
/////////////////////////////////////////////////////////////////////
class Registry
{
private:
	int numEntities = 0;

	//Vector of component pools, each pool contains all the 
	//data for a certain component type
	//[Vector index = component type ID]
	//[Pool index = entity ID]
	/*std::vector<IPool*> componentPools; */ //old, we now use smart pointers
	std::vector<std::shared_ptr<IPool>> componentPools;
	//We say IPool instead of Pool because we don't 
	//know the type, so if we use IPool as the parent class,
	//we don't need to specify the type each time.

	//Vector of component signatures per entity, saying which
	//component is turned "on" for a given entity
	//[Vector index = entity id]
	std::vector<Signature> entityComponentSignatures;

	//unordered_map is a map but where things don't need to be sorted or ordered
	//data structure in memory that works with keys and values
	// Map of active systems
	// [Map key = system type id]
	//std::unordered_map<std::type_index, System*> systems;
	std::unordered_map<std::type_index, std::shared_ptr<System>> systems; //smart pointer implementation

	//Set of entities that are flagged to be added or removed
	//in the next registry Update()
	std::set<Entity> entitiesToBeAdded;
	std::set<Entity> entitiesToBeKilled;

public: 
	//Registry() = default; //Replaced for use of smart pointers
	Registry() 
	{ 
		Logger::Log("Registry constructor called"); 
	}

	~Registry()
	{
		Logger::Log("Registry destructor called");
	}

	// The registry Update() finally processes the entities that are waiting to be added/killed
	void Update();

	//Entity management
	Entity CreateEntity();	


	///// Component management /////
	template <typename TComponent, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
	// when we use "...TArgs", this just means that we have many 
	// arguments to add, but we don't know how many. Could be 1 or 5
	// this allows us to add as many as we want without specifying 
	// how many we want when we initialise it

	//more component management below (the two templates)

	// Ask to RemoveComponent<T> from an entity
	template <typename TComponent> void RemoveComponent(Entity entity);

	// Checks if an entity HasComponent<T>()
	template <typename TComponent> bool HasComponent(Entity entity) const;

	template <typename TComponent> TComponent& GetComponent(Entity entity) const;


	///// Systems Management /////
	//Would be benefitial if we made templates, so we can make multiple systems (damageSystem, MovementSystem, CollisionSystem) from a single template
	template <typename TSystem, typename ...TArgs> void AddSystem(TArgs&& ...args);
	template <typename TSystem> void RemoveSystem();
	template <typename TSystem> bool HasSystem() const;
	template <typename TSystem> TSystem& GetSystem() const;


	// CHecks the component signature of an entity and add the 
	// entity to the systems that are interested in it
	void AddEntityToSystems(Entity entity);

};
//std::unordered_map<std::typeIndex, System*

template <typename TComponent>
void System::RequireComponent()
{
	const auto componentId = Component<TComponent>::GetId();
	componentSignature.set(componentId);
}


template <typename TSystem, typename ...TArgs> 
void Registry::AddSystem(TArgs&& ...args)
{
	//add new system object to map of systems in registry
//Old implementation without smart pointers 	TSystem* newSystem(new TSystem(std::forward<TArgs>(args)...)); //new object of type newSystem
	std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...); //new object of type newSystem
	//add new object (newSystem) to unordered map. systems is name of unordered map. Key and Value pair needed
	systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem)); //(key,value).		Key is of type "type_index" (line 191 (subject to change))
	
}

template <typename TSystem>
void Registry::RemoveSystem()
{
	auto system = systems.find(std::type_index(typeid(TSystem)));
	systems.erase(system);
}


template <typename TSystem>
bool Registry::HasSystem() const
{
	return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template <typename TSystem>
TSystem& Registry::GetSystem() const
{ //
	auto system = systems.find(std::type_index(typeid(TSystem))); //try to find the system. Find the key and returns an iterator pointer
	// first is key, second is value
	return *(std::static_pointer_cast<TSystem>(system->second)); //use pointer to ask for the second (value) and convert to static pointer
}



template <typename TComponent, typename ...TArgs>
void Registry::AddComponent(Entity entity, TArgs&& ...args)
{
	const auto componentId = Component<TComponent>::GetId(); //get componentId
	const auto entityId = entity.GetId(); //get entityId  

	if (componentId >= componentPools.size()) //if new component type
	{ //if new component type, resize the pool
		componentPools.resize(componentId + 1, nullptr);
	}

	if (!componentPools[componentId]) //if nothing in new componentPool position for this component type id
	{ //if at componentId index component position it's a nullptr
		//create new pool of type T
		//Pool<TComponent>* newComponentPool = new Pool<TComponent>(); //create new pool
		std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>();
		componentPools[componentId] = newComponentPool; //assign new pool for that position of componentPools
	}

	//fetch position from componentpools vector
	//Pool<TComponent>* componentPool = componentPools[componentId];
	std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

	//resize pool of components to accommodate new entity
	if (entityId >= componentPool->GetSize()) //if pool too small to accommodate new entity
	{
		componentPool->Resize(numEntities); //resize the pool 
	}
	//create new compoennt of type TComponent
	TComponent newComponent(std::forward<TArgs>(args)...); //this constructs the component of type TComponent

	componentPool->Set(entityId, newComponent); //this is value i want to set in pool position
	entityComponentSignatures[entityId].set(componentId); //enable component in signature (turns on in the bitset)


	Logger::Log("Component id = " + std::to_string(componentId) + " was added to entity id " + std::to_string(entityId));
}

/*
* My Attempt at implementing HasComponent();
* Proper implementation below my attempt
*
* void Registry::HasComponent(Entity entity)
* {
*	const auto componentId = Component<TComponent>::GetId();
*	const auto entityId = entity.GetId();
*	const bool hasComponent;
*
*	if(componentId != entity.GetId();
*	{
*		return hasComponent = false; 
*	}
*
*	return hasComponent = true;
* }
*/
////////////////////////////////////////////////////////////////////////
/* 
* Proper implementation: (Implemented on Line 399 (subject to change))
* 
* template <typename T>
* bool Registry::HasComponent(Entity entity)
* {
*	const auto componentId = Component<T>::GetId();
*	const auto entityId = entity.GetId();
* 
*	return entityComponentSignatures[entityId].test(componentId);
* }
* 
*/


//no function here (yet!)
//placeholder for several functions
//compiler will come to this template placeholder, and replace 
//this with the actual fuctions
//one for removeTransformComponentType
//another for removeSpriteComponentType
//		So, templates are not functions, but placeholders for the 
//		compiler to use to make the functions for each component
template <typename TComponent>
void Registry::RemoveComponent(Entity entity)
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();
	entityComponentSignatures[entityId].set(componentId, false);

	Logger::Log("Component id = " + std::to_string(componentId) + " was removed from entity id " + std::to_string(entityId));
}

template <typename TComponent>
bool Registry::HasComponent(Entity entity) const
{
	const auto componentId = Component<TComponent>::Getid();
	const auto entityId = entity.GetId();
	return entityComponentSignatures[entityId].test(componentId);
}

template <typename TComponent> TComponent& Registry::GetComponent(Entity entity) const
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();
	auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
	//fetching from comoponent pools at componentid vector index, gthen casting to static pointer cast to get componentPool object
	//return componentPool
	return componentPool->Get(entityId);
}


template <typename TComponent, typename ...TArgs>
void Entity::AddComponent(TArgs&& ...args)
{
	/* My Implementation
	* 
	* I'll be honest, no idea in how to go about doing this myself
	* 
	* 
	* 
	*/

	//Here we ask an entity to add a component to itself by asking 
	//the registry to help with adding the component
	registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);


}

template <typename TComponent>
void Entity::RemoveComponent()
{
	/*
	* Now that I've seen how to do it in the AddComponent() 
	* I feel like I can do it myself here. 
	* I will add my own implementation here, commented out,
	* but will say if I was incorrect or correct, and will add
	* the proper implementation below this section
	* 
	* 
	* registry->RemoveComponent<TComponent>(*this);
	* 
	* 
	* My attempt was correct. Woo!
	*/

	registry->RemoveComponent<TComponent>(*this);

}

template <typename TComponent>
bool Entity::HasComponent() const
{
	return registry->HasComponent<TComponent>(*this); //my attempt for this was also correct. Woo!
}

template <typename TComponent>
TComponent& Entity::GetComponent() const
{
	return registry->GetComponent<TComponent>(*this); //my attempt was also correct for this
}