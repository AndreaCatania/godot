
#include "pipeline.h"

EntityBuilder::EntityBuilder(Pipeline *p_pipeline) :
		pipeline(p_pipeline) {
}

EntityID Pipeline::create_entity_index() {
	return entity_count++;
}

const EntityBuilder &Pipeline::create_entity() {
	entity_builder.entity = create_entity_index();
	return entity_builder;
}
