/**
	@author AndreaCatania
*/

#ifndef QUERY_H
#define QUERY_H

#include "core/local_vector.h"
#include "modules/ecs/pipeline.h"
#include "modules/ecs/storages/storage.h"
#include <tuple>

template <class... Cs>
class Query {
public:
	Query(Pipeline *p_pipeline) {}

	std::tuple<Cs &...> get() const { return std::tuple(); }
};

template <class C, class... Cs>
class Query<C, Cs...> : Query<Cs...> {
	uint32_t index = 0;
	Pipeline *pipeline;
	TypedStorage<C> *storage = nullptr;

public:
	Query(Pipeline *p_pipeline) :
			Query<Cs...>(p_pipeline), pipeline(p_pipeline) {
		storage = pipeline->get_storage<C>();
		ERR_FAIL_COND_MSG(storage == nullptr, "The storage" + String(typeid(TypedStorage<C>).name()) + " is null.");
	}

	bool is_done() const {
		return false;
	}

	void operator+=(uint32_t p_i) {
		index += p_i;
	}

	std::tuple<C &, Cs &...> get() const {
#ifdef DEBUG_ENABLED
		// This can't happen because `is_done` returns true.
		CRASH_COND_MSG(storage == nullptr, "The storage" + String(typeid(TypedStorage<C>).name()) + " is null.");
#endif

		// TODO just a test.
		C &c = storage->get(EntityID(0));

		return std::tuple_cat(std::tuple<C &>(c), Query<Cs...>::get());
	}
};

#endif