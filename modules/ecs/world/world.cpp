
#include "world.h"

#include "modules/ecs/ecs.h"
#include "modules/ecs/pipeline/pipeline.h"

EntityBuilder::EntityBuilder(World *p_world) :
		world(p_world) {
}

EntityID World::create_entity_index() {
	return entity_count++;
}

const EntityBuilder &World::create_entity() {
	entity_builder.entity = create_entity_index();
	return entity_builder;
}

void World::destroy_entity(EntityID p_entity) {
	// Removes the components assigned to this entity.
	for (uint32_t i = 0; i < storages.size(); i += 1) {
		storages[i]->remove(p_entity);
	}

	// TODO consider to reuse this ID.
}

EntityID World::get_last_entity_id() const {
	if (entity_count == 0) {
		return EntityID();
	} else {
		return EntityID(entity_count - 1);
	}
}

void World::add_component(EntityID p_entity, StringName p_component_name, const Variant &p_data) {
	ECS::add_component_by_name(this, p_entity, p_component_name, p_data);
}

const Storage *World::get_storage_by_id(uint32_t p_storage_id) const {
	ERR_FAIL_COND_V_MSG(p_storage_id == UINT32_MAX, nullptr, "The component is not registered.");

	if (p_storage_id >= storages.size() || storages[p_storage_id] == nullptr) {
		return nullptr;
	}

	return storages[p_storage_id];
}

Storage *World::get_storage_by_id(uint32_t p_storage_id) {
	ERR_FAIL_COND_V_MSG(p_storage_id == UINT32_MAX, nullptr, "The component is not registered.");

	if (p_storage_id >= storages.size() || storages[p_storage_id] == nullptr) {
		return nullptr;
	}

	return storages[p_storage_id];
}

void World::set_pipeline(Pipeline *p_pipeline) {
	pipeline = p_pipeline;
}

Pipeline *World::get_pipeline() const {
	return pipeline;
}

void World::dispatch() {
	if (unlikely(pipeline != nullptr)) {
		// No world, nothing to do.
		return;
	}
	pipeline->dispatch(this);
}
