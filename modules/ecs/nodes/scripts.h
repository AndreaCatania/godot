#pragma once

#include "core/io/resource.h"

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

	static void _bind_methods();

public:
	Component();

	static String validate_script(Ref<Script> p_script);
};

String resource_validate_script(Ref<Script> p_script);
