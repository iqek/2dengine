#pragma once

#include "../ecs/ECS.h"
#include "../event_manager/EventBus.h"
#include "../events/CollisionEvent.h"
#include "../components/TransformComponent.h"
#include "../components/RigidbodyComponent.h"
#include "../components/SpriteComponent.h"

class MovementSystem: public System {
public: 
	MovementSystem(){
		requireComponent<TransformComponent>();
		requireComponent<RigidbodyComponent>();
	}

	void listenToEvents(EventBus* eventBus) {
		eventBus->listenToEvent(this, &MovementSystem::onCollision);
	}

	void onCollision(CollisionEvent& event) {
		Entity a = event.a;
		Entity b = event.b;

		if (a.isInGroup("enemies") && b.isInGroup("obstacles")) {
			onEnemyHitsObstacle(a, b);
		}

		if (b.isInGroup("enemies") && a.isInGroup("obstacles")) {
			onEnemyHitsObstacle(b, a);
		}
	}

	void onEnemyHitsObstacle(Entity enemy, Entity obstacle) {
		if (enemy.hasComponent<RigidbodyComponent>()) {
			auto& rigidbody = enemy.getComponent<RigidbodyComponent>();
			auto& sprite = enemy.getComponent<SpriteComponent>();

			if (rigidbody.velocity.x != 0) {
				rigidbody.velocity.x *= -1;
				sprite.flip = (sprite.flip == SDL_FLIP_NONE) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
			}

			if (rigidbody.velocity.y != 0) {
				rigidbody.velocity.y *= -1;
			}
		}
	}

	void update(double deltaTime){
		for(auto entity: getSystemEntities()){
			auto& transform = entity.getComponent<TransformComponent>();
			const auto rigidbody = entity.getComponent<RigidbodyComponent>();

			transform.position.x += rigidbody.velocity.x * deltaTime;
			transform.position.y += rigidbody.velocity.y * deltaTime;

			if (entity.hasTag("player")) {
				int paddingLeft = 10;
				int paddingTop = 10;
				int paddingRight = 10;
				int paddingBottom = 10;
				transform.position.x = transform.position.x < paddingLeft ? paddingLeft : transform.position.x;
				transform.position.x = transform.position.x > Game::mapWidth - paddingRight ? Game::mapWidth - paddingRight : transform.position.x;
				transform.position.y = transform.position.y < paddingTop ? paddingTop : transform.position.y;
				transform.position.y = transform.position.y > Game::mapHeight - paddingBottom ? paddingBottom : transform.position.y;
			}

			bool isOutsideMap = (
				transform.position.x < 0 ||
				transform.position.x > Game::mapWidth ||
				transform.position.y < 0 ||
				transform.position.y > Game::mapHeight
			);
			if (isOutsideMap && !entity.hasTag("player")) {
				entity.kill();
			}
		}
	}
};
