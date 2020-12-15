#include "dynamic_query.h"

#include "modules/ecs/ecs.h"

using godex::AccessComponent;
using godex::DynamicQuery;

AccessComponent::AccessComponent(bool p_mut) :
		mut(p_mut) {}

void AccessComponent::set(StringName p_name, Variant p_data) const {
	ERR_FAIL_COND_MSG(mut == false, "This component was taken as not mutable.");
	component->set(p_name, p_data);
}

Variant AccessComponent::get(StringName p_name) const {
	return component->get(p_name);
}

bool AccessComponent::is_mutable() const {
	return mut;
}

DynamicQuery::DynamicQuery() {
}

void DynamicQuery::add_component(StringName p_component, bool p_mutable) {
	ERR_FAIL_COND_MSG(is_valid() == false, "This component is not valid.");
	ERR_FAIL_COND_MSG(can_change == false, "This query can't change at this point, you have to `clear` it.");

	const uint32_t id = ECS::get_component_id(p_component);
	if (id == UINT32_MAX) {
		// Invalidate.
		valid = false;
	}
	storage_ids.push_back(id);
	access_component.push_back(p_mutable);
}

bool DynamicQuery::is_valid() const {
	return valid;
}

void DynamicQuery::reset() {
	valid = true;
	can_change = true;
	storage_ids.clear();
	access_component.clear();
	world = nullptr;
}

void DynamicQuery::begin(World *p_world) {
	// Can't change anymore.
	can_change = false;
	entity_id = UINT32_MAX;

	ERR_FAIL_COND(is_valid() == false);

	// Using a crash cond so the dev knows immediately if it's using the query
	// in the wrong way.
	// The user doesn't never use this query directly. So it's fine put the
	// crash cond here.
	CRASH_COND_MSG(world != nullptr, "Make sure to call `DynamicQuery::end()` when you finish using the query!");
	world = p_world;

	storages.resize(storage_ids.size());
	for (uint32_t i = 0; i < storage_ids.size(); i += 1) {
		storages[i] = world->get_storage(storage_ids[i]);
		ERR_FAIL_COND_MSG(storages[i] == nullptr, "There is a storage nullptr. This is not supposed to happen.");
	}

	// At this point all the storages are taken

	// Search the fist entity
	entity_id = 0;
	if (has_entity(0) == false) {
		next_entity();
	}
}

bool DynamicQuery::is_done() const {
	return entity_id == UINT32_MAX;
}

const LocalVector<AccessComponent> *DynamicQuery::get() {
	ERR_FAIL_COND_V_MSG(entity_id == UINT32_MAX, nullptr, "There is nothing to fetch.");

	for (uint32_t i = 0; i < storages.size(); i += 1) {
		access_component[i].component = storages[i]->get_ptr(entity_id);
	}

	return &access_component;
}

EntityID DynamicQuery::get_current_entity_id() const {
	return entity_id;
}

// TODO see how to improve this lookup mechanism so that no cache is miss and
// it's fast.
void DynamicQuery::next_entity() {
	const uint32_t last_id = world->get_last_entity_id();
	if (unlikely(entity_id == UINT32_MAX || last_id == UINT32_MAX)) {
		entity_id = UINT32_MAX;
		return;
	}

	for (uint32_t new_entity_id = entity_id + 1; new_entity_id <= last_id; new_entity_id += 1) {
		if (has_entity(new_entity_id)) {
			// Confirmed, this `new_entity_id` has all the storages.
			entity_id = new_entity_id;
			return;
		}
	}

	// No more entity
	entity_id = UINT32_MAX;
}

void DynamicQuery::end() {
	world = nullptr;
	storages.clear();
}

bool DynamicQuery::has_entity(EntityID p_id) const {
	for (uint32_t i = 0; i < storages.size(); i += 1) {
		if (storages[i]->has(p_id) == false) {
			return false;
		}
	}
	return true;
}
