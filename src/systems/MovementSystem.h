#pragma once

#include "../ecs/ECS.h"
#include "../components/TransformComponent.h"
#include "../components/RigidbodyComponent.h"

class MovementSystem: public System {
public: 
	MovementSystem(){
		requireComponent<TransformComponent>();
		requireComponent<RigidbodyComponent>();
	}

	void update(double deltaTime){
		for(auto entity: getSystemEntities()){
			auto& transform = entity.getComponent<TransformComponent>();
			const auto rigidbody = entity.getComponent<RigidbodyComponent>();

			transform.position.x += rigidbody.velocity.x * deltaTime;
			transform.position.y += rigidbody.velocity.y * deltaTime;
		}
	}
};
