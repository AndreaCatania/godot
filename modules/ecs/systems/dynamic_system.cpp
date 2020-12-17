#include "dynamic_system.h"

godex::DynamicSystemInfo::DynamicSystemInfo() {
}

godex::DynamicSystemInfo::add_resource(uint32_t p_resource_id, bool p_mutable) {
	const uint32_t index = resource_element_map.size() + query_element_map.size();
	resource_element_map.push_back(index);
	resources.push_back({ p_resource_id, p_mutable });
}

godex::DynamicSystemInfo::add_component(uint32_t p_component_id, bool p_mutable) {
	const uint32_t index = resource_element_map.size() + query_element_map.size();
	query_element_map.push_back(index);
	query.add_component(p_component_id, p_mutable);
}
