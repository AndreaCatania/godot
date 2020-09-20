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

enum class StorageType {
	DenseVector,
};

// TODO move this elsewhere.
// TODO just a naive test
struct SystemMethod {
	virtual ~SystemMethod();
	virtual void execute(Vector<Object *> &p_objects) = 0;
};

struct EntityIndex {
	const uint32_t index;

	EntityIndex() :
			index(UINT32_MAX) {}
	EntityIndex(uint32_t p_index) :
			index(p_index) {}

	bool is_null() const {
		return index == UINT32_MAX;
	}

	bool operator==(const EntityIndex &p_other) {
		return p_other.index == index;
	}
};

class ECS : public Object {
	GDCLASS(ECS, Object);

	friend class Entity;

	static ECS *singleton;

	static LocalVector<StringName> components;

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
	components.push_back(C::get_class_static());
	C::init_storage();
	C::_bind_properties();
	// At this point the storage can't be nullptr.
	CRASH_COND(C::get_storage() == nullptr);
}

template <class C>
void ECS::unregister_component() {
	const int64_t index = components.find(C::get_class_static());
	ERR_FAIL_COND_MSG(index == -1, "The component " + C::get_class_static() + " is not registered.");
	components.remove(index);
	C::clear_properties_static();
	C::destroy_storage();
	// At this point the storage is always nullptr.
	CRASH_COND(C::get_storage() != nullptr);
}

#endif