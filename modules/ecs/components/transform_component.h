/* Author: AndreaCatania */

#ifndef TRANSFOMR_COMPONENT_H
#define TRANSFOMR_COMPONENT_H

#include "modules/ecs/component.h"

class TransformComponent : public Component {
	COMPONENT(TransformComponent, DenseVector)

	Transform transform;

public:
	TransformComponent();
	TransformComponent(const Transform &p_transform);

	void set_transform(const Transform &p_transform);
	const Transform &get_transform() const;

protected:
	static void _bind_properties();
};

#endif
