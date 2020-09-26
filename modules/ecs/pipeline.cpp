
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

EntityID Pipeline::get_last_entity_id() const {
	if (entity_count == 0) {
		return EntityID();
	} else {
		return EntityID(entity_count - 1);
	}
}
