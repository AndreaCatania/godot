#ifndef TEST_ECS_BASE_H
#define TEST_ECS_BASE_H

#include "tests/test_macros.h"

#include "modules/ecs/components/mesh_component.h"
#include "modules/ecs/components/transform_component.h"
#include "modules/ecs/ecs.h"

namespace godex_tests {

TEST_CASE("[Modules][ECS] Test ECS singleton validity.") {
	CHECK(ECS::get_singleton() != nullptr);
}

TEST_CASE("[Modules][ECS] Test ECS Component ID validity.") {
	// Make sure the component IDs are properly registerd.
	CHECK(String(ECS::get_component_name(TransformComponent::get_component_id())) == TransformComponent::get_class_static());
	CHECK(String(ECS::get_component_name(MeshComponent::get_component_id())) == MeshComponent::get_class_static());
}

} // namespace godex_tests

#endif // TEST_ECS_BASE_H
