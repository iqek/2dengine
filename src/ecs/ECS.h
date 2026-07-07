#pragma once

#include <spdlog/spdlog.h>
#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <memory>
#include <cstdint>

constexpr std::size_t MAX_COMPONENTS = 32;
// a bitset to keep track of which components an entity has.
// also helps keep track of which entities a system is interested in
using Signature = std::bitset<MAX_COMPONENTS>;

struct IComponent {
protected:
	static uint32_t nextId;
};

// Used to assign a unique id to a component type
template <typename T>
class Component: public IComponent {
public:
	static uint32_t getId() {
		static auto id = nextId++;
		return id;
	}
};

class Entity {
private:
	uint32_t id;

public:
	Entity(uint32_t id): id(id) {};
	Entity(const Entity& entity) = default;
	uint32_t getId() const;

	Entity& operator =(const Entity& other) = default;
	bool operator ==(const Entity& other) const { return id == other.id; }
	bool operator !=(const Entity& other) const { return id != other.id; }
	bool operator <(const Entity& other) const { return id < other.id; }
	bool operator >(const Entity& other) const { return id > other.id; }

	template<typename TComponent, typename ...TArgs> void addComponent(TArgs&& ...args);
	template<typename TComponent> void removeComponent();
	template<typename TComponent> bool hasComponent() const;
	template<typename TComponent> TComponent& getComponent() const;
	class Registry* registry;
};


// The system processes entities that contain a specific signiture
class System {
private:
	Signature componentSignature;
	std::vector<Entity> entities;

public:
	System() = default;
	~System() = default;

	void addEntityToSystem(Entity entity);
	void removeEntityFromSystem(Entity entity);
	std::vector<Entity> getSystemEntities() const;
	const Signature& getComponentSignature() const;

	// Defines the component type that entities must have to be considered by the system
	template <typename TComponent> void requireComponent();
};


class IPool {
public:
	virtual ~IPool() {}
};

// a pool is just a vector (contiguous data) of objects of type T
template <typename T>
class Pool : public IPool {
private:
	std::vector<T> data;

public:
	Pool(std::size_t size = 100){
		data.resize(size);
	}

	virtual ~Pool() = default;

	bool isEmpty() const{
		return data.empty();
	}

	std::size_t getSize() const{
		return data.size();
	}

	void resize(std::size_t n){
		data.resize(n);
	}

	void clear(){
		data.clear();
	}

	void add(T obj){
		data.push_back(obj);
	}

	void set(std::size_t index, T obj){
		data[index] = obj;
	}

	T& get(std::size_t index){
		return data[index];
	}

	T& operator [](std::size_t index) {
		return data[index];
	}
};


// The registry manages the creation and destruction of entities, add systems and components
class Registry {
private:
	uint32_t numEntities = 0;

	// vector of component pools, each pool contains the data for a certian component type
	// vector index is the component type id
	// [pool index = entity id]
	std::vector<std::shared_ptr<IPool>> componentPools;

	// Vector of component signatures per entity, saying which component in turned on for each entity
	// [vector index = entity id]
	std::vector<Signature> entityComponentSignatures;

	// map of active systems
	// index = systems type id
	std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

	// set of entities that are flagged to be added or removed in the next registry update
	std::set<Entity> entitiesToBeAdded;
	std::set<Entity> enitiesToBeKilled;

public:
	Registry() = default;

	void update();

	Entity createEntity();
	void addEntityToSystem(Entity entity);
	Entity killEntity();

	// Component management
	template<typename TComponent, typename ...TArgs> void addComponent(Entity entity, TArgs&& ...args);
	template<typename TComponent> void removeComponent(Entity entity);
	template<typename TComponent> bool hasComponent(Entity entity) const;
	template<typename TComponent> TComponent& getComponent(Entity entity) const;

	// System management
	template<typename TSystem, typename ...TArgs> void addSystem(TArgs&& ...args);
	template<typename TSystem> void removeSystem();
	template<typename TSystem> bool hasSystem() const;
	template<typename TSystem> TSystem& getSystem() const;

	// check the component signature of an entity and add to the systems
	void addEntityToSystems(Entity entity);
};



template <typename TComponent>
void System::requireComponent() {
	const auto componentId = Component<TComponent>::getId();
	componentSignature.set(componentId);
}



template<typename TSystem, typename ...TArgs>
void Registry::addSystem(TArgs&& ...args){
	std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
	systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}


template<typename TSystem>
void Registry::removeSystem(){
	auto system = systems.find(std::type_index(typeid(TSystem)));
	systems.erase(system);
}


template<typename TSystem>
bool Registry::hasSystem() const{
	return systems.find(typeid(TSystem)) != systems.end();
}


template<typename TSystem>
TSystem& Registry::getSystem() const{
	auto system = systems.find(std::type_index(typeid(TSystem)));
	return *(std::static_pointer_cast<TSystem>(system->second));
}



template<typename TComponent, typename ...TArgs>
void Registry::addComponent(Entity entity, TArgs&&... args){
	const auto componentId = Component<TComponent>::getId();
	const auto entityId = entity.getId();

	if(componentId >= componentPools.size()){
		componentPools.resize(componentId + 1, nullptr);
	}

	if(!componentPools[componentId]){
		componentPools[componentId] = std::make_shared<Pool<TComponent>>();
	}

	// Get the pool of component values for that component type
	std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

	if(entityId >= componentPool->getSize()){
		componentPool->resize(numEntities);
	}

	//Create a new component object of type T, and forward the various parameters to the constructor
	TComponent newComponent(std::forward<TArgs>(args)...);
	//Add the new component to the component pool and change the component signature of the entity
	componentPool->set(entityId, newComponent);
	entityComponentSignatures[entityId].set(componentId);

	spdlog::info("ComponentId = {} was added to EntityId {}",componentId, entityId);
}


template<typename TComponent>
void Registry::removeComponent(Entity entity){
	const auto componentId = Component<TComponent>::getId();
	const auto entityId = entity.getId();
	entityComponentSignatures[entityId].set(componentId, false);

	spdlog::info("ComponentId = {} was removed from EntityId {}", componentId, entityId);
}


template<typename TComponent>
bool Registry::hasComponent(Entity entity) const{
	const auto componentId = Component<TComponent>::getId();
	const auto entityId = entity.getId();
	return entityComponentSignatures[entityId].test(componentId);
}

template<typename TComponent>
TComponent& Registry::getComponent(Entity entity) const{
	const auto componentId = Component<TComponent>::getId() ;
	const auto entityId = entity.getId();
	auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
	return componentPool->get(entityId);
}


template<typename TComponent, typename ...TArgs>
void Entity::addComponent(TArgs&& ...args){
	registry -> addComponent<TComponent>(*this, std::forward<TArgs>(args)...); 
}

template<typename TComponent>
void Entity::removeComponent(){
	registry->removeComponent<TComponent>(*this);
}

template<typename TComponent>
bool Entity::hasComponent() const {
	return registry->hasComponent<TComponent>(*this);
}

template<typename TComponent>
TComponent& Entity::getComponent() const {
	return registry->getComponent<TComponent>(*this);
}