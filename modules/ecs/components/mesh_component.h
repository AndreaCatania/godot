/* Author: AndreaCatania */

#ifndef MESH_COMPONENT_H
#define MESH_COMPONENT_H

#include "modules/ecs/component.h"

class MeshComponent : public Component {
	COMPONENT(MeshComponent, DenseVector)

public:
	MeshComponent();

protected:
	static void _bind_params();
};

#endif