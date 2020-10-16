#pragma once

/* Author: AndreaCatania */

#include "modules/ecs/pipeline.h"
#include "scene/main/node.h"

class WorldECS : public Node {
	GDCLASS(WorldECS, Node)

	Pipeline *pipeline = nullptr;
	bool is_active = false;

protected:
	static void _bind_methods();

public:
	WorldECS();
	virtual ~WorldECS();

	void _notification(int p_what);

	/// Returns the pipeline only if this is not an active world.
	/// If this is an active world and you need to interact with the pipeline is
	/// possible to do it via the commands object that you can take using:
	/// `ECS::get_singleton()->get_commands()`
	Pipeline *get_pipeline() const;

	String get_configuration_warning() const override;

private:
	void active_pipeline();
	void unactive_pipeline();
};
