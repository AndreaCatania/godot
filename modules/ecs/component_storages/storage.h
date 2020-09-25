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
	virtual void remove(EntityIndex p_index) {}
};

#endif