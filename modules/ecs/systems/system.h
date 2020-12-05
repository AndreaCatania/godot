/** @author AndreaCatania */

#pragma once

#include "core/templates/local_vector.h"

class Pipeline;

typedef void (*system_execute)(Pipeline *p_pipeline);

struct SystemInfo {
	StringName name;
	String description;
	LocalVector<uint32_t> mutable_components;
	LocalVector<uint32_t> immutable_components;
	LocalVector<uint32_t> mutable_resources;
	LocalVector<uint32_t> immutable_resources;
	system_execute system_func = nullptr;
};

typedef SystemInfo (*get_system_info_func)();
