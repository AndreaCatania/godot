#pragma once

/**
	@author AndreaCatania
*/

#include "core/local_vector.h"
#include "core/string_name.h"
#include "ecs_types.h"
#include "modules/ecs/storages/storage.h"
#include "systems/system.h"

class Storage;
class Pipeline;
class ECSResource;

/// Utility that can be used to create an entity with components.
/// You can use it in this way:
/// ```
///	Pipeline pipeline;
///
///	pipeline.create_entity()
///			.with(TransformComponent())
///			.with(MeshComponent());
/// ```
class EntityBuilder {
	friend class Pipeline;

	EntityID entity;
	Pipeline *pipeline;

private:
	EntityBuilder(Pipeline *p_pipeline);
	EntityBuilder &operator=(const EntityBuilder &) = delete;
	EntityBuilder &operator=(EntityBuilder) = delete;
	EntityBuilder(const EntityBuilder &) = delete;
	EntityBuilder() = delete;

public:
	template <class C>
	const EntityBuilder &with(const C &p_data) const;

	operator EntityID() const {
		return entity;
	}
};

class Pipeline {
	LocalVector<Storage *> storages;
	LocalVector<system_execute> systems;
	LocalVector<ECSResource *> resources;
	uint32_t entity_count = 0;
	EntityBuilder entity_builder = EntityBuilder(this);

public:
	/// Creates a new Entity id. You can add the components using the function
	/// `add_component`.
	EntityID create_entity_index();

	/// Creates a new Entity id and returns an `EntityBuilder`.
	/// You can use the `EntityBuilder` in this way:
	/// ```
	///	Pipeline pipeline;
	///
	///	pipeline.create_entity()
	///			.with(TransformComponent())
	///			.with(MeshComponent());
	/// ```
	///
	/// It's possible to get the `EntityID` just by assining the `EntityBuilder`
	/// to an `EntityID`.
	/// ```
	///	EntityID entity = pipeline.create_entity()
	///			.with(MeshComponent());
	/// ```
	///
	/// Note: The `EntityBuilder` reference points to an internal variable.
	/// It's undefined behavior use it in any other way than the above one.
	const EntityBuilder &create_entity();

	/// Remove the entity from this Pipeline.
	void destroy_entity(EntityID p_entity);

	/// Returns the last created EntityID or UINT32_MAX.
	EntityID get_last_entity_id() const;

	/// Adds a new component (or sets the default if already exists) to a
	/// specific Entity.
	template <class C>
	void add_component(EntityID p_entity, const C &p_data);

	/// Adds a new component using the name of the component and Variant data to
	/// initialize it. Usually this function is used to initialize the component
	/// from GDScript data.
	void add_component(EntityID p_entity, StringName p_component_name, const Variant &p_data);

	/// Returns the constant storage pointer.
	/// If the storage doesn't exist, returns null.
	/// If the type is wrong, this function crashes.
	template <class C>
	const TypedStorage<const C> *get_storage() const;

	/// Returns the storage pointer.
	/// If the storage doesn't exist, returns null.
	/// If the type is wrong, this function crashes.
	template <class C>
	TypedStorage<C> *get_storage();

	void add_system(get_system_info_func p_get_info_func);

	/// Adds a new resource or updates it if already exists.
	template <class R>
	void add_resource(const R &p_resource);

	template <class R>
	R &get_resource();

	void dispatch();

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
const EntityBuilder &EntityBuilder::with(const C &p_data) const {
	pipeline->add_component(entity, p_data);
	return *this;
}

template <class C>
void Pipeline::add_component(EntityID p_entity, const C &p_data) {
	create_storage<C>();
	TypedStorage<C> *storage = get_storage<C>();
	ERR_FAIL_COND(storage == nullptr);
	storage->insert(p_entity, p_data);
}

template <class C>
const TypedStorage<const C> *Pipeline::get_storage() const {
	const uint32_t id = C::get_component_id();
	ERR_FAIL_COND_V_MSG(id == UINT32_MAX, nullptr, "The component is not registered.");

	if (id >= storages.size() || storages[id] == nullptr) {
		return nullptr;
	}

	return static_cast<TypedStorage<const C> *>(storages[id]);
}

template <class C>
TypedStorage<C> *Pipeline::get_storage() {
	const uint32_t id = C::get_component_id();
	ERR_FAIL_COND_V_MSG(id == UINT32_MAX, nullptr, "The component is not registered.");

	if (id >= storages.size() || storages[id] == nullptr) {
		return nullptr;
	}

	return static_cast<TypedStorage<C> *>(storages[id]);
}

template <class R>
void Pipeline::add_resource(const R &p_resource) {
	const uint32_t id = R::get_resource_id();
	ERR_FAIL_COND_MSG(id == UINT32_MAX, "The resource is not registered.");

	if (id >= resources.size()) {
		const uint32_t start = resources.size();
		resources.resize(id + 1);
		for (uint32_t i = start; i < resources.size(); i += 1) {
			resources[i] = nullptr;
		}
	}

	if (resources[id] == nullptr) {
		resources[id] = memnew(R());
	}

	(*resources[id]) = p_resource;
}

template <class R>
R &Pipeline::get_resource() {
	const uint32_t id = R::get_resource_id();
	CRASH_COND_MSG(id == UINT32_MAX, "The resource is not registered.");

	if (unlikely(id >= resources.size() || resources[id] == nullptr)) {
		add_resource(R());
	}

	return *static_cast<R *>(resources[id]);
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

// By defining the same name of the method, the IDE autocomplete shows the method
// name `add_system`, properly + it's impossible use the function directly
// by mistake.
#define add_system(func)                                                   \
	add_system([]() -> SystemInfo {                                        \
		SystemInfo i = SystemBuilder::get_system_info_from_function(func); \
		i.system_func = [](Pipeline *p_pipeline) {                         \
			SystemBuilder::system_exec_func(p_pipeline, func);             \
		};                                                                 \
		return i;                                                          \
	})
