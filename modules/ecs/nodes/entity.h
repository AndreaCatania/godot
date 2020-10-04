#pragma once

/* Author: AndreaCatania */

#include "core/local_vector.h"
#include "modules/ecs/component.h"
#include "scene/main/node.h"

class ECSWorld;

class Entity : public Node {
	GDCLASS(Entity, Node);

	EntityID entity_id;
	LocalVector<Component> components;

	ECSWorld *ecs_world = nullptr;

protected:
	static void _bind_methods();

public:
	Entity();
	virtual ~Entity();

	void _notification(int p_what);
	void add_component(StringName p_component_name);

private:
	void update_world();
};
