/** @author AndreaCatania */

#pragma once

#include "core/templates/local_vector.h"
#include "modules/ecs/iterators/dynamic_query.h"

class World;

namespace godex {

class DynamicSystemInfo {
	struct DResource {
		uint32_t resource_id;
		bool mutability;
	};

	/// Map used to map the list of Resources to the script.
	LocalVector<uint32_t> resource_element_map;
	/// Map used to map the list of Components to the script.
	LocalVector<uint32_t> query_element_map;
	LocalVector<DResource> resources;
	DynamicQuery query;

public:
	DynamicSystemInfo();

	void add_resource(uint32_t p_resource_id, bool p_mutable);
	void add_component(uint32_t p_component_id, bool p_mutable);
};

static inline DynamicQuery dynamic_system_query;
void dynamic_system(World *p_world) {
}

} // namespace godex
