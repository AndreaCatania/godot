#pragma once

#include "core/string/string_name.h"
#include "core/templates/local_vector.h"
#include "modules/ecs/components/component.h"

class World;

namespace godex {

class AccessComponent {
	friend class DynamicQuery;

	godex::Component *component = nullptr;
	bool mut;

public:
	AccessComponent(bool p_mut = false);

	void set(StringName p_name, Variant p_data) const;
	Variant get(StringName p_name) const;

	bool is_mutable() const;
};

/// This query is slower compared to `Query` but can be builded at runtime, so
/// that the scripts can still interact with the `World`.
/// Cache this query allow to save the time needed to lockup the components IDs,
/// so it's advised store it and use when needed.
class DynamicQuery {
	bool valid = true;
	bool can_change = true;
	LocalVector<uint32_t> storage_ids;
	LocalVector<AccessComponent> access_component;
	LocalVector<Storage *> storages;

	World *world = nullptr;
	uint32_t entity_id = UINT32_MAX;

public:
	DynamicQuery();

	/// Add component.
	void add_component(StringName p_component, bool p_mutable = false);

	/// Returns true if this query is valid.
	bool is_valid() const;

	/// Clear the query so this memory can be reused.
	void reset();

	/// Start the execution of this query.
	void begin(World *p_world);

	/// Returns `false` if this query can still return the components via `get`.
	bool is_done() const;

	/// Returns the components.
	/// IMPORTANT: Store this pointer is unsafe and may cause crash; the pointers
	/// are valid only for the duration of the query process.
	const LocalVector<AccessComponent> *get();

	/// Returns entity id.
	EntityID get_current_entity_id() const;

	/// Advance entity
	void next_entity();

	/// Ends the query execution.
	void end();

private:
	bool has_entity(EntityID p_id) const;
};

} // namespace godex
