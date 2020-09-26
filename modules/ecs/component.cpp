#include "component.h"

/** @author AndreaCatania */

uint32_t Component::get_component_id() {
	return component_id;
}

Component::Component() {
}

void Component::_bind_properties() {}

OAHashMap<StringName, PropertyInfo> *Component::get_properties() const {
	CRASH_NOW_MSG("The component class must always be overridden.");
	return nullptr;
}