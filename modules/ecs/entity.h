#pragma once

/* Author: AndreaCatania */

#include "component.h"
#include "core/local_vector.h"
#include "scene/main/node.h"

class Entity : public Node {
	GDCLASS(Entity, Node);

	EntityID index;
	LocalVector<Component> components;

protected:
	static void _bind_methods();

public:
	Entity();
	virtual ~Entity();

	void add_component(StringName p_component_name);
};
