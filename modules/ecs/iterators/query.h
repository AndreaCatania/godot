/**
	@author AndreaCatania
*/

#ifndef QUERY_H
#define QUERY_H

#include "modules/ecs/pipeline.h"
#include "modules/ecs/storages/storage.h"
#include <tuple>

template <class... Cs>
class QueryStorage {
public:
	QueryStorage(Pipeline *p_pipeline) {}

	bool has_data(EntityID p_entity) const { return true; }
	std::tuple<Cs &...> get() const { return std::tuple(); }
};

template <class C, class... Cs>
class QueryStorage<C, Cs...> : QueryStorage<Cs...> {
	TypedStorage<C> *storage = nullptr;

public:
	QueryStorage(Pipeline *p_pipeline) :
			QueryStorage<Cs...>(p_pipeline) {
		storage = p_pipeline->get_storage<C>();
		ERR_FAIL_COND_MSG(storage == nullptr, "The storage" + String(typeid(TypedStorage<C>).name()) + " is null.");
	}

	bool has_data(EntityID p_entity) const {
		if (storage == nullptr) {
			return false;
		}
		return storage->has(p_entity) && QueryStorage<Cs...>::has_data(p_entity);
	}

	std::tuple<C &, Cs &...> get() const {
#ifdef DEBUG_ENABLED
		// This can't happen because `is_done` returns true.
		CRASH_COND_MSG(storage == nullptr, "The storage" + String(typeid(TypedStorage<C>).name()) + " is null.");
#endif

		// TODO integrate this.
		C &c = storage->get(EntityID(0));

		return std::tuple_cat(std::tuple<C &>(c), QueryStorage<Cs...>::get());
	}
};

// TODO The lockup mechanism of this query must be improved to avoid any extra
// reduntant for and avoid any cache miss.

template <class... Cs>
class Query {
	Pipeline *pipeline;
	uint32_t id = 0;
	QueryStorage<Cs...> q;

public:
	Query(Pipeline *p_pipeline) :
			pipeline(p_pipeline), q(p_pipeline) {
		if (q.has_data(0) == false) {
			next_entity();
		}
	}

	bool is_done() const {
		return id == UINT32_MAX;
	}

	bool has_next() const {
		return id != UINT32_MAX;
	}

	void operator+=(uint32_t p_i) {
		for (uint32_t i = 0; i < p_i; i += 1) {
			next_entity();
			if (is_done()) {
				break;
			}
		}
	}

	void next_entity() {
		const uint32_t last_id = pipeline->get_last_entity_id();
		if (unlikely(id == UINT32_MAX || last_id == UINT32_MAX)) {
			return;
		}

		for (uint32_t i = id + 1; i <= last_id; i += 1) {
			if (q.has_data(i)) {
				id = i;
				return;
			}
		}

		id = UINT32_MAX;
	}

	std::tuple<Cs &...> get() const {
		return q.get();
	}
};

#endif
