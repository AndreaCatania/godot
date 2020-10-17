
#include "ecs.h"

#include "core/message_queue.h"
#include "pipeline.h"
#include "scene/main/scene_tree.h"

ECS *ECS::singleton = nullptr;
LocalVector<StringName> ECS::components;
LocalVector<ComponentInfo> ECS::components_info;
LocalVector<StringName> ECS::resources;

void ECS::_bind_methods() {
	ADD_SIGNAL(MethodInfo("pipeline_loaded"));
	ADD_SIGNAL(MethodInfo("pipeline_pre_unload"));
	ADD_SIGNAL(MethodInfo("pipeline_unloaded"));
}

ECS::ECS() :
		Object() {
	// TODO Do I need this?
	MessageQueue::get_singleton()->push_callable(callable_mp(this, &ECS::ecs_init));
}

ECS::~ECS() {
}

const LocalVector<StringName> &ECS::get_registered_components() {
	return components;
}

const OAHashMap<StringName, PropertyInfo> *ECS::get_component_properties(StringName p_component_name) {
	const int64_t id = components.find(p_component_name);
	ERR_FAIL_COND_V_MSG(id == -1, nullptr, "The component " + p_component_name + " doesn't exist or it's not registered.");
	return components_info[id].get_properties();
}

void ECS::add_component_by_name(
		Pipeline *p_pipeline,
		EntityID p_entity,
		StringName p_component_name,
		const Variant &p_data) {
	const int64_t id = components.find(p_component_name);
	ERR_FAIL_COND_MSG(id == -1, "The component " + p_component_name + " doesn't exist or it's not registered.");
	components_info[id].add_component_by_name(p_pipeline, p_entity, p_data);
}

const LocalVector<StringName> &ECS::get_registered_resources() {
	return resources;
}

ECS *ECS::get_singleton() {
	return singleton;
}

void ECS::__set_singleton(ECS *p_singleton) {
	if (p_singleton == nullptr) {
		ERR_FAIL_COND(singleton == nullptr);
		singleton = nullptr;
	} else {
		ERR_FAIL_COND_MSG(singleton != nullptr, "There is already a singleton, make sure to remove that first.");
		singleton = p_singleton;
	}
}

void ECS::set_active_pipeline(Pipeline *p_pipeline) {
	if (active_pipeline != nullptr) {
		if (p_pipeline == nullptr) {
			emit_signal("pipeline_pre_unload");
		} else {
			ERR_FAIL_COND("Before adding a new pipeline it's necessary remove the current one by calling `set_active_pipeline(nullptr);`.");
		}
	}

	active_pipeline = p_pipeline;

	if (active_pipeline != nullptr) {
		// The pipeline is just loaded.
		emit_signal("pipeline_loaded");
	} else {
		// The pipeline is just unloaded.
		emit_signal("pipeline_unloaded");
	}
}

bool ECS::has_active_pipeline() const {
	return active_pipeline != nullptr;
}

PipelineCommands *ECS::get_commands() {
	// TODO make sure this returns nullptr when the pipeline is dispatched.
	ERR_FAIL_COND_V_MSG(active_pipeline == nullptr, nullptr, "No active WorldsECS.");
	commands.pipeline = active_pipeline;
	return &commands;
}

void ECS::ecs_init() {
}
