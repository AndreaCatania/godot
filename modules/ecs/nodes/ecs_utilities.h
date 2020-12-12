#pragma once

#include "core/io/resource.h"
#include "core/templates/local_vector.h"
#include "core/templates/oa_hash_map.h"

class Script;

class System : public Resource {
	GDCLASS(System, Resource);

	static void _bind_methods();

public:
	enum Mutability {
		IMMUTABLE,
		MUTABLE
	};

public:
	System();

	void with_resource(const String &p_resource, Mutability p_mutability);
	void with_component(const String &p_component, Mutability p_mutability);
	void without_component(const String &p_component);

	static String validate_script(Ref<Script> p_script);
};

VARIANT_ENUM_CAST(System::Mutability);

class Component : public Resource {
	GDCLASS(Component, Resource);

	friend class ScriptECS;

	String name;
	Ref<Script> component_script;

	static void _bind_methods();

	void set_name(String p_name);
	void set_component_script(Ref<Script> p_script);

public:
	Component();
	~Component();

	const String &get_name() const;

	void get_component_property_list(List<PropertyInfo> *p_info);
	Variant get_property_default_value(StringName p_property_name);

	static String validate_script(Ref<Script> p_script);
};

String resource_validate_script(Ref<Script> p_script);

/// Utility that allow to handle the godot scripted Component, Resources, Systems.
class ScriptECS {
	static LocalVector<String> component_names;
	static LocalVector<Ref<Component>> components;

public:
	static uint32_t get_component_id(const String &p_name);

	/// Loads components.
	static void load_components();

	/// Load or Reloads a component. Retuns the component id.
	static uint32_t reload_component(const String &p_path);

	static const LocalVector<Ref<Component>> &get_components();

	static Ref<Component> get_component(uint32_t p_id);
};
