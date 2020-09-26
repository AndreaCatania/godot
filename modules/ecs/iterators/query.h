/**
	@author AndreaCatania
*/

#ifndef QUERY_H
#define QUERY_H

#include "core/local_vector.h"
#include "modules/ecs/storages/storage.h"

template <typename... ARGS>
class Query {
	static const uint32_t storages_count = sizeof...(ARGS);

	uint32_t index = 0;

public:
	Query() {
	}

	uint32_t get_storages_count() const {
		return storages_count;
	}

	bool has_next() const {
		return true;
	}

	void operator+=(uint32_t p_i) {
		index += p_i;
	}

	//template <typename HEAD, typename... TAIL>
	//void get(const TypedStorage<HEAD> *p_storage, const TypedStorage<TAIL> *... p_tail) const {
	//	// TODO return a tuple.
	//	print_line(p_storage->get_type_name());
	//	get(p_tail...);
	//}
};

#endif