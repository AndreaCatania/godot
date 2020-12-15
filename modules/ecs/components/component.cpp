#include "component.h"

/** @author AndreaCatania */

using godex::Component;

Component::Component() {
}

void Component::_bind_properties() {}

const LocalVector<PropertyInfo> *Component::get_properties() const {
	CRASH_NOW_MSG("The component class must always be tagged using the macro `COMPONENT()`.");
	return nullptr;
}

void Component::set(StringName p_name, Variant p_data) {
	CRASH_NOW_MSG("The component class must always be tagged using the macro `COMPONENT()`.");
}

Variant Component::get(StringName p_name) const {
	CRASH_NOW_MSG("The component class must always be tagged using the macro `COMPONENT()`.");
	return Variant();
}
