#pragma once

#include "core/local_vector.h"

class Pipeline;

typedef void (*system_execute)(Pipeline *p_pipeline);

struct SystemInfo {
	LocalVector<uint32_t> mutable_components;
	LocalVector<uint32_t> immutable_components;
	system_execute system_func = nullptr;

	/// Creates a SystemInfo, extracting the information from a system function.
	//template<>
	static SystemInfo from_function();
};

typedef SystemInfo (*get_system_info_func)();

SystemInfo SystemInfo::from_function() {
	return SystemInfo();
}
