#ifndef TEST_ECS_QUERY_H
#define TEST_ECS_QUERY_H

#include "tests/test_macros.h"

#include "modules/ecs/components/dynamic_component.h"
#include "modules/ecs/components/transform_component.h"
#include "modules/ecs/ecs.h"
#include "modules/ecs/iterators/dynamic_query.h"
#include "modules/ecs/world/world.h"

class TagQueryTestComponent : public godex::Component {
	COMPONENT(TagQueryTestComponent, DenseVector)
};

namespace godex_tests {

TEST_CASE("[Modules][ECS] Test dynamic query") {
	ECS::register_component<TagQueryTestComponent>();

	World world;

	EntityID entity_1 = world
								.create_entity()
								.with(TransformComponent())
								.with(TagQueryTestComponent());

	EntityID entity_2 = world
								.create_entity()
								.with(TransformComponent());

	EntityID entity_3 = world
								.create_entity()
								.with(TransformComponent())
								.with(TagQueryTestComponent());

	{
		godex::DynamicQuery query;
		query.add_component("TransformComponent", true);
		query.add_component("TagQueryTestComponent", false);

		CHECK(query.is_valid());

		// Test process.
		query.begin(&world);

		// Entity 1
		CHECK(query.is_done() == false);
		CHECK(query.get_current_entity_id() == entity_1);

		{
			const LocalVector<godex::AccessComponent> *data = query.get();
			CHECK(data->size() == 2);
			CHECK((*data)[0].is_mutable());
			CHECK((*data)[1].is_mutable() == false);
			(*data)[0].set("transform", Transform(Basis(), Vector3(100.0, 100.0, 100.0)));
		}

		query.next_entity();

		// Entity 3 (Entity 2 is skipped because it doesn't fulfil the query)
		CHECK(query.is_done() == false);
		CHECK(query.get_current_entity_id() == entity_3);

		{
			const LocalVector<godex::AccessComponent> *data = query.get();
			CHECK(data->size() == 2);
			CHECK((*data)[0].is_mutable());
			CHECK((*data)[1].is_mutable() == false);
			(*data)[0].set("transform", Transform(Basis(), Vector3(200.0, 200.0, 200.0)));
		}

		query.next_entity();

		// Nothing more to do at this point.
		CHECK(query.is_done());
		query.end();
	}

	{
		godex::DynamicQuery query;
		query.add_component("TransformComponent", false);

		// Test process.
		query.begin(&world);

		// Entity 1
		CHECK(query.is_done() == false);
		CHECK(query.get_current_entity_id() == entity_1);

		{
			const LocalVector<godex::AccessComponent> *data = query.get();
			CHECK(data->size() == 1);
			CHECK((*data)[0].is_mutable() == false);
			const Transform t = (*data)[0].get("transform");
			// Check if the entity_1 is changed.
			CHECK(t.origin.x - 100.0 <= CMP_EPSILON);
		}

		query.next_entity();

		// Entity 2
		CHECK(query.is_done() == false);
		CHECK(query.get_current_entity_id() == entity_2);
		{
			const LocalVector<godex::AccessComponent> *data = query.get();
			CHECK(data->size() == 1);
			CHECK((*data)[0].is_mutable() == false);
			const Transform t = (*data)[0].get("transform");
			// Make sure the entity_2 is not changed.
			CHECK(t.origin.x <= CMP_EPSILON);
		}

		query.next_entity();

		// Entity 3
		CHECK(query.is_done() == false);
		CHECK(query.get_current_entity_id() == entity_3);
		{
			const LocalVector<godex::AccessComponent> *data = query.get();
			CHECK(data->size() == 1);
			CHECK((*data)[0].is_mutable() == false);
			const Transform t = (*data)[0].get("transform");
			// Make sure the entity_3 is changed.
			CHECK(t.origin.x - 200.0 <= CMP_EPSILON);
		}

		query.next_entity();

		CHECK(query.is_done());
		query.end();
	}
}

TEST_CASE("[Modules][ECS] Test invalid dynamic query.") {
	godex::DynamicQuery query;

	// Add an invalid component.
	query.add_component("ThisComponentDoesntExists");

	CHECK(query.is_valid() == false);

	// Reset the query.
	query.reset();

	// Build it again but this time valid.
	query.add_component("TransformComponent");
	CHECK(query.is_valid());
}

} // namespace godex_tests

#endif // TEST_ECS_QUERY_H
