#pragma once

#include "../ecs/ECS.h"
#include "../components/ScriptComponent.h"

//Declare some native C++ functions that we will bind with lua functions
std::tuple<double, double> getEntityPosition(Entity entity) {
    if (entity.hasComponent<TransformComponent>()) {
        const auto transform = entity.getComponent<TransformComponent>();
        return std::make_tuple(transform.position.x, transform.position.y);
    } else {
        spdlog::error("Trying to get the position of an entity that has no transform component");
        return std::make_tuple(0.0, 0.0);
    }
}

std::tuple<double, double> getEntityVelocity(Entity entity) {
    if (entity.hasComponent<RigidbodyComponent>()) {
        const auto rigidbody = entity.getComponent<RigidbodyComponent>();
        return std::make_tuple(rigidbody.velocity.x, rigidbody.velocity.y);
    } else {
        spdlog::error("Trying to get the velocity of an entity that has no rigidbody component");
        return std::make_tuple(0.0, 0.0);
    }
}

void setEntityPosition(Entity entity, double x, double y) {
    if (entity.hasComponent<TransformComponent>()) {
        auto& transform = entity.getComponent<TransformComponent>();
        transform.position.x = x;
        transform.position.y = y;
    } else {
        spdlog::error("Trying to set the position to an entity that has no transform");
    }
}

void setEntityVelocity(Entity entity, double x, double y) {
    if (entity.hasComponent<RigidbodyComponent>()) {
        auto& rigidbody = entity.getComponent<RigidbodyComponent>();
        rigidbody.velocity.x = x;
        rigidbody.velocity.y = y;
    } else {
        spdlog::error("Trying to set the velocity of an entity that has no rigidbody component");
    }
}

void setEntityRotation(Entity entity, double angle) {
    if (entity.hasComponent<TransformComponent>()) {
        auto& transform = entity.getComponent<TransformComponent>();
        transform.rotation = angle;
    } else {
        spdlog::error("Trying to set the rotation of an entity that has no transform component");
    }
}

void setProjectileVelocity(Entity entity, double x, double y) {
    if (entity.hasComponent<ProjectileEmitterComponent>()) {
        auto& projectileEmitter = entity.getComponent<ProjectileEmitterComponent>();
        projectileEmitter.projectileVelocity.x = x;
        projectileEmitter.projectileVelocity.y = y;
    } else {
        spdlog::error("Trying to set the projectile velocity of an entity that has no projectile emitter component");
    }
}

void setAnimation(Entity entity, int frame) {
    if (entity.hasComponent<AnimationComponent>()) {
        auto& animation = entity.getComponent<AnimationComponent>();
        animation.currFrame = frame;
    } else {
        spdlog::error("Trying to set the animation frame of an entity that has no animation component");
    }
}

class ScriptSystem: public System {
public:
    ScriptSystem() {
        requireComponent<ScriptComponent>();
    }

    void createLuaBindings(sol::state& lua) {
        // creating the "entity" usertype so Lua knows what an entity is
        lua.new_usertype<Entity>(
            "entity",
            "get_id", &Entity::getId,
            "destroy", &Entity::kill,
            "has_tag", &Entity::hasTag,
            "belongs_to_group", &Entity::isInGroup
        );

        // Create binding
        lua.set_function("get_position", getEntityPosition);
        lua.set_function("get_velocity", getEntityVelocity);
        lua.set_function("set_position", setEntityPosition);
        lua.set_function("set_velocity", setEntityVelocity);
        lua.set_function("set_rotation", setEntityRotation);
        lua.set_function("set_projectile_velocity", setProjectileVelocity);
        lua.set_function("set_animation", setAnimation);
    }

    void update(double deltaTime, int elapsedTime){
        for (auto entity: getSystemEntities()) {
            const auto script = entity.getComponent<ScriptComponent>();
            script.func(entity, deltaTime, elapsedTime); //here is where we invoke a sol::function
        }
    }
};
