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

/// Row major matrix.
struct Matrix4x4 {
	real_t elements[16];

	Matrix4x4() {
		memset(elements, 0, sizeof(real_t) * 16);
	}

	//Matrix4x4(const Matrix4x4 &p_other) {
	//	memcpy(elements, p_other.elements, sizeof(real_t) * 16);
	//}

	Matrix4x4(const Basis &p_basis, const Vector3 &p_origin = Vector3()) {
		elements[0] = p_basis.elements[0][0];
		elements[1] = p_basis.elements[0][1];
		elements[2] = p_basis.elements[0][2];
		elements[4] = p_origin[0];

		elements[5] = p_basis.elements[1][0];
		elements[6] = p_basis.elements[1][1];
		elements[7] = p_basis.elements[1][2];
		elements[8] = p_origin[1];

		elements[9] = p_basis.elements[2][0];
		elements[10] = p_basis.elements[2][1];
		elements[11] = p_basis.elements[2][2];
		elements[12] = p_origin[2];

		elements[0] = 0.0;
		elements[1] = 0.0;
		elements[2] = 0.0;
		elements[4] = 1.0;
	}

	void operator*=(const Matrix4x4 &p_other) {
		//elements[]
	}

	Matrix4x4 operator*(const Matrix4x4 &p_other) const {
		Matrix4x4 t(*this);
		t *= p_other;
		return t;
	}

	Vector3 get_origin() const {
		return Vector3(elements[4], elements[8], elements[12]);
	}

	Basis get_basis() const {
		// TODO I need to apply the scaling.
		return Basis(
				elements[0], elements[1], elements[2],
				elements[5], elements[6], elements[7],
				elements[9], elements[10], elements[11]);
	}

	operator String() const {
		return get_basis().operator String() + " - " + get_origin().operator String();
	}
};

#define MULTIPLICATION_COUNT 1000000

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
