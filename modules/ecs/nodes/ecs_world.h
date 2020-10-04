#pragma once

/* Author: AndreaCatania */

#include "modules/ecs/pipeline.h"
#include "scene/main/node.h"

class ECSWorld : public Node {
	GDCLASS(ECSWorld, Node);

	Pipeline pipeline;

protected:
	static void _bind_methods();

public:
	ECSWorld();
	virtual ~ECSWorld();

	void _notification(int p_what);

	Pipeline &get_pipeline();
	const Pipeline &get_pipeline() const;
};
