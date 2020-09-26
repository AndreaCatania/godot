/* Author: AndreaCatania */

#ifndef ECS_H
#define ECS_H

#include "core/local_vector.h"
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
	virtual String get_class() const {
		return "ECSClass";
	}
};

class EntityIndex {
	uint32_t index;

public:
	EntityIndex() :
			index(UINT32_MAX) {}
	EntityIndex(uint32_t p_index) :
			index(p_index) {}
	EntityIndex(const EntityIndex &p_other) :
			index(p_other.index) {}

	bool is_null() const {
		return index == UINT32_MAX;
	}

	bool operator==(const EntityIndex &p_other) {
		return p_other.index == index;
	}

	bool operator==(uint32_t p_naked_index) {
		return index == p_naked_index;
	}

	operator uint32_t() const {
		return index;
	}
};

class ECS : public Object {
	GDCLASS(ECS, Object);

	friend class Entity;

	static ECS *singleton;
	static LocalVector<StringName> components;

public:
	template <class C>
	static void register_component();

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
}

#endif