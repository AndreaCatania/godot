#pragma once

/* Author: AndreaCatania */

#include "core/local_vector.h"
#include "core/oa_hash_map.h"
#include "core/object.h"

#define ECSCLASS(m_class)                             \
private:                                              \
	friend class ECS;                                 \
                                                      \
public:                                               \
	virtual String get_class() const override {       \
		return String(#m_class);                      \
	}                                                 \
	static _FORCE_INLINE_ String get_class_static() { \
		return String(#m_class);                      \
	}                                                 \
                                                      \
private:

class ECSClass {
public:
	virtual ~ECSClass(){}
	virtual String get_class() const {
		return "ECSClass";
	}
};

class EntityID {
	uint32_t id = UINT32_MAX;

public:
	EntityID() :
			id(UINT32_MAX) {}

	EntityID(uint32_t p_index) :
			id(p_index) {}

	bool is_null() const {
		return id == UINT32_MAX;
	}

	bool operator==(const EntityID &p_other) const {
		return id == p_other.id;
	}

	bool operator==(uint32_t p_naked_index) const {
		return id == p_naked_index;
	}

	operator uint32_t() const {
		return id;
	}
};

struct ComponentInfo {
	OAHashMap<StringName, PropertyInfo> *(*get_properties)();
};

class ECS : public Object {
	GDCLASS(ECS, Object);

	friend class Entity;

	static ECS *singleton;
	static LocalVector<StringName> components;
	static LocalVector<ComponentInfo> components_info;
	static LocalVector<StringName> resources;

public:
	template <class C>
	static void register_component();

	static const LocalVector<StringName> &get_registered_components();
	static const OAHashMap<StringName, PropertyInfo> *get_component_properties(StringName p_component_name);

	template <class C>
	static void register_resource();

	static const LocalVector<StringName> &get_registered_resources();

protected:
	static void _bind_methods();

public:
	static void __set_singleton(ECS *p_singleton);
	static ECS *get_singleton();

	ECS();
	virtual ~ECS();

private:
	void ecs_init();
};

template <class C>
void ECS::register_component() {
	ERR_FAIL_COND_MSG(C::get_component_id() != UINT32_MAX, "This component is already registered.");

	StringName component_name = C::get_class_static();
	C::component_id = components.size();
	C::_bind_properties();
	components.push_back(component_name);
	components_info.push_back(
			ComponentInfo{
					&C::get_properties_static });
}

template <class R>
void ECS::register_resource() {
	ERR_FAIL_COND_MSG(R::get_resource_id() != UINT32_MAX, "This resource is already registered.");

	StringName resource_name = R::get_class_static();
	R::resource_id = resources.size();
	R::_bind_properties();
	resources.push_back(resource_name);
}
