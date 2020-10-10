
#include "entity.h"

#include "ecs_world.h"
#include "modules/ecs/components/transform_component.h" // TODO remove

void Entity::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_set_components_data"), &Entity::set_components_data);
	ClassDB::bind_method(D_METHOD("_get_components_data"), &Entity::get_components_data);

	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "_component_data", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE), "_set_components_data", "_get_components_data");
}

void Entity::set_components_data(Dictionary p_data) {
	components_data = p_data;
	update_component_data();
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

void Entity::add_component_data(StringName p_component_name) {
	ERR_FAIL_COND_MSG(Engine::get_singleton()->is_editor_hint() == false, "This function can only be called on Editor, probably you need to use `add_component`.");
	ERR_FAIL_COND(components_data.has(p_component_name));
	components_data[p_component_name] = Variant();
}

void Entity::remove_component_data(StringName p_component_name) {
	ERR_FAIL_COND_MSG(Engine::get_singleton()->is_editor_hint() == false, "This function can only be called on Editor, probably you need to use `add_component`.");
	components_data.erase(p_component_name);
}

const Dictionary &Entity::get_components_data() const {
	return components_data;
}

void Entity::update_world() {
	if (ecs_world != nullptr) {
#ifdef DEBUG_ENABLED
		// If the pipeline is not null the entity_id is not null.
		CRASH_COND(entity_id.is_null());
#endif
		// TODO this is not working.
		//ecs_world->get_pipeline().destroy_entity(entity_id);
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

void Entity::update_component_data() {
}