
#include "entity.h"

#include "ecs_world.h"
#include "modules/ecs/components/transform_component.h" // TODO remove

void Entity::_bind_methods() {
}

Entity::Entity() :
		Node() {}

Entity::~Entity() {
}

void Entity::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_PARENTED:
			update_world();
			break;
		case NOTIFICATION_UNPARENTED:
			update_world();
			break;
	}
}

void Entity::add_component(StringName p_component_name) {
	// TODO
}

void Entity::update_world() {
	if (ecs_world != nullptr) {
#ifdef DEBUG_ENABLED
		// If the pipeline is not null the entity_id is not null.
		CRASH_COND(entity_id.is_null());
#endif
		ecs_world->get_pipeline().destroy_entity(entity_id);
		entity_id = EntityID();
	}

	ecs_world = nullptr;

	// Search the ECSWorld.
	for (Node *p = get_parent(); p != nullptr; p = p->get_parent()) {
		if (Object::cast_to<ECSWorld>(p) != nullptr) {
			ecs_world = static_cast<ECSWorld *>(p);
			break;
		}
	}

	if (ecs_world == nullptr) {
		// TODO fallback to the main scene one.
		// ecs_world=ECS::get_singleton()->main_world();
	}

	if (ecs_world != nullptr) { // TODO consider remove this `IF` when the main world is added
		entity_id = ecs_world->get_pipeline().create_entity();

		// TODO add components.

		// TODO this is just a test to measure performances
		ecs_world->get_pipeline().add_component(entity_id, TransformComponent());
	}
}