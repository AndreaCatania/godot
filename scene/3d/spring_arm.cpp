/*************************************************************************/
/*  spring_arm.h                                                         */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2018 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2018 Godot Engine contributors (cf. AUTHORS.md)    */
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

#include "spring_arm.h"
#include "engine.h"
#include "scene/3d/collision_object.h"
#include "scene/resources/sphere_shape.h"
#include "servers/physics_server.h"

SpringArm::SpringArm() :
		spring_length(1) {}

void SpringArm::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE:
			//if (!Engine::get_singleton()->is_editor_hint())
			set_process_internal(true);
			break;
		case NOTIFICATION_EXIT_TREE:
			//if (!Engine::get_singleton()->is_editor_hint())
			set_process_internal(false);
			break;
		case NOTIFICATION_INTERNAL_PROCESS:
			process_spring();
			break;
	}
}

void SpringArm::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_spring_length", "length"), &SpringArm::set_spring_length);
	ClassDB::bind_method(D_METHOD("get_spring_length"), &SpringArm::get_spring_length);

	ClassDB::bind_method(D_METHOD("set_shape", "shape"), &SpringArm::set_shape);
	ClassDB::bind_method(D_METHOD("get_shape"), &SpringArm::get_shape);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shape", PROPERTY_HINT_RESOURCE_TYPE, "Shape"), "set_shape", "get_shape");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "spring_length"), "set_spring_length", "get_spring_length");
}

float SpringArm::get_spring_length() const {
	return spring_length;
}

void SpringArm::set_spring_length(float p_length) {
	spring_length = p_length;
}

void SpringArm::set_shape(Ref<Shape> p_shape) {
	shape = p_shape;
}

Ref<Shape> SpringArm::get_shape() const {
	return shape;
}

void SpringArm::process_spring() {

	if (shape.is_null())
		return;

	// From
	real_t motion_delta(1);
	real_t motion_delta_unsafe(1);

	const Vector3 cast_direction(get_global_transform().basis.xform(Vector3(0, 0, 1)));
	const Vector3 motion(cast_direction * spring_length);

	get_world()->get_direct_space_state()->cast_motion(shape->get_rid(), get_global_transform(), motion, 0, motion_delta, motion_delta_unsafe);

	Transform childs_transform;
	childs_transform.basis = get_global_transform().basis;
	childs_transform.origin = get_global_transform().origin + cast_direction * (spring_length * motion_delta);

	for (int i = get_child_count() - 1; 0 <= i; --i) {

		Spatial *child(Object::cast_to<Spatial>(get_child(i)));
		if (child)
			child->set_global_transform(childs_transform);
	}
}
