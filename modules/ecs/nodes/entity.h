#pragma once

/* Author: AndreaCatania */

#include "core/local_vector.h"
#include "modules/ecs/component.h"
#include "scene/main/node.h"

class ECSWorld;

class Entity : public Node {
	GDCLASS(Entity, Node);

	EntityID entity_id;
	Dictionary components_data;

	ECSWorld *ecs_world = nullptr;

protected:
	static void _bind_methods();
	void set_components_data(Dictionary p_data);

public:
	Entity();
	virtual ~Entity();

	void _notification(int p_what);

	void add_component_data(StringName p_component_name);
	void remove_component_data(StringName p_component_name);
	const Dictionary &get_components_data() const;

private:
	void update_world();
	void update_component_data();
};
