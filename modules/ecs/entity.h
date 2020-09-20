/* Author: AndreaCatania */

#ifndef ENTITY_H
#define ENTITY_H

#include "component.h"
#include "core/local_vector.h"
#include "scene/main/node.h"

class Entity : public Node {
	GDCLASS(Entity, Node);

	LocalVector<Component> components;

public:
	Entity();
	virtual ~Entity();

protected:
	static void _bind_methods();
};

#endif