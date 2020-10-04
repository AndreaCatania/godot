
/* Author: AndreaCatania */

#include "ecs_world.h"

void ECSWorld::_bind_methods() {
}

void ECSWorld::_notification(int p_what) {
	// TODO this is just a test, because the pipeline will be dispatched into
	// the Main.
	switch (p_what) {
		case NOTIFICATION_INTERNAL_PROCESS: {
			pipeline.dispatch();
		} break;
		case NOTIFICATION_READY:
			set_process_internal(true);
			break;
	}
}

#include "modules/ecs/components/transform_component.h" // TODO remove
#include "modules/ecs/iterators/query.h" // TODO remove
#include "modules/ecs/systems/system_builder.h" // TODO remove
// TODO just a test
void transform_system(Query<TransformComponent> &p_query) {
	while (p_query.has_next()) {
		auto [transform] = p_query.get();
		transform.set_transform(Transform(Basis(), transform.get_transform().origin + Vector3(10.0, 0, 0)));
		p_query.next_entity();
	}
}

ECSWorld::ECSWorld() {
	pipeline.add_system(transform_system);
}

ECSWorld::~ECSWorld() {
}

Pipeline &ECSWorld::get_pipeline() {
	return pipeline;
}

const Pipeline &ECSWorld::get_pipeline() const {
	return pipeline;
}