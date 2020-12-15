
#include "ecs.h"

#include "components/variant_component.h"
#include "core/object/message_queue.h"
#include "scene/main/scene_tree.h"
#include "world/world.h"

ECS *ECS::singleton = nullptr;
LocalVector<StringName> ECS::components;
LocalVector<ComponentInfo> ECS::components_info;
LocalVector<StringName> ECS::resources;
LocalVector<StringName> ECS::systems;
LocalVector<SystemInfo> ECS::systems_info;

void ECS::_bind_methods() {
	ADD_SIGNAL(MethodInfo("world_loaded"));
	ADD_SIGNAL(MethodInfo("world_pre_unload"));
	ADD_SIGNAL(MethodInfo("world_unloaded"));
}

ECS::ECS() :
		Object() {
	if (MessageQueue::get_singleton() != nullptr) {
		// TODO Do I need this? https://github.com/godotengine/godot-proposals/issues/1593
		MessageQueue::get_singleton()->push_callable(callable_mp(this, &ECS::ecs_init));
	}
}

ECS::~ECS() {
}

const LocalVector<StringName> &ECS::get_registered_components() {
	return components;
}

uint32_t ECS::get_component_id(StringName p_component_name) {
	const int64_t i = components.find(p_component_name);
	return i < 0 ? UINT32_MAX : uint32_t(i);
}

StringName ECS::get_component_name(uint32_t p_component_id) {
	ERR_FAIL_INDEX_V_MSG(p_component_id, components.size(), "", "The `component_id` is invalid: " + itos(p_component_id));
	return components[p_component_id];
}

const OAHashMap<StringName, PropertyInfo> *ECS::get_component_properties(uint32_t p_component_id) {
	ERR_FAIL_INDEX_V_MSG(p_component_id, components.size(), nullptr, "The `component_id` is invalid: " + itos(p_component_id));
	return components_info[p_component_id].get_properties();
}

const OAHashMap<StringName, PropertyInfo> *ECS::get_component_properties(StringName p_component_name) {
	const int64_t id = components.find(p_component_name);
	ERR_FAIL_COND_V_MSG(id == -1, nullptr, "The component " + p_component_name + " doesn't exist or it's not registered.");
	return get_component_properties(id);
}

Variant ECS::get_component_property_default(StringName p_component_name, StringName p_property_name) {
	const int64_t id = components.find(p_component_name);
	ERR_FAIL_COND_V_MSG(id == -1, Variant(), "The component " + p_component_name + " doesn't exist or it's not registered.");
	return components_info[id].get_property_default(p_property_name);
}

void ECS::add_component_by_name(
		World *p_world,
		EntityID p_entity,
		StringName p_component_name,
		const Variant &p_data) {
	const int64_t id = components.find(p_component_name);
	ERR_FAIL_COND_MSG(id == -1, "The component " + p_component_name + " doesn't exist or it's not registered.");
	components_info[id].add_component_by_name(p_world, p_entity, p_data);
}

const LocalVector<StringName> &ECS::get_registered_resources() {
	return resources;
}

StringName ECS::get_resource_name(uint32_t p_resource_id) {
	ERR_FAIL_INDEX_V_MSG(p_resource_id, resources.size(), "", "The `resource_id` is invalid: " + itos(p_resource_id));
	return resources[p_resource_id];
}

// Undefine the macro defined into `ecs.h` so we can define the method properly.
#undef register_system
void ECS::register_system(get_system_info_func p_get_info_func, StringName p_name, String p_description) {
	SystemInfo info = p_get_info_func();
	info.name = p_name;
	info.description = p_description;

	systems.push_back(p_name);
	systems_info.push_back(info);
}

uint32_t ECS::find_system_id(StringName p_name) {
	const int64_t index = systems.find(p_name);
	return index >= 0 ? uint32_t(index) : UINT32_MAX;
}

uint32_t ECS::get_systems_count() {
	return systems.size();
}

static const SystemInfo invalid_system_info;
const SystemInfo &ECS::get_system_info(uint32_t p_system_id) {
	ERR_FAIL_INDEX_V_MSG(p_system_id, systems_info.size(), invalid_system_info, "The SystemID: " + itos(p_system_id) + " doesn't exists.");
	return systems_info[p_system_id];
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

void ECS::set_active_world(World *p_world) {
	if (active_world != nullptr) {
		if (p_world == nullptr) {
			emit_signal("world_pre_unload");
		} else {
			ERR_FAIL_COND("Before adding a new world it's necessary remove the current one by calling `set_active_world(nullptr);`.");
		}
	}

	active_world = p_world;

	if (active_world != nullptr) {
		// The world is just loaded.
		emit_signal("world_loaded");
	} else {
		// The world is just unloaded.
		emit_signal("world_unloaded");
	}
}

bool ECS::has_active_world() const {
	return active_world != nullptr;
}

WorldCommands *ECS::get_commands() {
	// TODO make sure this returns nullptr when the world is dispatched.
	ERR_FAIL_COND_V_MSG(active_world == nullptr, nullptr, "No active WorldsECS.");
	commands.world = active_world;
	return &commands;
}

bool ECS::dispatch_active_world() {
	if (likely(active_world)) {
		active_world->dispatch();
	}

	// TODO add a way to terminate, from a system, the engine execution
	// returning true. (Hint: use a resource).
	return false;
}

void ECS::ecs_init() {
}

uint32_t ECS::register_script_component(StringName p_name, const LocalVector<ScriptProperty> &p_properties) {
	uint32_t id = get_component_id(p_name);
	ERR_FAIL_COND_V_MSG(id != UINT32_MAX, id, "The script component " + p_name + " is already registered.");

	// This component is not registered, go ahead.
	ScriptComponentInfo *info = memnew(ScriptComponentInfo);

	info->properties.resize(p_properties.size());
	info->defaults.resize(p_properties.size());

	// Validate and initialize the parameters.
	for (uint32_t i = 0; i < p_properties.size(); i += 1) {
		switch (p_properties[i].property.type) {
			case Variant::NIL:
			case Variant::RID:
			case Variant::OBJECT:
			case Variant::SIGNAL:
			case Variant::CALLABLE:
				// TODO what about dictionary and arrays?
				memdelete(info);
				ERR_PRINT("The script component " + p_name + " is using a pointer variable. This is unsafe, so not supported. Please use a resource.");
				return UINT32_MAX;
			default:
				// Nothing to do.
				break;
		}

		info->property_map.insert(p_properties[i].property.name, i);
		info->properties[i] = p_properties[i].property;
		info->defaults[i] = p_properties[i].default_value;
	}

	info->component_id = components.size();

	components.push_back(p_name);
	components_info.push_back(
			ComponentInfo{
					nullptr,
					nullptr,
					nullptr,
					nullptr,
					info });

	return id;
}

bool ECS::verify_component_id(uint32_t p_component_id) {
	return components.size() > p_component_id;
}

Storage *ECS::create_storage(uint32_t p_component_id) {
#ifdef DEBUG_ENABLED
	// Crash cond because this function is not supposed to fail in any way.
	CRASH_COND_MSG(ECS::verify_component_id(p_component_id), "This component id " + itos(p_component_id) + " is not valid.");
#endif
	if (components_info[p_component_id].script_component_info) {
		// This is a script component
		return components_info[p_component_id].script_component_info->create_storage();
	} else {
		// This is a native component.
		return components_info[p_component_id].create_storage();
	}
}
