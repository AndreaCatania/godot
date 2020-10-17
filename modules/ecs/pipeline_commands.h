#pragma once

/**
	@author AndreaCatania
*/

#include "ecs_types.h"
#include "pipeline.h"

/// Via this class is possible to spawn or destroy entities, add or remove
/// components.
// TODO make this also a resource so it can be queried
// TODO make this also a GDScript object so it can be also used to query.
class PipelineCommands {
	friend class ECS;

	Pipeline *pipeline = nullptr;

public:
	/// Creates a new Entity id and returns an `EntityBuilder`.
	/// You can use the `EntityBuilder` in this way:
	/// ```
	///	Pipeline pipeline;
	///
	///	pipeline.create_entity()
	///			.with(TransformComponent())
	///			.with(MeshComponent());
	/// ```
	///
	/// It's possible to get the `EntityID` just by assining the `EntityBuilder`
	/// to an `EntityID`.
	/// ```
	///	EntityID entity = pipeline.create_entity()
	///			.with(MeshComponent());
	/// ```
	///
	/// Note: The `EntityBuilder` reference points to an internal variable.
	/// It's undefined behavior use it in any other way than the above one.
	const EntityBuilder &create_entity();

	/// Remove the entity from this Pipeline.
	void destroy_entity(EntityID p_entity);

	/// Adds a new component (or sets the default if already exists) to a
	/// specific Entity.
	template <class C>
	void add_component(EntityID p_entity, const C &p_data);

	/// Adds a new component using the name of the component and Variant data to
	/// initialize it. Usually this function is used to initialize the component
	/// from GDScript data.
	void add_component(EntityID p_entity, StringName p_component_name, const Variant &p_data);
};

template <class C>
void PipelineCommands::add_component(EntityID p_entity, const C &p_data) {
	pipeline->add_component<C>(p_entity, p_data);
}
