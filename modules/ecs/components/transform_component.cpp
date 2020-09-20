#include "transform_component.h"

/* Author: AndreaCatania */

TransformComponent::TransformComponent() {
}

void TransformComponent::set_transform(Transform &p_transform) {
	transform = p_transform;
}

const Transform &TransformComponent::get_transform() const {
	return transform;
}

void TransformComponent::_bind_properties() {
	add_property(PropertyInfo(Variant::TRANSFORM, "transform"), "set_transform", "get_transform");
}
