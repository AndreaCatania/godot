/*************************************************************************/
/*  test_oa_hash_map.cpp                                                 */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2020 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2020 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "test_transform.h"

#include "core/os/os.h"
#include "thirdparty/libsimdpp/simdpp/simd.h"

#define CELL_ID(row, col) col + row * 4
#define TRANSPOSED_CELL_ID(row, col) row + col * 4

/// Row major matrix.
struct Matrix4x4 {
	real_t elements[16];

	/// Init identity matrix.
	Matrix4x4() {
		memset(elements, 0, sizeof(real_t) * 16);
		elements[CELL_ID(0, 0)] = 1.0;
		elements[CELL_ID(1, 1)] = 1.0;
		elements[CELL_ID(2, 2)] = 1.0;
		elements[CELL_ID(3, 3)] = 1.0;
	}

	Matrix4x4(real_t def_val) {
		memset(elements, def_val, sizeof(real_t) * 16);
	}

	Matrix4x4(const Basis &p_basis, const Vector3 &p_origin = Vector3()) {
		elements[CELL_ID(0, 0)] = p_basis.elements[0][0];
		elements[CELL_ID(0, 1)] = p_basis.elements[0][1];
		elements[CELL_ID(0, 2)] = p_basis.elements[0][2];
		elements[CELL_ID(0, 3)] = p_origin[0];

		elements[CELL_ID(1, 0)] = p_basis.elements[1][0];
		elements[CELL_ID(1, 1)] = p_basis.elements[1][1];
		elements[CELL_ID(1, 2)] = p_basis.elements[1][2];
		elements[CELL_ID(1, 3)] = p_origin[1];

		elements[CELL_ID(2, 0)] = p_basis.elements[2][0];
		elements[CELL_ID(2, 1)] = p_basis.elements[2][1];
		elements[CELL_ID(2, 2)] = p_basis.elements[2][2];
		elements[CELL_ID(2, 3)] = p_origin[2];

		elements[CELL_ID(3, 0)] = 0.0;
		elements[CELL_ID(3, 1)] = 0.0;
		elements[CELL_ID(3, 2)] = 0.0;
		elements[CELL_ID(3, 3)] = 1.0;
	}

	void operator*=(const Matrix4x4 &p_other) {
		*this = (*this) * p_other;
	}

	// Performs the multiplication between two `Matrix` using SIMD operations.
	//
	// This algorithm doen't use the "mathematical" way of doing it: A_rows * B_columns.
	//
	// Rather it:
	// - Creates the result `Matrix` with all cells set to 0.
	// - For each element in the Matrix A perform a multiplication against the elements of the same
	// rows in the Matrix B, advancing per each column in the Matrix A a row in the Matrix B.
	// - Sum the result directly to the result `Matrix`.
	//
	// In this way it is possible to use SIMD APIs and benefit from the CPU cache.
	Matrix4x4 operator*(const Matrix4x4 &p_other) const {
		// Testing how much time it needs to store 4 vectors.

		real_t a[4] = { elements[CELL_ID(0, 0)], elements[CELL_ID(1, 0)], elements[CELL_ID(2, 0)], elements[CELL_ID(3, 0)] };
		real_t b[4] = { elements[CELL_ID(0, 1)], elements[CELL_ID(1, 1)], elements[CELL_ID(2, 1)], elements[CELL_ID(3, 1)] };
		real_t c[4] = { elements[CELL_ID(0, 2)], elements[CELL_ID(1, 2)], elements[CELL_ID(2, 2)], elements[CELL_ID(3, 2)] };
		real_t d[4] = { elements[CELL_ID(0, 3)], elements[CELL_ID(1, 3)], elements[CELL_ID(2, 3)], elements[CELL_ID(3, 3)] };

		const simdpp::float32<4> col_1 = simdpp::load(a);
		const simdpp::float32<4> col_2 = simdpp::load(b);
		const simdpp::float32<4> col_3 = simdpp::load(c);
		const simdpp::float32<4> col_4 = simdpp::load(d);

		const simdpp::float32<4> row_1 = simdpp::load(p_other.elements + CELL_ID(0, 0));
		const simdpp::float32<4> row_2 = simdpp::load(p_other.elements + CELL_ID(1, 0));
		const simdpp::float32<4> row_3 = simdpp::load(p_other.elements + CELL_ID(2, 0));
		const simdpp::float32<4> row_4 = simdpp::load(p_other.elements + CELL_ID(3, 0));

		const simdpp::float32<4> res_1 = simdpp::mul(col_1, row_1);
		const simdpp::float32<4> res_2 = simdpp::mul(col_2, row_2);
		const simdpp::float32<4> res_3 = simdpp::mul(col_3, row_3);
		const simdpp::float32<4> res_4 = simdpp::mul(col_4, row_4);

		Matrix4x4 res(0.0);
		simdpp::store(res.elements + CELL_ID(0, 0), res_1);
		simdpp::store(res.elements + CELL_ID(1, 0), res_2);
		simdpp::store(res.elements + CELL_ID(2, 0), res_3);
		simdpp::store(res.elements + CELL_ID(3, 0), res_4);

		return res;
	}

	//Matrix4x4 operator*(const Matrix4x4 &p_other) const {
	//	Matrix4x4 res(0.0);

	//	const simdpp::float32<4> row_1 = simdpp::load(p_other.elements + CELL_ID(0, 0));
	//	const simdpp::float32<4> row_2 = simdpp::load(p_other.elements + CELL_ID(1, 0));
	//	const simdpp::float32<4> row_3 = simdpp::load(p_other.elements + CELL_ID(2, 0));
	//	const simdpp::float32<4> row_4 = simdpp::load(p_other.elements + CELL_ID(3, 0));

	//	for (int r = 0; r < 4; r += 1) {
	//		const simdpp::float32<4> broad_1 = simdpp::splat(elements[CELL_ID(r, 0)]);
	//		const simdpp::float32<4> broad_2 = simdpp::splat(elements[CELL_ID(r, 1)]);
	//		const simdpp::float32<4> broad_3 = simdpp::splat(elements[CELL_ID(r, 2)]);
	//		const simdpp::float32<4> broad_4 = simdpp::splat(elements[CELL_ID(r, 3)]);

	//		const simdpp::float32<4> row_val = simdpp::add(
	//				simdpp::add(
	//						simdpp::mul(broad_1, row_1),
	//						simdpp::mul(broad_2, row_2)),
	//				simdpp::add(
	//						simdpp::mul(broad_3, row_3),
	//						simdpp::mul(broad_4, row_4)));

	//		simdpp::store(res.elements + CELL_ID(r, 0), row_val);
	//	}
	//	return res;
	//}

	//Matrix4x4 operator*(const Matrix4x4 &p_other) const {
	//	const int rows = 4;
	//	const int columns = 4;

	//	Matrix4x4 res(0.0);

	//	// TODO use alligned?
	//	for (int r = 0; r < rows; r += 1) {
	//		for (int c = 0; c < columns; c += 1) {
	//			//const real_t left_val = elements[CELL_ID(r, c)];
	//			const simdpp::float32<4> left_val = simdpp::splat(elements[CELL_ID(r, c)]);

	//			// TODO support f64
	//			for (int other_c = 0; other_c < columns; other_c += SIMDPP_FAST_FLOAT32_SIZE) {
	//				//const real_t value = left_val * p_other.elements[CELL_ID(c, other_c)];
	//				//res.elements[CELL_ID(r, other_c)] += value;

	//				const simdpp::float32<4> right_vals = simdpp::load(p_other.elements + CELL_ID(c, other_c));
	//				const simdpp::float32<4> mul_vals = simdpp::mul(left_val, right_vals);

	//				const simdpp::float32<4> current_vals = simdpp::load(res.elements + CELL_ID(r, other_c));
	//				const simdpp::float32<4> vals = simdpp::add(current_vals, mul_vals);

	//				simdpp::store(res.elements + CELL_ID(r, other_c), vals);
	//			}
	//		}
	//	}

	//	return res;
	//}

	Vector3 get_origin() const {
		return Vector3(elements[CELL_ID(0, 3)], elements[CELL_ID(1, 3)], elements[CELL_ID(2, 3)]);
	}

	Basis get_basis() const {
		// TODO I need to apply the scaling.
		return Basis(
				elements[CELL_ID(0, 0)], elements[CELL_ID(0, 1)], elements[CELL_ID(0, 2)],
				elements[CELL_ID(1, 0)], elements[CELL_ID(1, 1)], elements[CELL_ID(1, 2)],
				elements[CELL_ID(2, 0)], elements[CELL_ID(2, 1)], elements[CELL_ID(2, 2)]);
	}

	operator String() const {
		return get_basis().operator String() + " - " + get_origin().operator String();
	}
};

#define MULTIPLICATION_COUNT 100000000
//#define MULTIPLICATION_COUNT 10

namespace TestTransform {

void test_godot_transform() {
	OS::get_singleton()->print("\n\n\nTransform perf\n");

	Transform total;

	// Test transform multiplication
	for (int i = 0; i < MULTIPLICATION_COUNT; i += 1) {
		const Transform t1(Basis(), Vector3(i, 0, 0));
		const Transform t2(Basis(), Vector3(0, i, 0));
		const Transform t3(Basis(), Vector3(0, 0, i));
		total = total * t1 * t2 * t3;
	}

	OS::get_singleton()->print("\n\n\nResult: %ls\n", String(total).c_str());
}

void test_new_transform() {
	OS::get_singleton()->print("\n\n\nTransform perf\n");

	Matrix4x4 total;

	// Test transform multiplication
	for (int i = 0; i < MULTIPLICATION_COUNT; i += 1) {
		const Matrix4x4 t1(Basis(), Vector3(i, 0, 0));
		const Matrix4x4 t2(Basis(), Vector3(0, i, 0));
		const Matrix4x4 t3(Basis(), Vector3(0, 0, i));
		total = total * t1 * t2 * t3;
	}

	OS::get_singleton()->print("\n\n\nResult: %ls\n", String(total).c_str());
}

MainLoop *test() {
	test_godot_transform();
	test_new_transform();

	return nullptr;
}

} // namespace TestTransform
