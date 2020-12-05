
#include "pipeline.h"

#include "modules/ecs/ecs.h"

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

void Pipeline::destroy_entity(EntityID p_entity) {
	// Removes the components assigned to this entity.
	for (uint32_t i = 0; i < storages.size(); i += 1) {
		storages[i]->remove(p_entity);
	}

	// TODO consider to reuse this ID.
}

EntityID Pipeline::get_last_entity_id() const {
	if (entity_count == 0) {
		return EntityID();
	} else {
		return EntityID(entity_count - 1);
	}
}

void Pipeline::add_component(EntityID p_entity, StringName p_component_name, const Variant &p_data) {
	ECS::add_component_by_name(this, p_entity, p_component_name, p_data);
}

void Pipeline::add_native_system(const SystemInfo &p_system_info) {
	CRASH_COND_MSG(p_system_info.system_func == nullptr, "At this point `info.system_func` is supposed to be not null. To add a system use the following syntax: `add_system(function_name);` or use the `ECS` class to get the `SystemInfo` if it's a registered system.");
	//print_line(
	//		"Added function that has " + itos(info.mutable_components.size()) +
	//		" mut comp, " + itos(info.immutable_components.size()) + " immutable comp");

	// TODO compose the pipeline
	systems.push_back(p_system_info.system_func);
}

// Unset the macro defined into the `pipeline.h` so to properly point the method
// definition.
#undef add_system
void Pipeline::add_system(get_system_info_func p_get_info_func) {
	const SystemInfo info = p_get_info_func();
	add_native_system(info);
}

void Pipeline::dispatch() {
	for (uint32_t i = 0; i < systems.size(); i += 1) {
		systems[i](this);
	}
}
