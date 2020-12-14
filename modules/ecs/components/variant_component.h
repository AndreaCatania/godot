#pragma once

#include "component.h"

/// Structure used to create dynamic components.
class ScriptComponentInfo {
	friend class ECS;

	uint32_t component_id = UINT32_MAX;
	// Maps the property to the position
	OAHashMap<StringName, uint32_t> property_map;
	LocalVector<ScriptProperty> properties;

	ScriptComponentInfo();

public:
};

template <int SIZE>
class VariantComponent : public godex::Component {
	ScriptComponentInfo *info;

	Variant data[SIZE];

	VariantComponent(ScriptComponentInfo *p_info) :
			info(p_info) {}

public:
};
