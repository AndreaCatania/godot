#pragma once

/* Author: AndreaCatania */

#include "modules/ecs/pipeline.h"
#include "scene/main/node.h"

class WorldECS : public Node {
	GDCLASS(WorldECS, Node)

	Pipeline *pipeline = nullptr;

protected:
	static void _bind_methods();

public:
	WorldECS();
	virtual ~WorldECS();

	void _notification(int p_what);

	String get_configuration_warning() const override;

private:
	void active_pipeline();
	void unactive_pipeline();
};
