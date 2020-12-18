#include "ecs_utilities.h"

#include "core/config/engine.h"
#include "core/config/project_settings.h"
#include "core/io/resource_loader.h"
#include "core/object/script_language.h"
#include "modules/ecs/ecs.h"

void System::_bind_methods() {
	ClassDB::bind_method(D_METHOD("with_resource", "resource_name", "mutability"), &System::with_resource, DEFVAL(IMMUTABLE));
	ClassDB::bind_method(D_METHOD("with_component", "component_name", "mutability"), &System::with_component, DEFVAL(IMMUTABLE));
	ClassDB::bind_method(D_METHOD("without_component", "component_name"), &System::without_component);

	BIND_ENUM_CONSTANT(IMMUTABLE);
	BIND_ENUM_CONSTANT(MUTABLE);

	ClassDB::add_virtual_method(get_class_static(), MethodInfo("_prepare"));
	// TODO how to define `_for_each`? It has  dynamic argument, depending on the `_prepare` function.
}

System::System() {
}

void System::with_resource(const String &p_resource, Mutability p_mutability) {
}

void System::with_component(const String &p_component, Mutability p_mutability) {
}

void System::without_component(const String &p_component) {
}

String System::validate_script(Ref<Script> p_script) {
	ERR_FAIL_COND_V(p_script.is_null(), "Script is null.");
	ERR_FAIL_COND_V(p_script->is_valid() == false, "Script has some errors.");
	ERR_FAIL_COND_V("System" != p_script->get_instance_base_type(), "This script is not extending `System`.");

	List<PropertyInfo> properties;
	p_script->get_script_property_list(&properties);
	if (properties.size()) {
		return TTR("The System script can't have any property in it. It possible to only access `Component`s and `Resource`s.");
	}

	// This script is safe to use.
	return "";
}

void Component::_bind_methods() {}

Component::Component() {}

Component::~Component() {}

void Component::set_name(StringName p_name) {
	name = p_name;
}

void Component::set_component_script(Ref<Script> p_script) {
	component_script = p_script;
}

StringName Component::get_name() const {
	return name;
}

void Component::get_component_property_list(List<PropertyInfo> *r_info) {
	if (component_script.is_null()) {
		return;
	}

	component_script->get_script_property_list(r_info);
}

Variant Component::get_property_default_value(StringName p_property_name) {
	// TODO this function is EXTREMELY bad! create a `ScriptInstance` is an
	// TODO heavy task. Keep a script instance alive in editor seems unsafe because
	// TODO the script can change anytime and all the instances are immediately
	// TODO invalidated.
	// TODO Please optimize it.
	WARN_PRINT_ONCE("!IMPORTANT [TODO] please optimize the GDScript component get defaul val which is extremely slow!!!!!");

	ERR_FAIL_COND_V(component_script.is_null(), Variant());
	ScriptInstance *si = component_script->instance_create(this);
	ERR_FAIL_COND_V(si == nullptr, Variant());
	Variant ret;
	si->get(p_property_name, ret);
	// Make sure to clear the script, so it's correctly destroyed.
	set_script_instance(nullptr);
	set_script(Ref<Script>());
	return ret;
}

String Component::validate_script(Ref<Script> p_script) {
	ERR_FAIL_COND_V(p_script.is_null(), "Script is null.");
	ERR_FAIL_COND_V(p_script->is_valid() == false, "Script has some errors.");
	ERR_FAIL_COND_V("Component" != p_script->get_instance_base_type(), "This script is not extending `Component`.");

	// Make sure doesn't have any function in it.
	List<MethodInfo> methods;
	p_script->get_script_method_list(&methods);
	if (methods.size() > 0) {
		// Only this method is allowed.
		if (methods.front()->get().name != "@implicit_new") {
			return TTR("The component script can't have any method in it.");
		}
	}

	List<PropertyInfo> properties;
	p_script->get_script_property_list(&properties);
	for (List<PropertyInfo>::Element *e = properties.front(); e; e = e->next()) {
		switch (e->get().type) {
			case Variant::NIL:
				return "(" + e->get().name + ") " + TTR("Please make sure all variables are typed.");
			case Variant::RID:
			case Variant::OBJECT:
				return "(" + e->get().name + ") " + TTR("The Component can't hold unsafe references. The same reference could be holded by multiple things into the engine, this invalidates the thread safety of the ECS model. Please use a Resource or report your use case so a safe native type will be provided instead.");
			case Variant::SIGNAL:
			case Variant::CALLABLE:
				return "(" + e->get().name + ") " + TTR("The Component can't hold signals or callables. Please report your use case.");
			default:
				// Nothing to worry about.
				break;
		}
	}

	// This script is safe to use.
	return "";
}

String resource_validate_script(Ref<Script> p_script) {
	ERR_FAIL_COND_V(p_script.is_null(), "Script is null.");
	ERR_FAIL_COND_V(p_script->is_valid() == false, "Script has some errors.");
	ERR_FAIL_COND_V("Resource" != p_script->get_instance_base_type(), "This script is not extending `Resource`.");

	// TODO the resource are special. Make sure we can use as resource Objects,
	//      loaded files, anything. So we can easily use static things loaded from the disk.

	// This script is safe to use.
	return "Not yet implemented";
}

bool ScriptECS::component_loaded = false;
bool ScriptECS::ecs_initialized = false;

LocalVector<StringName> ScriptECS::component_names;
LocalVector<Ref<Component>> ScriptECS::components;

void ScriptECS::load_components() {
	if (component_loaded) {
		return;
	}
	component_loaded = true;

	if (ProjectSettings::get_singleton()->has_setting("ECS/Component/scripts") == false) {
		return;
	}

	const Array scripts = ProjectSettings::get_singleton()->get_setting("ECS/Component/scripts");
	for (int i = 0; i < scripts.size(); i += 1) {
		reload_component(scripts[i]);
	}
}

uint32_t ScriptECS::reload_component(const String &p_path) {
	const StringName name = p_path.get_file();
	uint32_t id = get_component_id(name);
	if (id == UINT32_MAX) {
		// Component doesn't exists.

		Ref<Script> script = ResourceLoader::load(p_path);

		ERR_FAIL_COND_V_MSG(script.is_null(), UINT32_MAX, "The script [" + p_path + "] can't be loaded.");
		ERR_FAIL_COND_V_MSG(script->is_valid() == false, UINT32_MAX, "The script [" + p_path + "] is not a valid script.");
		ERR_FAIL_COND_V_MSG("Component" != script->get_instance_base_type(), UINT32_MAX, "This script [" + p_path + "] is not extending `Component`.");

		Ref<Component> component;
		component.instance();
		component->set_name(name);
		component->set_component_script(script);

		id = component_names.size();
		component_names.push_back(name);
		components.push_back(component);
	}
	return id;
}

uint32_t ScriptECS::get_component_id(const StringName &p_name) {
	const int64_t index = component_names.find(p_name);
	return index < 0 ? UINT32_MAX : uint32_t(index);
}

const LocalVector<Ref<Component>> &ScriptECS::get_components() {
	load_components();

	return components;
}

Ref<Component> ScriptECS::get_component(uint32_t p_id) {
	load_components();

	return components[p_id];
}

//String ScriptECS::get_component_name(Ref<Script> p_component_script) {
//	ERR_FAIL_COND_V(p_component_script.is_null(), "");
//	return p_component_script->get_path();
//}

void ScriptECS::register_runtime_scripts() {
	if (Engine::get_singleton()->is_editor_hint()) {
		// Only when the editor is off the Scripted components are registered.
		return;
	}
	if (ecs_initialized) {
		return;
	}

	ecs_initialized = true;

	register_dynamic_components();
	// TODO resources
	register_dynamic_systems();
}

void ScriptECS::register_dynamic_components() {
	load_components();

	for (uint32_t i = 0; i < components.size(); i += 1) {
		List<PropertyInfo> raw_properties;
		components[i]->get_component_property_list(&raw_properties);

		LocalVector<ScriptProperty> properties;
		properties.reserve(raw_properties.size());
		for (List<PropertyInfo>::Element *e = raw_properties.front(); e; e = e->next()) {
			properties.push_back({ e->get(),
					// TODO use a way to get all the values at once.
					components[i]->get_property_default_value(e->get().name) });
		}

		ECS::register_script_component(
				component_names[i],
				properties,
				// TODO make the storage customizable.
				StorageType::DENSE_VECTOR);
	}
}

void ScriptECS::register_dynamic_systems() {
}
