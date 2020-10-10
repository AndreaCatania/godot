





/* Author: AndreaCatania */

#ifndef STORAGE_H
#define STORAGE_H

#include "modules/ecs/ecs.h"

enum class StorageType {
	NONE,
	DENSE_VECTOR,
};

class Storage {
public:
	virtual StorageType get_type() const { return StorageType::NONE; }
	virtual String get_type_name() const { return "Overload this function `get_type_name()` please."; }
	virtual void remove(EntityID p_index) {}
};

template <class T>
class TypedStorage : public Storage {
public:
	virtual void insert(EntityID p_entity, T p_data) {
		CRASH_NOW_MSG("Override this function.");
	}

	virtual bool has(EntityID p_entity) const {
		CRASH_NOW_MSG("Override this function.");
		return false;
	}

#pragma warning( push )
#pragma warning( disable: 4172) // Disable warning for local address return.

	virtual const T &get(EntityID p_entity) const {
		CRASH_NOW_MSG("Override this function.");
		return T();
	}

	virtual T &get(EntityID p_entity) {
		CRASH_NOW_MSG("Override this function.");
		return T();
	}

#pragma warning( pop ) 
};

#endif
