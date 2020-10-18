
/** @author Andrea Catania */

#include "pipeline_commands.h"

const EntityBuilder &PipelineCommands::create_entity() {
	return pipeline->create_entity();
}

void PipelineCommands::destroy_entity(EntityID p_entity) {
	pipeline->destroy_entity(p_entity);
}

void PipelineCommands::add_component(EntityID p_entity, StringName p_component_name, const Variant &p_data) {
	pipeline->add_component(p_entity, p_component_name, p_data);
}
