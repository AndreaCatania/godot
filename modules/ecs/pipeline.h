
/**
	@author AndreaCatania
*/

#ifndef PIPELINE_H
#define PIPELINE_H

#include "core/local_vector.h"
#include "ecs.h"
#include "modules/ecs/storages/storage.h"

class Storage;

class Pipeline {
	LocalVector<Storage *> storages;
	uint32_t entity_count = 0;

public:
	EntityIndex create_entity();

	/// Add a new component or reset if already exists.
	template <class C>
	void add_component(EntityIndex p_entity, const C &p_data);

	/// Returns the constant storage pointer.
	/// If the storage doesn't exist, returns null.
	/// If the type is wrong, this function crashes.
	template <class C>
	const TypedStorage<C> *get_storage_const() const;

	/// Returns the storage pointer.
	/// If the storage doesn't exist, returns null.
	/// If the type is wrong, this function crashes.
	template <class C>
	TypedStorage<C> *get_storage_mut();

private:
	/// Creates a new component storage into the pipeline, if the storage
	/// already exists, does nothing.
	template <class C>
	void create_storage();

	/// Destroy a component storage if exists.
	// TODO when this is called?
	template <class C>
	void destroy_storage();
};

template <class C>
void Pipeline::add_component(EntityIndex p_entity, const C &p_data) {
	create_storage<C>();
	TypedStorage<C> *storage = get_storage_mut<C>();
	ERR_FAIL_COND(storage == nullptr);
	storage->insert(p_entity, p_data);
}

template <class C>
const TypedStorage<C> *Pipeline::get_storage_const() const {
	const uint32_t id = C::get_component_id();
	ERR_FAIL_COND_V_MSG(id == UINT32_MAX, nullptr, "The component is not registered.");

	if (id >= storages.size() || storages[id] == nullptr) {
		return nullptr;
	}

#ifdef DEBUG_ENABLED
	ERR_FAIL_COND_V_MSG(dynamic_cast<TypedStorage<C> *>(storages[id]) == nullptr, nullptr, "[FATAL] The data type (" + String(typeid(C).name()) + ") is not compatible with the storage type: (" + storages[id]->get_type_name() + ")");
#endif

	return static_cast<TypedStorage<C> *>(storages[id]);
}

template <class C>
TypedStorage<C> *Pipeline::get_storage_mut() {
	const uint32_t id = C::get_component_id();
	ERR_FAIL_COND_V_MSG(id == UINT32_MAX, nullptr, "The component is not registered.");

	if (id >= storages.size() || storages[id] == nullptr) {
		return nullptr;
	}

#ifdef DEBUG_ENABLED
	ERR_FAIL_COND_V_MSG(dynamic_cast<TypedStorage<C> *>(storages[id]) == nullptr, nullptr, "[FATAL] The data type (" + String(typeid(C).name()) + ") is not compatible with the storage type: (" + storages[id]->get_type_name() + ")");
#endif

	return static_cast<TypedStorage<C> *>(storages[id]);
}

template <class C>
void Pipeline::create_storage() {
	const uint32_t id = C::get_component_id();
	// Using crash because this function is not expected to fail.
	CRASH_COND_MSG(id == UINT32_MAX, "The component is not registered.");

	if (id >= storages.size()) {
		const uint32_t start = storages.size();
		storages.resize(id + 1);
		for (uint32_t i = start; i < storages.size(); i += 1) {
			storages[i] = nullptr;
		}
	} else {
		if (storages[id] != nullptr) {
			// Nothing to do.
			return;
		}
	}

	storages[id] = C::create_storage();
}

template <class C>
void Pipeline::destroy_storage() {
	const uint32_t id = C::get_component_id();
	// Using crash because this function is not expected to fail.
	CRASH_COND_MSG(id == UINT32_MAX, "The component is not registered.");

	if (id >= storages.size() || storages[id] == nullptr) {
		// Nothing to do.
		return;
	}

	C::destroy_storage(storages[id]);
}

#endif
