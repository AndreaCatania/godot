#ifndef TEST_ECS_WORLD_H
#define TEST_ECS_WORLD_H

#include "tests/test_macros.h"

#include "modules/ecs/components/transform_component.h"
#include "modules/ecs/ecs.h"
#include "modules/ecs/world/world.h"

namespace godex_tests {

TEST_CASE("[Modules][ECS] Test world") {
	World world;

	TransformComponent entity_1_transform_component;
	entity_1_transform_component.set_transform(Transform(Basis(), Vector3(10.0, 10.0, 10.0)));

	EntityID entity_1 = world.create_entity();
	world.add_component(
			entity_1,
			entity_1_transform_component);

	const TypedStorage<const TransformComponent> *storage = world.get_storage<const TransformComponent>();
	const TransformComponent &transform_from_storage = storage->get(entity_1);

	// Check the add component has the exact same data as the stored one.
	CHECK((entity_1_transform_component.get_transform().origin - transform_from_storage.get_transform().origin).length() < CMP_EPSILON);
}

} // namespace godex_tests

#endif // TEST_ECS_WORLD_H
