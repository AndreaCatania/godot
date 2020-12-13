#ifndef TEST_ECS_SYSTEM_H
#define TEST_ECS_SYSTEM_H

#include "tests/test_macros.h"

#include "modules/ecs/components/transform_component.h"
#include "modules/ecs/components/variant_component.h"
#include "modules/ecs/ecs.h"
#include "modules/ecs/pipeline/pipeline.h"
#include "modules/ecs/world/world.h"

class TagTestComponent : public godex::Component {
	COMPONENT(TagTestComponent, DenseVector)
};

namespace godex_tests {

void test_system_tag(Query<TransformComponent, const TagTestComponent> &p_query) {
	while (p_query.is_done() == false) {
		auto [transform, component] = p_query.get();
		transform.transform.origin.x += 100.0;
		p_query.next_entity();
	}
}

TEST_CASE("[Modules][ECS] Test system and query") {
	ECS::register_component<TagTestComponent>();

	World world;

	EntityID entity_1 = world
								.create_entity()
								.with(TransformComponent())
								.with(TagTestComponent());

	EntityID entity_2 = world
								.create_entity()
								.with(TransformComponent());

	EntityID entity_3 = world
								.create_entity()
								.with(TransformComponent())
								.with(TagTestComponent());

	Pipeline pipeline;
	pipeline.add_system(test_system_tag);

	for (uint32_t i = 0; i < 3; i += 1) {
		pipeline.dispatch(&world);
	}

	world.get_storage<TransformComponent>();

	const TypedStorage<const TransformComponent> *storage = world.get_storage<const TransformComponent>();

	const Vector3 entity_1_origin = storage->get(entity_1).get_transform().origin;
	const Vector3 entity_2_origin = storage->get(entity_2).get_transform().origin;
	const Vector3 entity_3_origin = storage->get(entity_3).get_transform().origin;

	// This entity is expected to change.
	CHECK(ABS(entity_1_origin.x - 300.0) <= CMP_EPSILON);

	// This entity doesn't have a `TagTestComponent` so the systems should not
	// change it.
	CHECK(entity_2_origin.x <= CMP_EPSILON);

	// This entity is expected to change.
	CHECK(ABS(entity_3_origin.x - 300.0) <= CMP_EPSILON);
}

TEST_CASE("[Modules][ECS] Test system and resource") {
	// TODO
}

//void test_system_script(Query<get_type("TransformComponent")> &p_query) {
//	while (p_query.is_done() == false) {
//		auto [transform] = p_query.get();
//		transform.transform.origin.x += 100.0;
//		p_query.next_entity();
//	}
//}

TEST_CASE("[Modules][ECS] Test dynamic system") {
	ECS::register_component<TagTestComponent>();

	World world;

	EntityID entity_1 = world
								.create_entity()
								.with(TransformComponent())
								.with(TagTestComponent());

	EntityID entity_2 = world
								.create_entity()
								.with(TransformComponent());

	EntityID entity_3 = world
								.create_entity()
								.with(TransformComponent())
								.with(TagTestComponent());

	// TODO use the dynamic system instead

	Pipeline pipeline;
	pipeline.add_system(test_system_tag);

	for (uint32_t i = 0; i < 3; i += 1) {
		pipeline.dispatch(&world);
	}

	world.get_storage<TransformComponent>();

	const TypedStorage<const TransformComponent> *storage = world.get_storage<const TransformComponent>();

	const Vector3 entity_1_origin = storage->get(entity_1).get_transform().origin;
	const Vector3 entity_2_origin = storage->get(entity_2).get_transform().origin;
	const Vector3 entity_3_origin = storage->get(entity_3).get_transform().origin;

	// This entity is expected to change.
	CHECK(ABS(entity_1_origin.x - 300.0) <= CMP_EPSILON);

	// This entity doesn't have a `TagTestComponent` so the systems should not
	// change it.
	CHECK(entity_2_origin.x <= CMP_EPSILON);

	// This entity is expected to change.
	CHECK(ABS(entity_3_origin.x - 300.0) <= CMP_EPSILON);
}

} // namespace godex_tests

#endif // TEST_ECS_SYSTEM_H
