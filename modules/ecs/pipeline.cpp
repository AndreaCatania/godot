
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

void Pipeline::add_system(get_system_info_func func) {
	SystemInfo info = func();
	// TODO is this correct?
	CRASH_COND_MSG(info.system_func == nullptr, "The syntax to add a system is: `add_system(SYSTEM(function_name));`");
	print_line(
			"Added function that has " + itos(info.mutable_components.size()) +
			" mut comp, " + itos(info.immutable_components.size()) + " immutable comp");

	// TODO just a test
	info.system_func(this);
	info.system_func(this);
	info.system_func(this);
	info.system_func(this);
}
