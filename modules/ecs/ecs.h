#pragma once

/* Author: AndreaCatania */

#include "core/local_vector.h"
#include "core/oa_hash_map.h"
#include "core/object.h"
#include "pipeline_commands.h"

class Pipeline;

struct ComponentInfo {
	OAHashMap<StringName, PropertyInfo> *(*get_properties)();
	void (*add_component_by_name)(Pipeline *, EntityID, const Variant &);
};

class ECS : public Object {
	GDCLASS(ECS, Object)

	static ECS *singleton;
	static LocalVector<StringName> components;
	static LocalVector<ComponentInfo> components_info;
	static LocalVector<StringName> resources;

	Pipeline *active_pipeline = nullptr;
	PipelineCommands commands;

public:
	template <class C>
	static void register_component();

	static const LocalVector<StringName> &get_registered_components();
	static const OAHashMap<StringName, PropertyInfo> *get_component_properties(StringName p_component_name);
	static void add_component_by_name(Pipeline *p_pipeline, EntityID p_entity, StringName p_component_name, const Variant &p_data);

	template <class C>
	static void register_resource();

	static const LocalVector<StringName> &get_registered_resources();

protected:
	static void _bind_methods();

public:
	static void __set_singleton(ECS *p_singleton);
	static ECS *get_singleton();

public:
	ECS();
	virtual ~ECS();

	/// Set the active pipeline. If there is already an active pipeline an error
	/// is generated.
	void set_active_pipeline(Pipeline *p_pipeline);
	bool has_active_pipeline() const;

	/// Returns a command object that can be used to spawn entities, add
	/// components.
	/// This function returns nullptr when the pipeline is dispatched because
	/// it's unsafe interact during that phase.
	PipelineCommands *get_commands();

private:
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
