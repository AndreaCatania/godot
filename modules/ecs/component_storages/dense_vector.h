/* Author: AndreaCatania */

#ifndef DENSE_VECTOR_H
#define DENSE_VECTOR_H

#include "core/local_vector.h"

template <class T>
class DenseVector {
	LocalVector<T> data;
};

#endif