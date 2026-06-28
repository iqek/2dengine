#pragma once

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidbodyComponent.h"

class MovementSystem: public System {
	public: 
		MovementSystem(){
			requireComponent<TransformComponent>();
			requireComponent<RigidbodyComponent>();
		}

		void update(double deltaTime){
			// Loop all entities that the system is interested in
			for(auto entity: getSystemEntities()){
				// update entity position based on its velocity
				auto& transform = entity.getComponent<TransformComponent>();
				const auto rigidbody = entity.getComponent<RigidbodyComponent>();

				transform.position.x += rigidbody.velocity.x * deltaTime;
				transform.position.y += rigidbody.velocity.y * deltaTime;
			}
		}
};
