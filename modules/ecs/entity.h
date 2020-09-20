/* Author: AndreaCatania */

#ifndef ENTITY_H
#define ENTITY_H

#include "component.h"
#include "core/local_vector.h"
#include "scene/main/node.h"

class Entity : public Node {
	GDCLASS(Entity, Node);

	EntityIndex index;
	LocalVector<Component> components;

protected:
	static void _bind_methods();

public:
	Entity();
	virtual ~Entity();

	void add_component(StringName p_component_name);
};

#endif