
/** @author Andrea Catania */

#include "world_commands.h"

const EntityBuilder &WorldCommands::create_entity() {
	return world->create_entity();
}

void WorldCommands::destroy_entity(EntityID p_entity) {
	world->destroy_entity(p_entity);
}

void WorldCommands::add_component(EntityID p_entity, StringName p_component_name, const Variant &p_data) {
	world->add_component(p_entity, p_component_name, p_data);
}
