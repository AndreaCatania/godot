
/* Author: AndreaCatania */

#include "ecs_world.h"

#include "modules/ecs/ecs.h"

void WorldECS::_bind_methods() {
}

void WorldECS::_notification(int p_what) {
	// TODO this is just a test, because the pipeline will be dispatched into
	// the Main.
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE:
			add_to_group("_world_ecs");
			if (Engine::get_singleton()->is_editor_hint() == false) {
				active_pipeline();
			}
			break;
		case NOTIFICATION_EXIT_TREE:
			if (Engine::get_singleton()->is_editor_hint() == false) {
				unactive_pipeline();
			}
			add_to_group("_world_ecs");
			break;
	}
}

WorldECS::WorldECS() {
}

WorldECS::~WorldECS() {
}

String WorldECS::get_configuration_warning() const {
	String warning = Node::get_configuration_warning();

	if (!is_inside_tree()) {
		return warning;
	}

	List<Node *> nodes;
	get_tree()->get_nodes_in_group("_world_ecs", &nodes);

	if (nodes.size() > 1) {
		if (!warning.empty()) {
			warning += "\n\n";
		}
		warning += TTR("Only one WorldECS is allowed per scene (or set of instanced scenes).");
	}

	return warning;
}

void WorldECS::active_pipeline() {
	if (ECS::get_singleton()->has_active_pipeline()) {
		pipeline = memnew(Pipeline);
		ECS::get_singleton()->set_active_pipeline(pipeline);

		if (ECS::get_singleton()->is_connected(
					"pipeline_unloaded",
					callable_mp(this, &WorldECS::active_pipeline))) {
			ECS::get_singleton()->disconnect(
					"pipeline_unloaded",
					callable_mp(this, &WorldECS::active_pipeline));
		}
	} else {
		if (ECS::get_singleton()->is_connected(
					"pipeline_unloaded",
					callable_mp(this, &WorldECS::active_pipeline)) == false) {
			ECS::get_singleton()->connect("pipeline_unloaded", callable_mp(this, &WorldECS::active_pipeline));
		}
		ERR_FAIL_MSG("Only one WorldECS is allowed at a time.");
	}
}

void WorldECS::unactive_pipeline() {
	if (ECS::get_singleton()->is_connected(
				"pipeline_unloaded",
				callable_mp(this, &WorldECS::active_pipeline)) == false) {
		ECS::get_singleton()->connect("pipeline_unloaded", callable_mp(this, &WorldECS::active_pipeline));
	}

	if (pipeline != nullptr) {
		ECS::get_singleton()->set_active_pipeline(nullptr);
		memdelete(pipeline);
		pipeline = nullptr;
	}
}
