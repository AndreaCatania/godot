#pragma once

#include "component.h"

/// Structure used to create dynamic components.
class ScriptComponentInfo {
	friend class ECS;

	uint32_t component_id = UINT32_MAX;
	// Maps the property to the position
	OAHashMap<StringName, uint32_t> property_map;
	LocalVector<PropertyInfo> properties;
	LocalVector<Variant> defaults;
	StorageType storage_type;

	ScriptComponentInfo();

public:
	Storage *create_storage();

	const LocalVector<PropertyInfo> *get_properties() const {
		return &properties;
	}
};

template <int SIZE>
class VariantComponent : public godex::Component {
	ScriptComponentInfo *info = nullptr;

	Variant data[SIZE];

	void initialize(ScriptComponentInfo *p_info);

public:
	VariantComponent() {}

	virtual const LocalVector<PropertyInfo> *get_properties() const override;
	virtual void set(StringName p_name, Variant p_data) override;
	virtual Variant get(StringName p_name) const override;
};

template <int SIZE>
void VariantComponent<SIZE>::initialize(ScriptComponentInfo *p_info) {
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
	// TODO
	CRASH_NOW();
}

template <int SIZE>
Variant VariantComponent<SIZE>::get(StringName p_name) const {
	// TODO
	CRASH_NOW();
	return Variant();
}
