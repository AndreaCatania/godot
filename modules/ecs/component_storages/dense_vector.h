/* Author: AndreaCatania */

#ifndef DENSE_VECTOR_H
#define DENSE_VECTOR_H

#include "core/local_vector.h"
#include "modules/ecs/ecs.h"

/// Dense vector storage.
/// Has a redirection 2-way table between entities and
/// components (vice versa), allowing to leave no gaps within the data.
/// The the entity indices are stored sparsely.
template <class T>
class DenseVector {
	LocalVector<T> data;
	LocalVector<EntityIndex> data_to_entity;
	// Each position of this vector is an Entity Index.
	LocalVector<uint32_t> index_to_data;

public:
	void insert(EntityIndex p_index, T p_data);
	void remove(EntityIndex p_index);
};

template <class T>
void DenseVector<T>::insert(EntityIndex p_index, T p_data) {
	if (index_to_data.size() <= p_index.index) {
		const uint32_t start = index_to_data.size();
		// Resize the vector so to fit this new entity.
		index_to_data.resize(p_index.index + 1);
		for (uint32_t i = start; i < index_to_data.size(); i += 1) {
			index_to_data[i] = UINT32_MAX;
		}
	}
	index_to_data[p_index.index] = data.size();
	data.push_back(p_data);
	data_to_entity.push_back(p_index);
}

template <class T>
void DenseVector<T>::remove(EntityIndex p_index) {
	ERR_FAIL_COND_MSG(data.size() <= 0, "The storage is already void.");
	ERR_FAIL_COND_MSG(p_index.index >= data.size(), "This entity doesn't have anything stored into this storage.");

	const uint32_t last = data.size() - 1;

	data[index_to_data[p_index.index]] = data[last];
	data_to_entity[index_to_data[p_index.index]] = data_to_entity[last];

	index_to_data[p_index.index] = UINT32_MAX;
}
#endif