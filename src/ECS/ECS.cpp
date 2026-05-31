#include "ECS.h"
#include <spdlog/spdlog.h>

uint32_t IComponent::nextId = 0;

uint32_t Entity::getId() const {
	return id;
}

void System::addEntityToSystem(Entity entity) {
	entities.push_back(entity);
}

void System::removeEntityFromSystem(Entity entity){
	entities.erase(std::remove_if(entities.begin(), entities.end(), [&entity](Entity other) {
		return entity == other;
	}), entities.end());
}

std::vector<Entity> System::getSystemEntities() const {
	return entities;
}

const Signature& System::getComponentSignature() const{
	return componentSignature;
}


Entity Registry::createEntity(){
	uint32_t entityId;
	entityId = numEntities++;

	Entity entity(entityId);
	entitiesToBeAdded.insert(entity);

	if(entityId >= entityComponentSignatures.size()){
		entityComponentSignatures.resize(entityId + 1);
	}

	spdlog::info("Entity created with id = {}", entityId);
	return entity;
}

void Registry::addEntityToSystems(Entity entity){
	const auto entityId = entity.getId();

	// Match entityComponentSignature <---> systemComponentSignature
	const auto entityComponentSignature = entityComponentSignatures[entityId];

	for(auto& system : systems){
		const auto& systemComponentSignature = system.second->getComponentSignature();

		bool isMatched = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;

		if(isMatched){
			system.second->addEntityToSystem(entity);
		}
	}
}

void Registry::update(){
	// Add the entities that are waiting to be created to the active systems
	for(auto entity: entitiesToBeAdded){
		addEntityToSystems(entity);
	}
	entitiesToBeAdded.clear();

	// Remove the enitites that are waiting to be killed from the active systems
}
