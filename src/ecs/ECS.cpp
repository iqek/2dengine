#include "ECS.h"
#include <spdlog/spdlog.h>

ComponentId IComponent::nextId = 0;

EntityId Entity::getId() const {
	return id;
}

void Entity::kill(){
	registry->killEntity(*this);
}

void Entity::tag(const std::string &tag) {
	registry->tagEntity(*this, tag);
}

bool Entity::hasTag(const std::string &tag) const {
	return registry->entityHasTag(*this, tag);
}

void Entity::group(const std::string &group) {
	registry->groupEntity(*this, group);
}

bool Entity::isInGroup(const std::string &group) const {
	return registry->entityIsInGroup(*this, group);
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
	EntityId entityId;

	if(freeIds.empty()){
		// if there are no free ids to be reused
		entityId = numEntities++;
		if(entityId >= entityComponentSignatures.size()){
			entityComponentSignatures.resize(entityId + 1);
		}
	} else {
		entityId = freeIds.front();
		freeIds.pop_front();
	}
	
	Entity entity(entityId);
	entity.registry = this;
	entitiesToBeAdded.insert(entity);

	spdlog::info("Entity created with id = {}", entityId);
	return entity;
}

void Registry::killEntity(Entity entity){
	entitiesToBeKilled.insert(entity);
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

void Registry::removeEntityFromSystems(Entity entity){
	for(auto system: systems){
		system.second->removeEntityFromSystem(entity);
	}
}

void Registry::tagEntity(Entity entity, const std::string &tag) {
	entityPerTag.emplace(tag, entity);
	tagPerEntity.emplace(entity.getId(), tag);
}

bool Registry::entityHasTag(Entity entity, const std::string &tag) const {
	if (tagPerEntity.find(entity.getId()) == tagPerEntity.end()) {
		return false;
	}
	return true;
}

Entity Registry::getEntityByTag(const std::string &tag) const {
	return entityPerTag.at(tag);
}

void Registry::removeEntityTag(Entity entity) {
	auto taggedEntity = tagPerEntity.find(entity.getId());
	if (taggedEntity != tagPerEntity.end()) {
		auto tag = taggedEntity->second;
		entityPerTag.erase(tag);
		tagPerEntity.erase(taggedEntity);
	}
}

void Registry::groupEntity(Entity entity, const std::string &group) {
	entitiesPerGroup.emplace(group, std::set<Entity>());
	entitiesPerGroup[group].emplace(entity);
	groupPerEntity.emplace(entity.getId(), group);
}

bool Registry::entityIsInGroup(Entity entity, const std::string &group) const {
	auto groupEntities = entitiesPerGroup.find(group);
	if (groupEntities == entitiesPerGroup.end()) {
		return false;
	}
	return groupEntities->second.find(entity.getId()) != groupEntities->second.end();
}

std::vector<Entity> Registry::getEntitiesByGroup(const std::string &group) const {
	auto it = entitiesPerGroup.find(group);
	if (it == entitiesPerGroup.end()) {
		return std::vector<Entity>();
	}
	return std::vector<Entity>(it->second.begin(), it->second.end());
}

void Registry::removeEntityGroup(Entity entity) {
	auto groupedEntity = groupPerEntity.find(entity.getId());
	if (groupedEntity != groupPerEntity.end()) {
		auto group = entitiesPerGroup.find(groupedEntity->second);
		if (group != entitiesPerGroup.end()) {
			auto entityInGroup = group->second.find(entity);
			if (entityInGroup != group->second.end()) {
				group->second.erase(entityInGroup);
			}
		}
		groupPerEntity.erase(groupedEntity);
	}
}

void Registry::update(){
	// Processing the entities that are waiting to be created to the active systems
	for(auto entity: entitiesToBeAdded){
		addEntityToSystems(entity);
	}
	entitiesToBeAdded.clear();

	// Process the enitites that are waiting to be killed from the active systems
	for(auto entity: entitiesToBeKilled){
		removeEntityFromSystems(entity);

		// remove the entity's data from every component pool it was part of
		for(auto& pool: componentPools){
			if(pool){
				pool->removeEntityFromPool(entity.getId());
			}
		}

		entityComponentSignatures[entity.getId()].reset();

		// make the entity id available to be reused
		freeIds.push_back(entity.getId());

		// remove any traces of thst entity from the tag/group maps
		removeEntityTag(entity);
		removeEntityGroup(entity);
	}
	entitiesToBeKilled.clear();
}
