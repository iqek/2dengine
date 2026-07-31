#pragma once

#include <spdlog/spdlog.h>
#include <vector>
#include <typeindex>
#include <bitset>
#include <unordered_map>
#include <set>
#include <deque>
#include <memory>
#include <cstdint>

constexpr std::size_t MAX_COMPONENTS = 32;
// a bitset to keep track of which components an entity has.
// also helps keep track of which entities a system is interested in
using Signature = std::bitset<MAX_COMPONENTS>;

using EntityId = std::uint32_t;
using ComponentId = std::uint32_t;

struct IComponent {
protected:
	static ComponentId nextId;
};

// Used to assign a unique id to a component type
template <typename T>
class Component: public IComponent {
public:
	static ComponentId getId() {
		static auto id = nextId++;
		return id;
	}
};

class Entity {
private:
	EntityId id;

public:
	Entity(EntityId id): id(id) {};
	Entity(const Entity& entity) = default;
	void kill();
	EntityId getId() const;

	void tag(const std::string& tag);
	bool hasTag(const std::string& tag) const;
	void group(const std::string& group);
	bool isInGroup(const std::string& group) const;

	Entity& operator =(const Entity& other) = default;
	bool operator ==(const Entity& other) const { return id == other.id; }
	bool operator !=(const Entity& other) const { return id != other.id; }
	bool operator <(const Entity& other) const { return id < other.id; }
	bool operator >(const Entity& other) const { return id > other.id; }

	// manage entity components
	template<typename TComponent, typename ...TArgs> void addComponent(TArgs&& ...args);
	template<typename TComponent> void removeComponent();
	template<typename TComponent> bool hasComponent() const;
	template<typename TComponent> TComponent& getComponent() const;

	// hold a pointer to the entity's owner registry
	class Registry* registry;
};


// The system processes entities that contain a specific signature
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
	virtual ~IPool() = default;
	virtual void removeEntityFromPool(EntityId entityId) = 0;
};

// A pool is a packed (contiguous) vector of components of type T.
// It uses a sparse-set: entityIdToIndex/indexToEntityId map entity ids to
// dense slots in `data`, so iteration stays cache-friendly even though
// entity ids can be sparse or reused.
template <typename T>
class Pool : public IPool {
private:
	std::vector<T> data;
	std::unordered_map<EntityId, std::size_t> entityIdToIndex;
	std::unordered_map<std::size_t, EntityId> indexToEntityId;

public:
	explicit Pool(std::size_t capacity = 100){
		data.reserve(capacity);
	}

	virtual ~Pool() = default;

	bool isEmpty() const{
		return data.empty();
	}

	std::size_t getSize() const{
		return data.size();
	}

	void clear(){
		data.clear();
		entityIdToIndex.clear();
		indexToEntityId.clear();
	}

	bool has(EntityId entityId) const{
		return entityIdToIndex.find(entityId) != entityIdToIndex.end();
	}

	void set(EntityId entityId, T obj){
		auto it = entityIdToIndex.find(entityId);
		if(it != entityIdToIndex.end()){
			data[it->second] = std::move(obj);
			return;
		}

		const std::size_t newIndex = data.size();
		entityIdToIndex[entityId] = newIndex;
		indexToEntityId[newIndex] = entityId;
		data.push_back(std::move(obj));
	}

	void remove(EntityId entityId){
		auto it = entityIdToIndex.find(entityId);
		if(it == entityIdToIndex.end()){
			return;
		}

		// Swap the removed element with the last one to keep data packed,
		// then fix up the index maps for the entity that got moved.
		const std::size_t indexOfRemoved = it->second;
		const std::size_t indexOfLast = data.size() - 1;
		data[indexOfRemoved] = std::move(data[indexOfLast]);

		const EntityId lastEntityId = indexToEntityId[indexOfLast];
		entityIdToIndex[lastEntityId] = indexOfRemoved;
		indexToEntityId[indexOfRemoved] = lastEntityId;

		entityIdToIndex.erase(entityId);
		indexToEntityId.erase(indexOfLast);
		data.pop_back();
	}

	void removeEntityFromPool(EntityId entityId) override{
		remove(entityId);
	}

	T& get(EntityId entityId){
		return data[entityIdToIndex.at(entityId)];
	}

	T& operator [](std::size_t index) {
		return data[index];
	}
};


// The registry manages the creation and destruction of entities, add systems and components
class Registry {
private:
	EntityId numEntities = 0;

	// Vector of component pools, each pool contains the data for a certian component type
	// vector index is the component type id
	std::vector<std::shared_ptr<IPool>> componentPools;

	// Vector of component signatures per entity, saying which component in turned on for each entity
	// [vector index = entity id]
	std::vector<Signature> entityComponentSignatures;

	// map of active systems
	// index = systems type id
	std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

	// set of entities that are flagged to be added or removed in the next registry update
	std::set<Entity> entitiesToBeAdded;
	std::set<Entity> entitiesToBeKilled;

	void addEntityToSystems(Entity entity);
	void removeEntityFromSystems(Entity entity);

	// Entity tags
	std::unordered_map<std::string, Entity> entityPerTag;
	std::unordered_map<EntityId, std::string> tagPerEntity;

	// Entity groups
	std::unordered_map<std::string, std::set<Entity>> entitiesPerGroup;
	std::unordered_map<EntityId, std::string> groupPerEntity;

	// list of free entity ids that were previously removed
	std::deque<EntityId> freeIds;

public:
	Registry(){
		spdlog::info("Registry constructor called");
	}

	~Registry(){
		spdlog::info("Registry destructor called");
	}

	void update();

	Entity createEntity();
	void killEntity(Entity entity);

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

	void tagEntity(Entity entity, const std::string& tag);
	bool entityHasTag(Entity entity, const std::string& tag) const;
	Entity getEntityByTag(const std::string& tag) const;
	void removeEntityTag(Entity entity);

	void groupEntity(Entity entity, const std::string& group);
	bool entityIsInGroup(Entity entity, const std::string& group) const;
	std::vector<Entity> getEntitiesByGroup(const std::string& group) const;
	void removeEntityGroup(Entity entity);
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

	//Create a new component object of type T, and forward the various parameters to the constructor
	TComponent newComponent(std::forward<TArgs>(args)...);
	//Add the new component to the component pool and change the component signature of the entity
	componentPool->set(entityId, std::move(newComponent));
	entityComponentSignatures[entityId].set(componentId);

	spdlog::info("ComponentId = {} was added to EntityId {}",componentId, entityId);
}


template<typename TComponent>
void Registry::removeComponent(Entity entity){
	const auto componentId = Component<TComponent>::getId();
	const auto entityId = entity.getId();

	std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
	componentPool->remove(entityId);
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