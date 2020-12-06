
/* Author: AndreaCatania */

#include "ecs_world.h"

#include "modules/ecs/ecs.h"
#include "modules/ecs/pipeline/pipeline.h"
#include "modules/ecs/world/world.h"

void PipelineECS::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_pipeline_name", "name"), &PipelineECS::set_pipeline_name);
	ClassDB::bind_method(D_METHOD("get_pipeline_name"), &PipelineECS::get_pipeline_name);
	ClassDB::bind_method(D_METHOD("set_system_links", "system_links"), &PipelineECS::set_system_links);
	ClassDB::bind_method(D_METHOD("get_system_links"), &PipelineECS::get_system_links);

	ClassDB::bind_method(D_METHOD("insert_system", "system_link", "position"), &PipelineECS::insert_system, DEFVAL(-1));

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "pipeline_name"), "set_pipeline_name", "get_pipeline_name");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "system_links"), "set_system_links", "get_system_links");
}

void PipelineECS::set_pipeline_name(StringName p_name) {
	pipeline_name = p_name;
	_change_notify("pipeline_name");
}

StringName PipelineECS::get_pipeline_name() const {
	return pipeline_name;
}

void PipelineECS::set_system_links(Array p_system_links) {
	system_links = p_system_links;
	_change_notify("system_links");
}

Array PipelineECS::get_system_links() const {
	return system_links;
}

void PipelineECS::insert_system(const String &p_system_link, uint32_t p_pos) {
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
	system_links.erase(p_system_link);

	if (p_pos == UINT32_MAX) {
		system_links.push_back(p_system_link);
	} else {
		ERR_FAIL_INDEX_MSG(int(p_pos), system_links.size() + 1, "The pipeline is not so big, this system: " + p_system_link + " can't be insert at this position: " + itos(p_pos));
		// Insert the system at given position.
		system_links.insert(p_pos, p_system_link);
	}

	_change_notify("system_links");
}

void WorldECS::_bind_methods() {
	ClassDB::bind_method(D_METHOD("add_pipeline", "pipeline"), &WorldECS::add_pipeline);
	ClassDB::bind_method(D_METHOD("remove_pipeline", "pipeline"), &WorldECS::remove_pipeline);
}

bool WorldECS::_set(const StringName &p_name, const Variant &p_value) {
	Vector<String> split = String(p_name).split("/");
	ERR_FAIL_COND_V_MSG(split.size() != 2, false, "This variable name is not recognized: " + p_name);

	const int index = find_pipeline_index(split[1]);

	Ref<PipelineECS> pip = p_value;
	if (pip.is_null()) {
		// Nothing to do.
		return false;
	}

	// Make sure the property name is the same.
	ERR_FAIL_COND_V(pip->get_pipeline_name() != split[1], false);

	if (index == -1) {
		pipelines.push_back(p_value);
	} else {
		pipelines.write[index] = p_value;
	}

	return true;
}

bool WorldECS::_get(const StringName &p_name, Variant &r_ret) const {
	Vector<String> split = String(p_name).split("/");
	ERR_FAIL_COND_V_MSG(split.size() != 2, false, "This variable name is not recognized: " + p_name);

	const int index = find_pipeline_index(split[1]);

	if (index == -1) {
		return false;
	} else {
		r_ret = pipelines[index];
		return true;
	}
}

void WorldECS::_get_property_list(List<PropertyInfo> *p_list) const {
	for (int i = 0; i < pipelines.size(); i += 1) {
		p_list->push_back(PropertyInfo(Variant::OBJECT, "pipelines/" + pipelines[i]->get_pipeline_name(), PROPERTY_HINT_RESOURCE_TYPE, "PipelineECS"));
	}
}

void WorldECS::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY:
			add_to_group("_world_ecs");
			if (Engine::get_singleton()->is_editor_hint() == false) {
				active_world();
			}
			break;
		case NOTIFICATION_EXIT_TREE:
			if (Engine::get_singleton()->is_editor_hint() == false) {
				unactive_world();
			}
			remove_from_group("_world_ecs");
			break;
	}
}

WorldECS::WorldECS() {
	world = memnew(World);
}

WorldECS::~WorldECS() {
	memdelete(world);
	world = nullptr;
}

World *WorldECS::get_world() const {
	ERR_FAIL_COND_V_MSG(is_active, nullptr, "This World is active, so you can manipulate the world through `ECS::get_singleton()->get_commands()`.");
	return world;
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

void WorldECS::set_pipelines(Vector<Ref<PipelineECS>> p_pipelines) {
	pipelines = p_pipelines;
}

const Vector<Ref<PipelineECS>> &WorldECS::get_pipelines() const {
	return pipelines;
}

Vector<Ref<PipelineECS>> &WorldECS::get_pipelines() {
	return pipelines;
}

void WorldECS::add_pipeline(Ref<PipelineECS> p_pipeline) {
	pipelines.push_back(p_pipeline);
	_change_notify("pipelines");
}

void WorldECS::remove_pipeline(Ref<PipelineECS> p_pipeline) {
	pipelines.erase(p_pipeline);
	_change_notify("pipelines");
}

Ref<PipelineECS> WorldECS::find_pipeline(StringName p_name) {
	const int index = find_pipeline_index(p_name);
	if (index == -1) {
		return Ref<PipelineECS>();
	} else {
		return pipelines[index];
	}
}

int WorldECS::find_pipeline_index(StringName p_name) const {
	for (int i = 0; i < pipelines.size(); i += 1) {
		if (pipelines[i]->get_pipeline_name() == p_name) {
			return i;
		}
	}
	return -1;
}

void WorldECS::active_world() {
	if (ECS::get_singleton()->has_active_world() == false) {
		ECS::get_singleton()->set_active_world(world);
		is_active = true;

		if (ECS::get_singleton()->is_connected(
					"world_unloaded",
					callable_mp(this, &WorldECS::active_world))) {
			// Disconnects to `world_unload`: previously connected because
			// there was already another world connected.
			ECS::get_singleton()->disconnect(
					"world_unloaded",
					callable_mp(this, &WorldECS::active_world));
		}
	} else {
		// Connects to `world_unload`: so when the current one is unloaded
		// this one can be loaded.
		if (ECS::get_singleton()->is_connected(
					"world_unloaded",
					callable_mp(this, &WorldECS::active_world)) == false) {
			ECS::get_singleton()->connect("world_unloaded", callable_mp(this, &WorldECS::active_world));
		}
		ERR_FAIL_MSG("Only one WorldECS is allowed at a time.");
	}
}

void WorldECS::unactive_world() {
	// Disconnects to the eventual connected world_unload.
	if (ECS::get_singleton()->is_connected(
				"world_unloaded",
				callable_mp(this, &WorldECS::active_world)) == false) {
		ECS::get_singleton()->connect("world_unloaded", callable_mp(this, &WorldECS::active_world));
	}

	if (is_active) {
		is_active = false;
		ECS::get_singleton()->set_active_world(nullptr);
	}
}
