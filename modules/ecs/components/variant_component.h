#pragma once

#include "component.h"

/// Structure used to create dynamic components.
class DynamicComponentInfo {
	friend class ECS;

	uint32_t component_id = UINT32_MAX;
	// Maps the property to the position
	OAHashMap<StringName, uint32_t> property_map; // TODO make this LocalVector?
	LocalVector<PropertyInfo> properties;
	LocalVector<Variant> defaults;
	StorageType storage_type;

	DynamicComponentInfo();

public:
	Storage *create_storage();

	const LocalVector<PropertyInfo> *get_properties() const {
		return &properties;
	}

	Variant get_property_default(StringName p_name) const {
		const uint32_t *id_ptr = property_map.lookup_ptr(p_name);
		ERR_FAIL_COND_V_MSG(id_ptr == nullptr, Variant(), "The property " + p_name + " doesn't exists on this component " + ECS::get_component_name(component_id));
		return defaults[*id_ptr];
	}

	uint32_t get_property_id(StringName p_name) const {
		const uint32_t *id_ptr = property_map.lookup_ptr(p_name);
		ERR_FAIL_COND_V_MSG(id_ptr == nullptr, UINT32_MAX, "The property " + p_name + " doesn't exists on this component " + ECS::get_component_name(component_id));
		return *id_ptr;
	}
};

/// The `VariantComponent` is a special type component designed for godot
/// scripts. The components are stored consecutively.
template <int SIZE>
class VariantComponent : public godex::Component {
	DynamicComponentInfo *info = nullptr;

	Variant data[SIZE];

	void initialize(DynamicComponentInfo *p_info);

public:
	VariantComponent() {}

	virtual const LocalVector<PropertyInfo> *get_properties() const override;
	virtual void set(StringName p_name, Variant p_data) override;
	virtual Variant get(StringName p_name) const override;
};

template <int SIZE>
void VariantComponent<SIZE>::initialize(DynamicComponentInfo *p_info) {
	info = p_info;
	CRASH_COND_MSG(p_info == nullptr, "The component info can't be nullptr.");
	CRASH_COND_MSG(info->properties.size() != SIZE, "The VariantComponent(size: " + itos(SIZE) + ") got created with a ScriptComponentInfo that has " + itos(info->properties.size()) + " parameters, this is not supposed to happen.");
}

template <int SIZE>
const LocalVector<PropertyInfo> *VariantComponent<SIZE>::get_properties() const {
	return info->get_properties();
}

template <int SIZE>
void VariantComponent<SIZE>::set(StringName p_name, Variant p_data) {
#ifdef DEBUG_ENABLED
	CRASH_COND_MSG(info == nullptr, "The component is not initialized. This is not supposed to happen.");
#endif
	const uint32_t index = info->get_property_id(p_name);
	ERR_FAIL_COND(index >= SIZE);
	return data[index] = p_data;
}

template <int SIZE>
Variant VariantComponent<SIZE>::get(StringName p_name) const {
#ifdef DEBUG_ENABLED
	CRASH_COND_MSG(info == nullptr, "The component is not initialized. This is not supposed to happen.");
#endif
	const uint32_t index = info->get_property_id(p_name);
	ERR_FAIL_COND_V(index >= SIZE, Variant());
	return data[index];
}
