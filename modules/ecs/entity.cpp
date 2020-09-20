
#include "entity.h"

void Entity::_bind_methods() {
}

Entity::Entity() :
		index(ECS::get_singleton()->create_new_entity_id()) {
}

Entity::~Entity() {
}

void Entity::add_component(StringName p_component_name) {
	// TODO
}