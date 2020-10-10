/* Author: AndreaCatania */

#ifndef DENSE_VECTOR_H
#define DENSE_VECTOR_H

#include "core/local_vector.h"
#include "modules/ecs/ecs.h"
#include "storage.h"

/// Dense vector storage.
/// Has a redirection 2-way table between entities and
/// components (vice versa), allowing to leave no gaps within the data.
/// The the entity indices are stored sparsely.
template <class T>
class DenseVector : public TypedStorage<T> {
	LocalVector<T> data;
	LocalVector<EntityID> data_to_entity;
	// Each position of this vector is an Entity Index.
	LocalVector<uint32_t> entity_to_data;

public:
	virtual StorageType get_type() const override;
	virtual String get_type_name() const override;

	virtual void insert(EntityID p_entity, T p_data) override;
	virtual bool has(EntityID p_entity) const override;
	virtual const T &get(EntityID p_entity) const override;
	virtual T &get(EntityID p_entity) override;
	virtual void remove(EntityID p_entity) override;
};

template <class T>
StorageType DenseVector<T>::get_type() const {
	return StorageType::DENSE_VECTOR;
}

template <class T>
String DenseVector<T>::get_type_name() const {
	return "DenseVector[" + String(typeid(T).name()) + "]";
}

template <class T>
void DenseVector<T>::insert(EntityID p_entity, T p_data) {
	if (entity_to_data.size() <= p_entity) {
		const uint32_t start = entity_to_data.size();
		// Resize the vector so to fit this new entity.
		entity_to_data.resize(p_entity + 1);
		for (uint32_t i = start; i < entity_to_data.size(); i += 1) {
			entity_to_data[i] = UINT32_MAX;
		}
	}

	// Store the data-index
	entity_to_data[p_entity] = data.size();

	// Store the data
	data.push_back(p_data);
	data_to_entity.push_back(p_entity);
}

template <class T>
bool DenseVector<T>::has(EntityID p_entity) const {
	return p_entity < entity_to_data.size() && entity_to_data[p_entity] != UINT32_MAX;
}

template <class T>
const T &DenseVector<T>::get(EntityID p_entity) const {
	CRASH_COND_MSG(has(p_entity) == false, "This entity doesn't have anything stored into this storage.");
	return data[entity_to_data[p_entity]];
}

template <class T>
T &DenseVector<T>::get(EntityID p_entity) {
	CRASH_COND_MSG(has(p_entity) == false, "This entity doesn't have anything stored into this storage.");
	return data[entity_to_data[p_entity]];
}

template <class T>
void DenseVector<T>::remove(EntityID p_entity) {
	ERR_FAIL_COND_MSG(has(p_entity) == false, "This entity doesn't have anything stored into this storage.");

	const uint32_t last = data.size() - 1;

	if (p_entity < last) {
		data[entity_to_data[p_entity]] = data[last];
		data_to_entity[entity_to_data[p_entity]] = data_to_entity[last];
	}

	data.remove(last);
	data_to_entity.remove(last);
	entity_to_data[p_entity] = UINT32_MAX;
}
#endif