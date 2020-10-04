
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

// Unset the macro defined into the `pipeline.h` so to properly point the method
// definition.
#undef add_system
void Pipeline::add_system(get_system_info_func p_get_info_func) {
	const SystemInfo info = p_get_info_func();
	CRASH_COND_MSG(info.system_func == nullptr, "At this point `info.system_func` is supposed to be not null. To add a system use the following syntax: `add_system(function_name);`");

	print_line(
			"Added function that has " + itos(info.mutable_components.size()) +
			" mut comp, " + itos(info.immutable_components.size()) + " immutable comp");

	// TODO compose the pipeline
}
