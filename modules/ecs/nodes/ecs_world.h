#pragma once

/* Author: AndreaCatania */

#include "modules/ecs/pipeline/pipeline.h"
#include "scene/main/node.h"

// TODO the World is the thing that contains the resources and the storages,
// though I need a way to allow fast pipeline swap while keeping the resources
// and storages.
// Maybe making the pipeline a world resource? Though, how the
// customization works?? Maybe we can keep the current editor and so customize
// the resource set.

class WorldECS : public Node {
	GDCLASS(WorldECS, Node)

	bool pipeline_build_in_progress = false;
	Pipeline *pipeline = nullptr;
	bool is_active = false;

	Array pipeline_system_links;

protected:
	static void _bind_methods();

public:
	WorldECS();
	virtual ~WorldECS();

	void _notification(int p_what);

	/// Returns the pipeline only if this is not an active world.
	/// If this is an active world and you need to interact with the pipeline is
	/// possible to do it via the commands object that you can take using:
	/// `ECS::get_singleton()->get_commands()`
	Pipeline *get_pipeline() const;

	String get_configuration_warning() const override;

	/// Get the stored systems count.
	uint32_t get_systems_count() const;

	/// Insert a new system into the world. This `System` is not immediately
	/// added to the pipeline. This function is mainly used by the editor to
	/// compose the pipeline.
	///
	/// @param `p_system_link` Can be a native system name (`TransformSystem`)
	///                        or a script system (`res://path/to/script.gd::system_func_name`)
	/// @param `p_pos` Sets the system at the given position, pushing the others.
	///                Passing `UINT32_MAX`, allow to push back the system.
	///
	/// If the `p_system_link` is already defined it's position is updated.
	///
	/// Note: Add and Remove a `system` will cause pipeline rebuild, which
	/// should be avoided by using more pipelines and activating them when
	/// needed.
	void insert_system(const String &p_system_link, uint32_t p_pos = UINT32_MAX);

	void set_pipeline_system_links(Array p_links);
	Array get_pipeline_system_links() const;

private:
	void build_pipeline();
	void active_pipeline();
	void unactive_pipeline();
};
