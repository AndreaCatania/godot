/* Author: AndreaCatania */

#ifndef ECS_H
#define ECS_H

#include "core/oa_hash_map.h"
#include "core/object.h"

class Storage;

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

// TODO move this elsewhere.
// TODO just a naive test
struct SystemMethod {
	virtual ~SystemMethod();
	virtual void execute(Vector<Object *> &p_objects) = 0;
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

	// TODO register the components it's fine, but the storages must be stored per pipeline.
	// Yes, we will have move pipelines, not just one.
	static OAHashMap<StringName, Storage *> components;

	uint32_t entity_count;

	// TODO just a naive test
	Vector<SystemMethod *> systems;

public:
	template <class C>
	static void register_component();

	template <class C>
	static void unregister_component();

protected:
	static void _bind_methods();

public:
	static void __set_singleton(ECS *p_singleton);
	static ECS *get_singleton();

	ECS();
	virtual ~ECS();

	// TODO just a naive test
	void add_system(SystemMethod *p_system);

	// TODO just a naive test
	void add_object(Object *p_object);

private:
	EntityIndex create_new_entity_id();

	void ecs_init();
};

template <class C>
void ECS::register_component() {
	StringName component_name = C::get_class_static();
	ERR_FAIL_COND_MSG(components.has(component_name), "This component is already registered.");
	C::init_storage();
	C::_bind_properties();
	// At this point the storage can't be nullptr.
	CRASH_COND(C::get_storage() == nullptr);
	components.insert(component_name, C::get_storage());
}

template <class C>
void ECS::unregister_component() {
	StringName component_name = C::get_class_static();
	ERR_FAIL_COND_MSG(components.has(component_name) == false, "This component is not registered.");
	components.remove(component_name);
	C::clear_properties_static();
	C::destroy_storage();
	// At this point the storage is always nullptr.
	CRASH_COND(C::get_storage() != nullptr);
}

#endif