
/** @author Andrea Catania */

#include "pipeline_commands.h"

const EntityBuilder &PipelineCommands::create_entity() {
	return pipeline->create_entity();
}

void PipelineCommands::destroy_entity(EntityID p_entity) {
	pipeline->destroy_entity(p_entity);
}
