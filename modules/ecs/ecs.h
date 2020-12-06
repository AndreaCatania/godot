#pragma once

/* Author: AndreaCatania */

#include "core/object/class_db.h"
#include "core/object/object.h"
#include "core/templates/local_vector.h"
#include "core/templates/oa_hash_map.h"
#include "modules/ecs/world/world_commands.h"

#include "modules/ecs/systems/system.h"
#include "modules/ecs/systems/system_builder.h"

class World;

struct ComponentInfo {
	OAHashMap<StringName, PropertyInfo> *(*get_properties)();
	// This functions is implemented by the `COMPONENT` macro.
	void (*add_component_by_name)(World *, EntityID, const Variant &);
};

class ECS : public Object {
	GDCLASS(ECS, Object)

	friend class Main;

	static ECS *singleton;
	static LocalVector<StringName> components;
	static LocalVector<ComponentInfo> components_info;

	static LocalVector<StringName> resources;

	static LocalVector<StringName> systems;
	static LocalVector<SystemInfo> systems_info;

	World *active_world = nullptr;
	WorldCommands commands;

public:
	template <class C>
	static void register_component();

	static const LocalVector<StringName> &get_registered_components();
	static const OAHashMap<StringName, PropertyInfo> *get_component_properties(StringName p_component_name);
	static void add_component_by_name(World *p_world, EntityID p_entity, StringName p_component_name, const Variant &p_data);

	template <class C>
	static void register_resource();

	static const LocalVector<StringName> &get_registered_resources();

	static void register_system(get_system_info_func p_get_info_func, StringName p_name, String p_description = "");

// This macro save the user the need to pass a `SystemInfo`, indeed it wraps
// the passed function with a labda function that creates a `SystemInfo`.
// By defining the same name of the method, the IDE autocomplete shows the method
// name `register_system`, properly + it's impossible use the function directly
// by mistake.
#define register_system(func, name, desc)                                  \
	register_system([]() -> SystemInfo {                                   \
		SystemInfo i = SystemBuilder::get_system_info_from_function(func); \
		i.system_func = [](World *p_world) {                               \
			SystemBuilder::system_exec_func(p_world, func);                \
		};                                                                 \
		return i;                                                          \
	},                                                                     \
			name, desc)

	/// Returns the system id or UINT32_MAX if not found.
	static uint32_t find_system_id(StringName p_name);
	static uint32_t get_systems_count();
	static const SystemInfo &get_system_info(uint32_t p_system_id);

protected:
	static void _bind_methods();

public:
	static void __set_singleton(ECS *p_singleton);
	static ECS *get_singleton();

public:
	ECS();
	virtual ~ECS();

	/// Set the active world. If there is already an active world an error
	/// is generated.
	void set_active_world(World *p_world);
	bool has_active_world() const;

	/// Returns a command object that can be used to spawn entities, add
	/// components.
	/// This function returns nullptr when the world is dispatched because
	/// it's unsafe interact during that phase.
	WorldCommands *get_commands();

private:
	bool dispatch_active_world();
	void ecs_init();
};

template <class C>
void ECS::register_component() {
	ERR_FAIL_COND_MSG(C::get_component_id() != UINT32_MAX, "This component is already registered.");

	StringName component_name = C::get_class_static();
	C::component_id = components.size();
	C::_bind_properties();
	components.push_back(component_name);
	components_info.push_back(
			ComponentInfo{
					&C::get_properties_static,
					&C::add_component_by_name });
}

template <class R>
void ECS::register_resource() {
	ERR_FAIL_COND_MSG(R::get_resource_id() != UINT32_MAX, "This resource is already registered.");

	StringName resource_name = R::get_class_static();
	R::resource_id = resources.size();
	R::_bind_properties();
	resources.push_back(resource_name);
}
