/*************************************************************************/
/*  physics_particle_body.cpp                                            */
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

/**
 * @author AndreaCatania
 */

#include "physics_particle_body.h"

#include "core/core_string_names.h"
#include "scene/3d/physics_particle_body_mesh_instance.h"
#include "scene/3d/skeleton.h"

void ParticleBody::_bind_methods() {

	ClassDB::bind_method(D_METHOD("get_particle_body_mesh"), &ParticleBody::get_particle_body_mesh);

	ClassDB::bind_method(D_METHOD("set_particle_body_model", "model"), &ParticleBody::set_particle_body_model);
	ClassDB::bind_method(D_METHOD("get_particle_body_model"), &ParticleBody::get_particle_body_model);

	ClassDB::bind_method(D_METHOD("set_update_spatial_transform", "update"), &ParticleBody::set_update_spatial_transform);
	ClassDB::bind_method(D_METHOD("get_update_spatial_transform"), &ParticleBody::get_update_spatial_transform);

	ClassDB::bind_method(D_METHOD("remove_particle", "particle_index"), &ParticleBody::remove_particle);
	ClassDB::bind_method(D_METHOD("remove_rigid", "rigid_index"), &ParticleBody::remove_rigid);

	ClassDB::bind_method(D_METHOD("set_collision_group", "layer"), &ParticleBody::set_collision_group);
	ClassDB::bind_method(D_METHOD("get_collision_group"), &ParticleBody::get_collision_group);

	ClassDB::bind_method(D_METHOD("set_collision_flag_self_collide", "active"), &ParticleBody::set_collision_flag_self_collide);
	ClassDB::bind_method(D_METHOD("get_collision_flag_self_collide"), &ParticleBody::get_collision_flag_self_collide);

	ClassDB::bind_method(D_METHOD("set_collision_flag_self_collide_filter", "active"), &ParticleBody::set_collision_flag_self_collide_filter);
	ClassDB::bind_method(D_METHOD("get_collision_flag_self_collide_filter"), &ParticleBody::get_collision_flag_self_collide_filter);

	ClassDB::bind_method(D_METHOD("set_collision_flag_fluid", "active"), &ParticleBody::set_collision_flag_fluid);
	ClassDB::bind_method(D_METHOD("get_collision_flag_fluid"), &ParticleBody::get_collision_flag_fluid);

	ClassDB::bind_method(D_METHOD("set_collision_primitive_mask", "mask"), &ParticleBody::set_collision_primitive_mask);
	ClassDB::bind_method(D_METHOD("get_collision_primitive_mask"), &ParticleBody::get_collision_primitive_mask);

	ClassDB::bind_method(D_METHOD("set_collision_primitive_mask_bit", "bit", "val"), &ParticleBody::set_collision_primitive_mask_bit);
	ClassDB::bind_method(D_METHOD("get_collision_primitive_mask_bit", "bit"), &ParticleBody::get_collision_primitive_mask_bit);

	ClassDB::bind_method(D_METHOD("set_monitorable", "monitorable"), &ParticleBody::set_monitorable);
	ClassDB::bind_method(D_METHOD("is_monitorable"), &ParticleBody::is_monitorable);

	ClassDB::bind_method(D_METHOD("set_monitoring_primitives_contacts", "monitoring"), &ParticleBody::set_monitoring_primitives_contacts);
	ClassDB::bind_method(D_METHOD("is_monitoring_primitives_contacts"), &ParticleBody::is_monitoring_primitives_contacts);

	ClassDB::bind_method(D_METHOD("get_particle_count"), &ParticleBody::get_particle_count);
	ClassDB::bind_method(D_METHOD("get_spring_count"), &ParticleBody::get_spring_count);
	ClassDB::bind_method(D_METHOD("get_rigid_count"), &ParticleBody::get_rigid_count);

	ClassDB::bind_method(D_METHOD("resource_changed", "resource"), &ParticleBody::resource_changed);

	ClassDB::bind_method(D_METHOD("commands_process_internal", "commands"), &ParticleBody::commands_process_internal);
	ClassDB::bind_method(D_METHOD("on_primitive_contact", "commands", "primitive_object", "particle_index", "velocity", "normal"), &ParticleBody::on_primitive_contact);

	ClassDB::bind_method(D_METHOD("_on_script_changed"), &ParticleBody::_on_script_changed);

	// Virtual methods
	BIND_VMETHOD(MethodInfo("_commands_process", PropertyInfo(Variant::OBJECT, "commands", PROPERTY_HINT_RESOURCE_TYPE, "ParticleBodyCommands")));
	BIND_VMETHOD(MethodInfo("_on_particle_index_change", PropertyInfo(Variant::OBJECT, "old_index"), PropertyInfo(Variant::OBJECT, "new_index")));
	BIND_VMETHOD(MethodInfo("_on_spring_index_change", PropertyInfo(Variant::OBJECT, "old_index"), PropertyInfo(Variant::OBJECT, "new_index")));

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "particle_body_model", PROPERTY_HINT_RESOURCE_TYPE, "ParticleBodyModel"), "set_particle_body_model", "get_particle_body_model");

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "update_spatial_transform"), "set_update_spatial_transform", "get_update_spatial_transform");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "monitorable"), "set_monitorable", "is_monitorable");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "monitoring_primitives_contacts"), "set_monitoring_primitives_contacts", "is_monitoring_primitives_contacts");

	ADD_GROUP("Collision", "collision_");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "collision_group", PROPERTY_HINT_RANGE, "0,21,1"), "set_collision_group", "get_collision_group");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "collision_flag_self_collide"), "set_collision_flag_self_collide", "get_collision_flag_self_collide");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "collision_flag_self_collide_filter"), "set_collision_flag_self_collide_filter", "get_collision_flag_self_collide_filter");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "collision_flag_fluid"), "set_collision_flag_fluid", "get_collision_flag_fluid");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "collision_primitive_mask", PROPERTY_HINT_LAYERS_3D_PHYSICS), "set_collision_primitive_mask", "get_collision_primitive_mask");

	ADD_SIGNAL(MethodInfo("resource_loaded"));
	ADD_SIGNAL(MethodInfo("commands_process", PropertyInfo(Variant::OBJECT, "commands", PROPERTY_HINT_RESOURCE_TYPE, "ParticleBodyCommands")));
	ADD_SIGNAL(MethodInfo("primitive_contact", PropertyInfo(Variant::OBJECT, "commands", PROPERTY_HINT_RESOURCE_TYPE, "ParticleBodyCommands"), PropertyInfo(Variant::OBJECT, "primitive_body"), PropertyInfo(Variant::INT, "particle_index"), PropertyInfo(Variant::VECTOR3, "velocity"), PropertyInfo(Variant::VECTOR3, "normal")));
}

ParticleBody::ParticleBody() :
		ParticleObject(ParticlePhysicsServer::get_singleton()->body_create()),
		update_spatial_transform(false),
		reload_particle_model(true),
		initial_transform(true),
		reset_transform(false),
		particle_body_mesh(NULL),
		draw_gizmo(false) {

	ParticlePhysicsServer::get_singleton()->body_set_object_instance(rid, this);

	set_notify_transform(true);

	connect(CoreStringNames::get_singleton()->script_changed, this, "_on_script_changed");
	ParticlePhysicsServer::get_singleton()->body_set_callback(rid, ParticlePhysicsServer::PARTICLE_BODY_CALLBACK_PRIMITIVECONTACT, this, "on_primitive_contact");
}

ParticleBody::~ParticleBody() {

	if (particle_body_model.is_valid())
		particle_body_model->unregister_owner(this);

	ParticlePhysicsServer::get_singleton()->body_set_callback(rid, ParticlePhysicsServer::PARTICLE_BODY_CALLBACK_SYNC, NULL, "");
	ParticlePhysicsServer::get_singleton()->body_set_callback(rid, ParticlePhysicsServer::PARTICLE_BODY_CALLBACK_PARTICLEINDEXCHANGED, NULL, "");
	ParticlePhysicsServer::get_singleton()->body_set_callback(rid, ParticlePhysicsServer::PARTICLE_BODY_CALLBACK_SPRINGINDEXCHANGED, NULL, "");
	ParticlePhysicsServer::get_singleton()->body_set_callback(rid, ParticlePhysicsServer::PARTICLE_BODY_CALLBACK_PRIMITIVECONTACT, NULL, "");
}

String ParticleBody::get_configuration_warning() const {
	String warning = ParticleObject::get_configuration_warning();

	if (!particle_body_mesh) {
		if (warning != String()) {
			warning += "\n\n";
		}
		warning += TTR("The body will be ignored until you set a ParticleBodyMeshInstance");
	}

	if (particle_body_model.is_null()) {
		if (warning != String()) {
			warning += "\n\n";
		}
		warning += TTR("The body will be ignored until you set a ParticleBodyModel.\nTo craete one is possible to use the ParticleBody menu in the top bar.");
	}

	if (!update_spatial_transform)
		if (get_child_count()) {
			if (get_child_count() != 1 || !particle_body_mesh) {
				bool has_spatial = false;

				for (int i(0); i < get_child_count(); ++i) {

					Spatial *s = cast_to<Spatial>(get_child(i));
					if (s && s != particle_body_mesh) {
						has_spatial = true;
						break;
					}
				}

				if (has_spatial) {

					if (warning != String()) {
						warning += "\n\n";
					}
					warning += TTR("The child spatial node will not follow this ParticleBody untill you set \"update_spatial_transform\" to TRUE.");
				}
			}
		}

	return warning;
}

void ParticleBody::set_particle_body_mesh(ParticleBodyMeshInstance *p_mesh) {
	particle_body_mesh = p_mesh;
	update_configuration_warning();
}

void ParticleBody::set_particle_body_model(Ref<ParticleBodyModel> p_model) {
	if (particle_body_model == p_model)
		return;

	if (particle_body_model.is_valid())
		particle_body_model->unregister_owner(this);

	particle_body_model = p_model;

	if (particle_body_model.is_valid())
		particle_body_model->register_owner(this);

	resource_changed(particle_body_model);
}

Ref<ParticleBodyModel> ParticleBody::get_particle_body_model() const {
	return particle_body_model;
}

void ParticleBody::remove_particle(int p_particle_index) {
	ParticlePhysicsServer::get_singleton()->body_remove_particle(rid, p_particle_index);
}

void ParticleBody::remove_rigid(int p_rigid_index) {
	ParticlePhysicsServer::get_singleton()->body_remove_rigid(rid, p_rigid_index);
}

void ParticleBody::set_collision_group(uint32_t p_group) {
	ParticlePhysicsServer::get_singleton()->body_set_collision_group(rid, p_group);
}

uint32_t ParticleBody::get_collision_group() const {
	return ParticlePhysicsServer::get_singleton()->body_get_collision_group(rid);
}

void ParticleBody::set_collision_flag_self_collide(bool p_active) {
	ParticlePhysicsServer::get_singleton()->body_set_collision_flag(rid, ParticlePhysicsServer::PARTICLE_COLLISION_FLAG_SELF_COLLIDE, p_active);
}

bool ParticleBody::get_collision_flag_self_collide() const {
	return ParticlePhysicsServer::get_singleton()->body_get_collision_flag(rid, ParticlePhysicsServer::PARTICLE_COLLISION_FLAG_SELF_COLLIDE);
}

void ParticleBody::set_collision_flag_self_collide_filter(bool p_active) {
	ParticlePhysicsServer::get_singleton()->body_set_collision_flag(rid, ParticlePhysicsServer::PARTICLE_COLLISION_FLAG_SELF_COLLIDE_FILTER, p_active);
}

bool ParticleBody::get_collision_flag_self_collide_filter() const {
	return ParticlePhysicsServer::get_singleton()->body_get_collision_flag(rid, ParticlePhysicsServer::PARTICLE_COLLISION_FLAG_SELF_COLLIDE_FILTER);
}

void ParticleBody::set_collision_flag_fluid(bool p_active) {
	ParticlePhysicsServer::get_singleton()->body_set_collision_flag(rid, ParticlePhysicsServer::PARTICLE_COLLISION_FLAG_FLUID, p_active);
}

bool ParticleBody::get_collision_flag_fluid() const {
	return ParticlePhysicsServer::get_singleton()->body_get_collision_flag(rid, ParticlePhysicsServer::PARTICLE_COLLISION_FLAG_FLUID);
}

void ParticleBody::set_collision_primitive_mask(uint32_t p_mask) {
	ParticlePhysicsServer::get_singleton()->body_set_collision_primitive_mask(rid, p_mask);
}

uint32_t ParticleBody::get_collision_primitive_mask() const {
	return ParticlePhysicsServer::get_singleton()->body_get_collision_primitive_mask(rid);
}

void ParticleBody::set_collision_primitive_mask_bit(int p_bit, bool p_value) {

	uint32_t mask = get_collision_primitive_mask();
	if (p_value)
		mask |= 1 << p_bit;
	else
		mask &= ~(1 << p_bit);
	set_collision_primitive_mask(mask);
}

bool ParticleBody::get_collision_primitive_mask_bit(int p_bit) const {

	return get_collision_primitive_mask() & (1 << p_bit);
}

void ParticleBody::set_monitorable(bool p_monitorable) {
	ParticlePhysicsServer::get_singleton()->body_set_monitorable(rid, p_monitorable);
}

bool ParticleBody::is_monitorable() const {
	return ParticlePhysicsServer::get_singleton()->body_is_monitorable(rid);
}

void ParticleBody::set_monitoring_primitives_contacts(bool p_monitoring) {
	ParticlePhysicsServer::get_singleton()->body_set_monitoring_primitives_contacts(rid, p_monitoring);
}

bool ParticleBody::is_monitoring_primitives_contacts() const {
	return ParticlePhysicsServer::get_singleton()->body_is_monitoring_primitives_contacts(rid);
}

void ParticleBody::set_update_spatial_transform(bool p_update) {
	update_spatial_transform = p_update;
}

bool ParticleBody::get_update_spatial_transform() const {
	return update_spatial_transform;
}

int ParticleBody::get_particle_count() const {
	return ParticlePhysicsServer::get_singleton()->body_get_particle_count(rid);
}

int ParticleBody::get_spring_count() const {
	return ParticlePhysicsServer::get_singleton()->body_get_spring_count(rid);
}

int ParticleBody::get_rigid_count() const {
	return ParticlePhysicsServer::get_singleton()->body_get_rigid_count(rid);
}

void ParticleBody::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			ParticlePhysicsServer::get_singleton()->body_set_space(rid, get_world()->get_particle_space());
			ParticlePhysicsServer::get_singleton()->body_set_callback(rid, ParticlePhysicsServer::PARTICLE_BODY_CALLBACK_SYNC, this, "commands_process_internal");
			resource_changed(particle_body_model);

			if (!particle_body_mesh) {
				for (int i = 0; i < get_child_count(); ++i) {
					ParticleBodyMeshInstance *m = cast_to<ParticleBodyMeshInstance>(get_child(i));
					if (m) {
						set_particle_body_mesh(m);
						break;
					}
				}
			}
		} break;
		case NOTIFICATION_TRANSFORM_CHANGED: {

			debug_reset_particle_positions();
			if (initial_transform) {
				initial_transform = false;
			} else {
				reset_transform = true;
			}

		} break;
		case NOTIFICATION_EXIT_TREE: {
			ParticlePhysicsServer::get_singleton()->body_set_callback(rid, ParticlePhysicsServer::PARTICLE_BODY_CALLBACK_SYNC, NULL, "");
			ParticlePhysicsServer::get_singleton()->body_set_space(rid, RID());
			debug_initialize_resource();
		} break;
	}
}

void ParticleBody::resource_changed(RES p_res) {
	if (particle_body_model == p_res) {
		reload_particle_model = true;
		debug_initialize_resource();
	}
	update_gizmo();
	update_configuration_warning();
}

void ParticleBody::commands_process_internal(Object *p_cmds) {

	ParticleBodyCommands *cmds(static_cast<ParticleBodyCommands *>(p_cmds));

	if (reload_particle_model) {
		reload_particle_model = false;
		reset_transform = false;
		cmds->load_model(particle_body_model, get_global_transform());
		emit_signal("resource_loaded");
	}

	if (reset_transform) {
		reset_transform = false;
		cmds->move_particles(get_global_transform() * compute_transform(cmds).inverse());
	} else {

		update_transform(cmds);
	}

	if (particle_body_mesh)
		particle_body_mesh->update_mesh(cmds);

	emit_signal("commands_process", cmds);

	debug_update(cmds);

	if (!get_script().is_null() && has_method("_commands_process")) {
		call("_commands_process", p_cmds);
	}
}

Transform ParticleBody::compute_transform(ParticleBodyCommands *p_cmds) {

	const int rigids_count = ParticlePhysicsServer::get_singleton()->body_get_rigid_count(get_rid());

	if (!rigids_count)
		return Transform();

	Transform average_transform(Basis(p_cmds->get_rigid_rotation(0)), p_cmds->get_rigid_position(0));

	for (int i = 1; i < rigids_count; ++i) {

		average_transform = average_transform.interpolate_with(Transform(Basis(p_cmds->get_rigid_rotation(i)), p_cmds->get_rigid_position(i)), 0.5);
	}

	return average_transform;
}

void ParticleBody::update_transform(ParticleBodyCommands *p_cmds) {

	if (!update_spatial_transform)
		return;

	set_notify_transform(false);
	set_global_transform(compute_transform(p_cmds));
	set_notify_transform(true);
}

void ParticleBody::on_primitive_contact(Object *p_cmds, Object *p_primitive_object, int p_particle_index, Vector3 p_velocity, Vector3 p_normal) {

	emit_signal("primitive_contact", p_cmds, p_primitive_object, p_particle_index, p_velocity, p_normal);
}

void ParticleBody::_on_script_changed() {
	if (has_method("_on_particle_index_change")) {
		ParticlePhysicsServer::get_singleton()->body_set_callback(rid, ParticlePhysicsServer::PARTICLE_BODY_CALLBACK_PARTICLEINDEXCHANGED, this, "_on_particle_index_change");
	} else {
		ParticlePhysicsServer::get_singleton()->body_set_callback(rid, ParticlePhysicsServer::PARTICLE_BODY_CALLBACK_PARTICLEINDEXCHANGED, NULL, "");
	}

	if (has_method("_on_spring_index_change")) {
		ParticlePhysicsServer::get_singleton()->body_set_callback(rid, ParticlePhysicsServer::PARTICLE_BODY_CALLBACK_SPRINGINDEXCHANGED, this, "_on_spring_index_change");
	} else {
		ParticlePhysicsServer::get_singleton()->body_set_callback(rid, ParticlePhysicsServer::PARTICLE_BODY_CALLBACK_SPRINGINDEXCHANGED, NULL, "");
	}
}

void ParticleBody::debug_initialize_resource() {

	if (!is_inside_tree() || !get_tree()->is_debugging_collisions_hint())
		return;

	const real_t radius = ParticlePhysicsServer::get_singleton()->space_get_particle_radius(get_world()->get_particle_space());

	debug_particle_mesh.instance();
	debug_particle_mesh->set_radius(radius);
	debug_particle_mesh->set_height(radius * 2);
	debug_particle_mesh->set_radial_segments(8);
	debug_particle_mesh->set_rings(8);

	const int particle_count = particle_body_model.is_valid() ? particle_body_model->get_particles_ref().size() : 0;
	debug_resize_particle_visual_instance(particle_count);
	debug_reset_particle_positions();
}

void ParticleBody::debug_resize_particle_visual_instance(int new_size) {

	if (debug_particles_mesh.size() == new_size)
		return;

	if (debug_particles_mesh.size() > new_size) {

		// If the particle count is less then visual instances size, free the last
		const int dif = debug_particles_mesh.size() - new_size;
		for (int i = 0; i < dif; ++i) {

			const int p = debug_particles_mesh.size() - i - 1;
			debug_particles_mesh[p]->queue_delete();
			debug_particles_mesh.write[p] = NULL;
		}
		debug_particles_mesh.resize(new_size);
	} else {

		if (!is_inside_world())
			return;

		// If the particle count is more then visual instances resize and create last
		const int dif = new_size - debug_particles_mesh.size();
		debug_particles_mesh.resize(new_size);
		for (int i = 0; i < dif; ++i) {

			const int p = new_size - i - 1;
			debug_particles_mesh.write[p] = memnew(MeshInstance);
			debug_particles_mesh[p]->set_as_toplevel(true);
			debug_particles_mesh[p]->set_material_override(get_tree()->get_debug_collision_material());
			debug_particles_mesh[p]->set_mesh(debug_particle_mesh);
			add_child(debug_particles_mesh[p]);
		}
	}
}

void ParticleBody::debug_update(ParticleBodyCommands *p_cmds) {

	if (!get_tree()->is_debugging_collisions_hint())
		return;

	const int particle_count = ParticlePhysicsServer::get_singleton()->body_get_particle_count(rid);
	debug_resize_particle_visual_instance(particle_count);

	Transform transf;
	for (int i = 0; i < particle_count; ++i) {

		transf.origin = p_cmds->get_particle_position(i);
		debug_particles_mesh[i]->set_global_transform(transf);
	}
}

void ParticleBody::debug_reset_particle_positions() {

	if (!get_tree()->is_debugging_collisions_hint())
		return;

	if (particle_body_model.is_null())
		return;

	if (debug_particles_mesh.size() == particle_body_model->get_particles_ref().size()) {

		Transform particle_relative_transf;
		for (int i = 0; i < debug_particles_mesh.size(); ++i) {

			particle_relative_transf.origin = particle_body_model->get_particles_ref()[i];
			debug_particles_mesh[i]->set_global_transform(get_global_transform() * particle_relative_transf);
		}
	}
}

SoftParticleBody::SoftParticleBody() :
		ParticleBody(),
		radius(0.1),
		global_stiffness(0.1),
		internal_sample(true),
		particle_spacing(1),
		sampling(1),
		cluster_spacing(4),
		cluster_radius(6),
		cluster_stiffness(0.2),
		link_radius(0.5),
		link_stiffness(0.1),
		plastic_threshold(0),
		plastic_creep(0) {
}

void SoftParticleBody::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_radius", "value"), &SoftParticleBody::set_radius);
	ClassDB::bind_method(D_METHOD("get_radius"), &SoftParticleBody::get_radius);

	ClassDB::bind_method(D_METHOD("set_global_stiffness", "value"), &SoftParticleBody::set_global_stiffness);
	ClassDB::bind_method(D_METHOD("get_global_stiffness"), &SoftParticleBody::get_global_stiffness);

	ClassDB::bind_method(D_METHOD("set_internal_sample", "value"), &SoftParticleBody::set_internal_sample);
	ClassDB::bind_method(D_METHOD("get_internal_sample"), &SoftParticleBody::get_internal_sample);

	ClassDB::bind_method(D_METHOD("set_particle_spacing", "value"), &SoftParticleBody::set_particle_spacing);
	ClassDB::bind_method(D_METHOD("get_particle_spacing"), &SoftParticleBody::get_particle_spacing);

	ClassDB::bind_method(D_METHOD("set_sampling", "value"), &SoftParticleBody::set_sampling);
	ClassDB::bind_method(D_METHOD("get_sampling"), &SoftParticleBody::get_sampling);

	ClassDB::bind_method(D_METHOD("set_cluster_spacing", "value"), &SoftParticleBody::set_cluster_spacing);
	ClassDB::bind_method(D_METHOD("get_cluster_spacing"), &SoftParticleBody::get_cluster_spacing);

	ClassDB::bind_method(D_METHOD("set_cluster_radius", "value"), &SoftParticleBody::set_cluster_radius);
	ClassDB::bind_method(D_METHOD("get_cluster_radius"), &SoftParticleBody::get_cluster_radius);

	ClassDB::bind_method(D_METHOD("set_cluster_stiffness", "value"), &SoftParticleBody::set_cluster_stiffness);
	ClassDB::bind_method(D_METHOD("get_cluster_stiffness"), &SoftParticleBody::get_cluster_stiffness);

	ClassDB::bind_method(D_METHOD("set_link_radius", "value"), &SoftParticleBody::set_link_radius);
	ClassDB::bind_method(D_METHOD("get_link_radius"), &SoftParticleBody::get_link_radius);

	ClassDB::bind_method(D_METHOD("set_link_stiffness", "value"), &SoftParticleBody::set_link_stiffness);
	ClassDB::bind_method(D_METHOD("get_link_stiffness"), &SoftParticleBody::get_link_stiffness);

	ClassDB::bind_method(D_METHOD("set_plastic_threshold", "value"), &SoftParticleBody::set_plastic_threshold);
	ClassDB::bind_method(D_METHOD("get_plastic_threshold"), &SoftParticleBody::get_plastic_threshold);

	ClassDB::bind_method(D_METHOD("set_plastic_creep", "value"), &SoftParticleBody::set_plastic_creep);
	ClassDB::bind_method(D_METHOD("get_plastic_creep"), &SoftParticleBody::get_plastic_creep);

	ADD_PROPERTY(PropertyInfo(Variant::REAL, "model/radius"), "set_radius", "get_radius");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "model/global_stiffness"), "set_global_stiffness", "get_global_stiffness");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "model/internal_sample"), "set_internal_sample", "get_internal_sample");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "model/particle_spacing"), "set_particle_spacing", "get_particle_spacing");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "model/sampling"), "set_sampling", "get_sampling");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "model/cluster_spacing"), "set_cluster_spacing", "get_cluster_spacing");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "model/cluster_radius"), "set_cluster_radius", "get_cluster_radius");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "model/cluster_stiffness"), "set_cluster_stiffness", "get_cluster_stiffness");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "model/link_radius"), "set_link_radius", "get_link_radius");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "model/link_stiffness"), "set_link_stiffness", "get_link_stiffness");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "model/plastic_threshold"), "set_plastic_threshold", "get_plastic_threshold");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "model/plastic_creep"), "set_plastic_creep", "get_plastic_creep");
}

void SoftParticleBody::set_radius(real_t p_value) {
	radius = p_value;
}

real_t SoftParticleBody::get_radius() const {
	return radius;
}

void SoftParticleBody::set_global_stiffness(real_t p_value) {
	global_stiffness = p_value;
}

real_t SoftParticleBody::get_global_stiffness() const {
	return global_stiffness;
}

void SoftParticleBody::set_internal_sample(bool p_value) {
	internal_sample = p_value;
}

bool SoftParticleBody::get_internal_sample() const {
	return internal_sample;
}

void SoftParticleBody::set_particle_spacing(real_t p_value) {
	particle_spacing = p_value;
}

real_t SoftParticleBody::get_particle_spacing() const {
	return particle_spacing;
}

void SoftParticleBody::set_sampling(real_t p_value) {
	sampling = p_value;
}

real_t SoftParticleBody::get_sampling() const {
	return sampling;
}

void SoftParticleBody::set_cluster_spacing(real_t p_value) {
	cluster_spacing = p_value;
}

real_t SoftParticleBody::get_cluster_spacing() const {
	return cluster_spacing;
}

void SoftParticleBody::set_cluster_radius(real_t p_value) {
	cluster_radius = p_value;
}

real_t SoftParticleBody::get_cluster_radius() const {
	return cluster_radius;
}

void SoftParticleBody::set_cluster_stiffness(real_t p_value) {
	cluster_stiffness = p_value;
}

real_t SoftParticleBody::get_cluster_stiffness() const {
	return cluster_stiffness;
}

void SoftParticleBody::set_link_radius(real_t p_value) {
	link_radius = p_value;
}

real_t SoftParticleBody::get_link_radius() const {
	return link_radius;
}

void SoftParticleBody::set_link_stiffness(real_t p_value) {
	link_stiffness = p_value;
}

real_t SoftParticleBody::get_link_stiffness() const {
	return link_stiffness;
}

void SoftParticleBody::set_plastic_threshold(real_t p_value) {
	plastic_threshold = p_value;
}

real_t SoftParticleBody::get_plastic_threshold() const {
	return plastic_threshold;
}

void SoftParticleBody::set_plastic_creep(real_t p_value) {
	plastic_creep = p_value;
}

real_t SoftParticleBody::get_plastic_creep() const {
	return plastic_creep;
}

RigidParticleBody::RigidParticleBody() :
		ParticleBody(),
		radius(0.1),
		expand(0.05) {
}

void RigidParticleBody::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_radius", "value"), &RigidParticleBody::set_radius);
	ClassDB::bind_method(D_METHOD("get_radius"), &RigidParticleBody::get_radius);
	ClassDB::bind_method(D_METHOD("set_expand", "value"), &RigidParticleBody::set_expand);
	ClassDB::bind_method(D_METHOD("get_expand"), &RigidParticleBody::get_expand);

	ADD_PROPERTY(PropertyInfo(Variant::REAL, "model/radius"), "set_radius", "get_radius");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "model/expand"), "set_expand", "get_expand");
}

void RigidParticleBody::set_radius(real_t p_value) {
	radius = p_value;
}

real_t RigidParticleBody::get_radius() const {
	return radius;
}

void RigidParticleBody::set_expand(real_t p_value) {
	expand = p_value;
}

real_t RigidParticleBody::get_expand() const {
	return expand;
}

ClothParticleBody::ClothParticleBody() :
		ParticleBody(),
		stretch_stiffness(0.8),
		bend_stiffness(0.8),
		tether_stiffness(0.8),
		tether_give(0.8),
		rest_pressure(1) {}

void ClothParticleBody::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_stretch_stiffness", "value"), &ClothParticleBody::set_stretch_stiffness);
	ClassDB::bind_method(D_METHOD("get_stretch_stiffness"), &ClothParticleBody::get_stretch_stiffness);
	ClassDB::bind_method(D_METHOD("set_bend_stiffness", "value"), &ClothParticleBody::set_bend_stiffness);
	ClassDB::bind_method(D_METHOD("get_bend_stiffness"), &ClothParticleBody::get_bend_stiffness);
	ClassDB::bind_method(D_METHOD("set_tether_stiffness", "value"), &ClothParticleBody::set_tether_stiffness);
	ClassDB::bind_method(D_METHOD("get_tether_stiffness"), &ClothParticleBody::get_tether_stiffness);
	ClassDB::bind_method(D_METHOD("set_tether_give", "value"), &ClothParticleBody::set_tether_give);
	ClassDB::bind_method(D_METHOD("get_tether_give"), &ClothParticleBody::get_tether_give);
	ClassDB::bind_method(D_METHOD("set_rest_pressure", "value"), &ClothParticleBody::set_rest_pressure);
	ClassDB::bind_method(D_METHOD("get_rest_pressure"), &ClothParticleBody::get_rest_pressure);

	ClassDB::bind_method(D_METHOD("set_pressure", "pressure"), &ClothParticleBody::set_pressure);
	ClassDB::bind_method(D_METHOD("get_pressure"), &ClothParticleBody::get_pressure);

	ADD_PROPERTY(PropertyInfo(Variant::REAL, "model/stretch_stiffness"), "set_stretch_stiffness", "get_stretch_stiffness");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "model/bend_stiffness"), "set_bend_stiffness", "get_bend_stiffness");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "model/tether_stiffness"), "set_tether_stiffness", "get_tether_stiffness");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "model/tether_give"), "set_tether_give", "get_tether_give");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "model/rest_pressure"), "set_rest_pressure", "get_rest_pressure");

	ADD_PROPERTY(PropertyInfo(Variant::REAL, "pressure"), "set_pressure", "get_pressure");
}

void ClothParticleBody::set_stretch_stiffness(real_t p_value) {
	stretch_stiffness = p_value;
}

real_t ClothParticleBody::get_stretch_stiffness() const {
	return stretch_stiffness;
}

void ClothParticleBody::set_bend_stiffness(real_t p_value) {
	bend_stiffness = p_value;
}

real_t ClothParticleBody::get_bend_stiffness() const {
	return bend_stiffness;
}

void ClothParticleBody::set_tether_stiffness(real_t p_value) {
	tether_stiffness = p_value;
}

real_t ClothParticleBody::get_tether_stiffness() const {
	return tether_stiffness;
}

void ClothParticleBody::set_tether_give(real_t p_value) {
	tether_give = p_value;
}

real_t ClothParticleBody::get_tether_give() const {
	return tether_give;
}

void ClothParticleBody::set_rest_pressure(real_t p_value) {
	rest_pressure = p_value;
}

real_t ClothParticleBody::get_rest_pressure() const {
	return rest_pressure;
}

void ClothParticleBody::set_pressure(real_t p_pressure) {
	ParticlePhysicsServer::get_singleton()->body_set_pressure(rid, p_pressure);
}

real_t ClothParticleBody::get_pressure() const {
	return ParticlePhysicsServer::get_singleton()->body_get_pressure(rid);
}
