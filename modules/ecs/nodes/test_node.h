#pragma once

#include "scene/3d/node_3d.h"

class TestNode : public Node3D {
	GDCLASS(TestNode, Node3D);

	Transform my_transform;

public:
	TestNode();
	virtual ~TestNode();

	void _notification(int p_what);
};
