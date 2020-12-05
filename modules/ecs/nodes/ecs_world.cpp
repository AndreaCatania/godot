
/* Author: AndreaCatania */

#include "ecs_world.h"

#include "modules/ecs/ecs.h"

void WorldECS::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_set_pipeline_system_links"), &WorldECS::set_pipeline_system_links);
	ClassDB::bind_method(D_METHOD("_get_pipeline_system_links"), &WorldECS::get_pipeline_system_links);
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "_pipeline_system_links"), "_set_pipeline_system_links", "_get_pipeline_system_links");
}

void WorldECS::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY:
			add_to_group("_world_ecs");
			if (Engine::get_singleton()->is_editor_hint() == false) {
				active_pipeline();
			}
			break;
		case NOTIFICATION_EXIT_TREE:
			if (Engine::get_singleton()->is_editor_hint() == false) {
				unactive_pipeline();
			}
			remove_from_group("_world_ecs");
			break;
	}
}

WorldECS::WorldECS() {
	pipeline = memnew(Pipeline);
}

WorldECS::~WorldECS() {
	memdelete(pipeline);
	pipeline = nullptr;
}

Pipeline *WorldECS::get_pipeline() const {
	ERR_FAIL_COND_V_MSG(is_active, nullptr, "This World is active, so you can manipulate the pipeline through `ECS::get_singleton()->get_commands()`.");
	return pipeline;
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

uint32_t WorldECS::get_systems_count() const {
	return pipeline_system_links.size();
}

void WorldECS::insert_system(const String &p_system_link, uint32_t p_pos) {
	if (p_system_link.find("::") >= 0) {
		// This is a script system.
		const Vector<String> ssd = p_system_link.split("::");
		ERR_FAIL_COND_MSG(ssd.size() != 2, "This system link is malformed.");
	} else {
		// This is a Native system.
		ERR_FAIL_COND_MSG(ECS::find_system_id(p_system_link) == UINT32_MAX, "This system is not known.");
	}

	// At this point the p_system_link is valid.

	// Make sure to remove any previously declared link.
	pipeline_system_links.erase(p_system_link);

	if (p_pos == UINT32_MAX) {
		pipeline_system_links.push_back(p_system_link);
	} else {
		ERR_FAIL_INDEX_MSG(p_pos, pipeline_system_links.size() + 1, "The pipeline is not so big, this system: " + p_system_link + " can't be insert at this position: " + itos(p_pos));
		// Insert the system at given position.
		pipeline_system_links.insert(p_pos, p_system_link);
	}
}

void WorldECS::set_pipeline_system_links(Array p_links) {
	pipeline_system_links = p_links;
}

Array WorldECS::get_pipeline_system_links() const {
	return pipeline_system_links;
}

void WorldECS::build_pipeline() {
	// TODO add a way to retrigger pipeline rebuild from GDScript.
}

void WorldECS::active_pipeline() {
	if (ECS::get_singleton()->has_active_pipeline() == false) {
		ECS::get_singleton()->set_active_pipeline(pipeline);
		is_active = true;

		if (ECS::get_singleton()->is_connected(
					"pipeline_unloaded",
					callable_mp(this, &WorldECS::active_pipeline))) {
			// Disconnects to `pipeline_unload`: previously connected because
			// there was already another pipeline connected.
			ECS::get_singleton()->disconnect(
					"pipeline_unloaded",
					callable_mp(this, &WorldECS::active_pipeline));
		}
	} else {
		// Connects to `pipeline_unload`: so when the current one is unloaded
		// this one can be loaded.
		if (ECS::get_singleton()->is_connected(
					"pipeline_unloaded",
					callable_mp(this, &WorldECS::active_pipeline)) == false) {
			ECS::get_singleton()->connect("pipeline_unloaded", callable_mp(this, &WorldECS::active_pipeline));
		}
		ERR_FAIL_MSG("Only one WorldECS is allowed at a time.");
	}
}

void WorldECS::unactive_pipeline() {
	// Disconnects to the eventual connected pipeline_unload.
	if (ECS::get_singleton()->is_connected(
				"pipeline_unloaded",
				callable_mp(this, &WorldECS::active_pipeline)) == false) {
		ECS::get_singleton()->connect("pipeline_unloaded", callable_mp(this, &WorldECS::active_pipeline));
	}

	if (is_active) {
		is_active = false;
		ECS::get_singleton()->set_active_pipeline(nullptr);
	}
}
