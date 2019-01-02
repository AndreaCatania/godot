/*************************************************************************/
/*  physics_particle_body_mesh_instance.h                                */
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

#include "physics_particle_body_mesh_instance.h"

#include "core/engine.h"
#include "scene/3d/physics_particle_body.h"
#include "scene/3d/skeleton.h"

ParticleClothVisualServerHandler::ParticleClothVisualServerHandler() {}

void ParticleClothVisualServerHandler::prepare(RID p_mesh, int p_surface, const Array &p_mesh_arrays) {
	clear();

	ERR_FAIL_COND(!p_mesh.is_valid());

	mesh = p_mesh;
	surface = p_surface;

	const uint32_t surface_format = VS::get_singleton()->mesh_surface_get_format(mesh, surface);
	const int surface_vertex_len = VS::get_singleton()->mesh_surface_get_array_len(mesh, p_surface);
	const int surface_index_len = VS::get_singleton()->mesh_surface_get_array_index_len(mesh, p_surface);
	uint32_t surface_offsets[VS::ARRAY_MAX];

	buffer = VS::get_singleton()->mesh_surface_get_array(mesh, surface);
	stride = VS::get_singleton()->mesh_surface_make_offsets_from_format(surface_format, surface_vertex_len, surface_index_len, surface_offsets);
	offset_vertices = surface_offsets[VS::ARRAY_VERTEX];
	offset_normal = surface_offsets[VS::ARRAY_NORMAL];

	mesh_indices = p_mesh_arrays[VS::ARRAY_INDEX];
}

void ParticleClothVisualServerHandler::clear() {

	if (mesh.is_valid()) {
		buffer.resize(0);
	}

	mesh = RID();
}

void ParticleClothVisualServerHandler::open() {
	write_buffer = buffer.write();
}

void ParticleClothVisualServerHandler::close() {
	write_buffer = PoolVector<uint8_t>::Write();
}

void ParticleClothVisualServerHandler::commit_changes() {
	VS::get_singleton()->mesh_surface_update_region(mesh, surface, 0, buffer);
}

void ParticleClothVisualServerHandler::set_vertex(int p_vertex, const void *p_vector3) {
	copymem(&write_buffer[p_vertex * stride + offset_vertices], p_vector3, sizeof(float) * 3);
}

void ParticleClothVisualServerHandler::set_normal(int p_vertex, const void *p_vector3) {
	copymem(&write_buffer[p_vertex * stride + offset_normal], p_vector3, sizeof(float) * 3);
}

void ParticleClothVisualServerHandler::set_aabb(const AABB &p_aabb) {
	VS::get_singleton()->mesh_set_custom_aabb(mesh, p_aabb);
}

void ParticleBodyMeshInstance::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_look_y_previous_cluster", "look"), &ParticleBodyMeshInstance::set_look_y_previous_cluster);
	ClassDB::bind_method(D_METHOD("get_look_y_previous_cluster"), &ParticleBodyMeshInstance::get_look_y_previous_cluster);

	ClassDB::bind_method(D_METHOD("set_weight_fall_off", "fall_off"), &ParticleBodyMeshInstance::set_weight_fall_off);
	ClassDB::bind_method(D_METHOD("get_weight_fall_off"), &ParticleBodyMeshInstance::get_weight_fall_off);

	ClassDB::bind_method(D_METHOD("set_weight_max_distance", "max_distance"), &ParticleBodyMeshInstance::set_weight_max_distance);
	ClassDB::bind_method(D_METHOD("get_weight_max_distance"), &ParticleBodyMeshInstance::get_weight_max_distance);

	ClassDB::bind_method(D_METHOD("_draw_mesh_pvparticles"), &ParticleBodyMeshInstance::_draw_mesh_pvparticles);

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "look_y_previous_cluster"), "set_look_y_previous_cluster", "get_look_y_previous_cluster");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "weight_fall_off"), "set_weight_fall_off", "get_weight_fall_off");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "weight_max_distance"), "set_weight_max_distance", "get_weight_max_distance");
}

void ParticleBodyMeshInstance::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {

			ERR_FAIL_COND(particle_body);

			if (!Engine::get_singleton()->is_editor_hint()) {

				set_as_toplevel(true);
				set_global_transform(Transform());
			}

			particle_body = Object::cast_to<ParticleBody>(get_parent());
			if (particle_body) {
				particle_body->set_particle_body_mesh(this);
				prepare_mesh_for_rendering();
			}

		} break;
		case NOTIFICATION_LOCAL_TRANSFORM_CHANGED: {

			if (!Engine::get_singleton()->is_editor_hint())
				return;

			if (!particle_body)
				return;
			particle_body->set_global_transform(get_global_transform());
			set_notify_local_transform(false);
			set_transform(Transform());
			set_notify_local_transform(true);
		} break;
		case NOTIFICATION_EXIT_TREE: {
			if (particle_body) {
				particle_body->set_particle_body_mesh(NULL);
			}

			_clear_pvparticles_drawing();
		} break;
	}
}

ParticleBodyMeshInstance::ParticleBodyMeshInstance() :
		MeshInstance(),
		look_y_previous_cluster(false),
		weight_fall_off(2),
		weight_max_distance(100.0),
		particle_body(NULL),
		skeleton(NULL),
		rendering_approach(RENDERING_UPDATE_APPROACH_NONE),
		visual_server_handler(NULL) {

	set_skeleton_path(NodePath());
	if (Engine::get_singleton()->is_editor_hint())
		set_notify_local_transform(true);
}

ParticleBodyMeshInstance::~ParticleBodyMeshInstance() {
	_clear_pvparticles_drawing();
}

void ParticleBodyMeshInstance::set_look_y_previous_cluster(bool look) {
	look_y_previous_cluster = look;
}

void ParticleBodyMeshInstance::set_weight_fall_off(real_t p_weight_fall_off) {
	weight_fall_off = p_weight_fall_off;
}

void ParticleBodyMeshInstance::set_weight_max_distance(real_t p_weight_max_distance) {
	weight_max_distance = p_weight_max_distance;
}

void ParticleBodyMeshInstance::update_mesh(ParticleBodyCommands *p_cmds) {
	switch (rendering_approach) {
		case RENDERING_UPDATE_APPROACH_PVP:
			update_mesh_pvparticles(p_cmds);
			break;
		case RENDERING_UPDATE_APPROACH_SKELETON:
			update_mesh_skeleton(p_cmds);
			break;
	}
}

void ParticleBodyMeshInstance::update_mesh_pvparticles(ParticleBodyCommands *p_cmds) {

	const ParticlePhysicsServer::TearingData *tearing_data =
			ParticlePhysicsServer::get_singleton()->body_get_tearing_data(
					particle_body->get_rid());

	const int split_count = tearing_data->splits.size();
	if (tearing_data && split_count) {

		Ref<Material> material(get_surface_material(0));

		Array surface_arrays = get_mesh()->surface_get_arrays(0);
		Array surface_blend_arrays = get_mesh()->surface_get_blend_shape_arrays(0);
		uint32_t surface_format = get_mesh()->surface_get_format(0);

		// Recreate the mesh with new vertices

		PoolVector<Vector3> vertices = surface_arrays[VS::ARRAY_VERTEX];
		PoolVector<Vector3> normals = surface_arrays[VS::ARRAY_NORMAL];
		PoolVector<real_t> tangents = surface_arrays[VS::ARRAY_TANGENT];
		PoolVector<Color> colors = surface_arrays[VS::ARRAY_COLOR];
		PoolVector<Vector3> uvs = surface_arrays[VS::ARRAY_TEX_UV];
		PoolVector<Vector3> uvs2 = surface_arrays[VS::ARRAY_TEX_UV2];
		PoolVector<int> indices = surface_arrays[VS::ARRAY_INDEX];

		const int ini_size_v = vertices.size();
		const int ini_size_n = normals.size();
		const int ini_size_t = tangents.size();
		const int ini_size_c = colors.size();
		const int ini_size_u = uvs.size();
		const int ini_size_u2 = uvs2.size();

		if (ini_size_v)
			vertices.resize(ini_size_v + split_count);

		if (ini_size_n)
			normals.resize(ini_size_n + split_count);

		if (ini_size_t)
			tangents.resize(ini_size_t + split_count * 4);

		if (ini_size_c)
			colors.resize(ini_size_c + split_count);

		if (ini_size_u)
			uvs.resize(ini_size_u + split_count);

		if (ini_size_u2)
			uvs2.resize(ini_size_u2 + split_count);

		{
			PoolVector<Vector3>::Write vertices_w = vertices.write();
			PoolVector<Vector3>::Write normals_w = normals.write();
			PoolVector<real_t>::Write tangents_w = tangents.write();
			PoolVector<Color>::Write colors_w = colors.write();
			PoolVector<Vector3>::Write uvs_w = uvs.write();
			PoolVector<Vector3>::Write uvs2_w = uvs2.write();
			PoolVector<int>::Write indices_w = indices.write();

			PoolVector<Vector3>::Read vertices_r = vertices.read();
			PoolVector<Vector3>::Read normals_r = normals.read();
			PoolVector<real_t>::Read tangents_r = tangents.read();
			PoolVector<Color>::Read colors_r = colors.read();
			PoolVector<Vector3>::Read uvs_r = uvs.read();
			PoolVector<Vector3>::Read uvs2_r = uvs2.read();

			// Duplicate phase
			for (
					int s(split_count - 1), new_vertex(ini_size_v);
					0 <= s;
					--s, ++new_vertex) {

				int old_vertex(-1);

				// Find old vertex index
				for (int x(pv_mapped_particle_indices.size() - 1); 0 <= x; --x) {
					if (pv_mapped_particle_indices[x] == tearing_data->splits[s].previous_p_index) {
						old_vertex = x;
						break;
					}
				}

				ERR_FAIL_COND(-1 == old_vertex);

				if (ini_size_v)
					vertices_w[new_vertex] = vertices_r[old_vertex];

				if (ini_size_n)
					normals_w[new_vertex] = normals_r[old_vertex];

				if (ini_size_t) {
					tangents_w[new_vertex * 4 + 0] = tangents_r[old_vertex * 4 + 0];
					tangents_w[new_vertex * 4 + 1] = tangents_r[old_vertex * 4 + 1];
					tangents_w[new_vertex * 4 + 2] = tangents_r[old_vertex * 4 + 2];
					tangents_w[new_vertex * 4 + 3] = tangents_r[old_vertex * 4 + 3];
				}

				if (ini_size_c)
					colors_w[new_vertex] = colors_r[old_vertex];

				if (ini_size_u)
					uvs_w[new_vertex] = uvs_r[old_vertex];

				if (ini_size_u2)
					uvs2_w[new_vertex] = uvs2_r[old_vertex];

				// Reindex

				for (int t(tearing_data->triangles.size() - 1); 0 <= t; --t) {
					if (tearing_data->triangles[t].a == tearing_data->splits[s].new_p_index) {
						indices_w[t * 3 + 0] = new_vertex;
						continue;
					}

					if (tearing_data->triangles[t].b == tearing_data->splits[s].new_p_index) {
						indices_w[t * 3 + 1] = new_vertex;
						continue;
					}

					if (tearing_data->triangles[t].c == tearing_data->splits[s].new_p_index) {
						indices_w[t * 3 + 2] = new_vertex;
						continue;
					}
				}

				pv_mapped_particle_indices.push_back(
						tearing_data->splits[s].new_p_index);
			}
		}

		surface_arrays[VS::ARRAY_VERTEX] = vertices;
		surface_arrays[VS::ARRAY_NORMAL] = normals;
		surface_arrays[VS::ARRAY_TANGENT] = tangents;

		if (colors.size())
			surface_arrays[VS::ARRAY_COLOR] = colors;
		else
			surface_arrays[VS::ARRAY_COLOR] = Variant();

		surface_arrays[VS::ARRAY_TEX_UV] = uvs;

		if (uvs2.size())
			surface_arrays[VS::ARRAY_TEX_UV2] = uvs2;
		else
			surface_arrays[VS::ARRAY_TEX_UV2] = Variant();

		surface_arrays[VS::ARRAY_INDEX] = indices;
		surface_arrays[VS::ARRAY_BONES] = Variant();
		surface_arrays[VS::ARRAY_WEIGHTS] = Variant();

		Ref<ArrayMesh> soft_mesh;
		soft_mesh.instance();
		soft_mesh->add_surface_from_arrays(
				Mesh::PRIMITIVE_TRIANGLES,
				surface_arrays,
				surface_blend_arrays,
				surface_format);

		soft_mesh->surface_set_material(0, get_mesh()->surface_get_material(0));

		set_mesh(soft_mesh);

		set_surface_material(0, material);

		visual_server_handler->prepare(soft_mesh->get_rid(), 0, surface_arrays);
	}

	visual_server_handler->open();

	Vector3 v;
	for (int i(pv_mapped_particle_indices.size() - 1); 0 <= i; --i) {

		v = p_cmds->get_particle_position(pv_mapped_particle_indices[i]);
		visual_server_handler->set_vertex(i, reinterpret_cast<void *>(&v));

		v = p_cmds->get_particle_normal(pv_mapped_particle_indices[i]) * -1;
		visual_server_handler->set_normal(i, reinterpret_cast<void *>(&v));
	}

	visual_server_handler->close();

	visual_server_handler->set_aabb(p_cmds->get_aabb());
}

void ParticleBodyMeshInstance::_draw_mesh_pvparticles() {

	// The buffer is updated in the update_mesh_pvparticles
	visual_server_handler->commit_changes();
}

void ParticleBodyMeshInstance::update_mesh_skeleton(ParticleBodyCommands *p_cmds) {

	const int rigids_count = ParticlePhysicsServer::get_singleton()->body_get_rigid_count(particle_body->get_rid());
	const PoolVector<Vector3>::Read rigids_local_pos_r = particle_body->get_particle_body_model()->get_clusters_positions().read();

	Vector3 initial_cluster_x;
	if (look_y_previous_cluster && rigids_count) {

		initial_cluster_x = Basis(p_cmds->get_rigid_rotation(0)).xform(Vector3(1, 0, 0));
	}

	for (int i = 0; i < rigids_count; ++i) {

		Basis b;
		if (look_y_previous_cluster && i != 0) {

			Vector3 delta = p_cmds->get_rigid_position(i) - p_cmds->get_rigid_position(i - 1);
			delta.normalize();

			Vector3 xa = initial_cluster_x;
			Vector3 ya = delta;
			Vector3 za = xa.cross(ya).normalized();
			xa = ya.cross(za).normalized();
			b.set(xa, ya, za);
		} else {

			b.set_quat(p_cmds->get_rigid_rotation(i));
		}

		Transform t(b, p_cmds->get_rigid_position(i));
		t.translate(rigids_local_pos_r[i] * -1);
		skeleton->set_bone_pose(i, t);
	}

	VS::get_singleton()->mesh_set_custom_aabb(mesh->get_rid(), p_cmds->get_aabb());
}

void ParticleBodyMeshInstance::prepare_mesh_for_rendering() {

	if (Engine::get_singleton()->is_editor_hint())
		return;

	Ref<ParticleBodyModel> model = particle_body->get_particle_body_model();
	if (model.is_null())
		return;

	if (model->get_clusters_positions().size())
		prepare_mesh_skeleton_deformation();

	else if (model->get_dynamic_triangles_indices().size())
		prepare_mesh_for_pvparticles();
}

void ParticleBodyMeshInstance::prepare_mesh_for_pvparticles() {

	Ref<ParticleBodyModel> model = particle_body->get_particle_body_model();
	if (model.is_null())
		return;

	ERR_FAIL_COND(!model->get_dynamic_triangles_indices().size());

	ERR_FAIL_COND(!get_mesh()->get_surface_count());

	Ref<Material> material(get_surface_material(0));

	// Get current mesh array and create new mesh array with necessary flag for softbody
	Array surface_arrays = get_mesh()->surface_get_arrays(0);
	Array surface_blend_arrays = get_mesh()->surface_get_blend_shape_arrays(0);
	uint32_t surface_format = get_mesh()->surface_get_format(0);

	surface_format &= ~(Mesh::ARRAY_COMPRESS_VERTEX | Mesh::ARRAY_COMPRESS_NORMAL);
	surface_format |= Mesh::ARRAY_FLAG_USE_DYNAMIC_UPDATE;

	Ref<ArrayMesh> soft_mesh;
	soft_mesh.instance();
	soft_mesh->add_surface_from_arrays(
			Mesh::PRIMITIVE_TRIANGLES,
			surface_arrays,
			surface_blend_arrays,
			surface_format);

	soft_mesh->surface_set_material(0, get_mesh()->surface_get_material(0));

	set_mesh(soft_mesh);

	set_surface_material(0, material);

	rendering_approach = RENDERING_UPDATE_APPROACH_PVP;

	ERR_FAIL_COND(visual_server_handler);
	visual_server_handler = memnew(ParticleClothVisualServerHandler);
	visual_server_handler->prepare(get_mesh()->get_rid(), 0, surface_arrays);

	/// Necessary in order to render the mesh correctly (Soft body nodes are in global space)
	call_deferred("set_as_toplevel", true);
	call_deferred("set_transform", Transform());

	VS::get_singleton()->connect("frame_pre_draw", this, "_draw_mesh_pvparticles");

	// Map indices
	PoolVector<int>::Read mesh_indices_r = visual_server_handler->get_mesh_indices().read();

	PoolVector<int>::Read pb_indices_r = particle_body->get_particle_body_model()->get_dynamic_triangles_indices().read();

	int biggest_mesh_index(-1);
	for (int i(visual_server_handler->get_mesh_indices().size() - 1); 0 <= i; --i) {
		if (biggest_mesh_index < mesh_indices_r[i])
			biggest_mesh_index = mesh_indices_r[i];
	}

	pv_mapped_particle_indices.resize(biggest_mesh_index + 1);
	for (int i(visual_server_handler->get_mesh_indices().size() - 1); 0 <= i; --i) {
		pv_mapped_particle_indices.write[mesh_indices_r[i]] = pb_indices_r[i];
	}
}

void ParticleBodyMeshInstance::prepare_mesh_skeleton_deformation() {

	if (Engine::get_singleton()->is_editor_hint())
		return;

	if (get_mesh().is_null())
		return;

	Ref<ParticleBodyModel> model = particle_body->get_particle_body_model();
	if (model.is_null())
		return;

	ERR_FAIL_COND(!model->get_clusters_positions().size());
	ERR_FAIL_COND(get_mesh()->get_surface_count() != 1);

	Ref<Material> material(get_surface_material(0));

	const int surface_id = 0;

	PoolVector<Vector3>::Read clusters_pos_r = model->get_clusters_positions().read();
	const int bone_count(model->get_clusters_positions().size());

	Array array_mesh = get_mesh()->surface_get_arrays(surface_id).duplicate();
	PoolVector<Vector3> vertices = array_mesh[VS::ARRAY_VERTEX];
	PoolVector<Vector3>::Read vertices_read = vertices.read();
	const int vertex_count(vertices.size());

	PoolVector<float> weights; // The weight value for vertex
	PoolVector<int> bone_indices; // The index of bone relative of vertex
	int max_weights_per_vertex = 0;

	ParticlePhysicsServer::get_singleton()->create_skeleton(weight_fall_off, weight_max_distance, clusters_pos_r.ptr(), bone_count, vertices_read.ptr(), vertex_count, &weights, &bone_indices, &max_weights_per_vertex);

	ERR_FAIL_COND(max_weights_per_vertex != VS::ARRAY_WEIGHTS_SIZE);

	if (!skeleton) {
		skeleton = memnew(Skeleton);
		add_child(skeleton);
		set_skeleton_path(skeleton->get_path());
	} else {
		skeleton->clear_bones();
	}

	for (int i(0); i < bone_count; ++i) {
		skeleton->add_bone("particle_" + String::num(i));
		skeleton->set_bone_disable_rest(i, true);
	}

	// Create skeleton using these info:
	array_mesh[VS::ARRAY_WEIGHTS] = weights;
	array_mesh[VS::ARRAY_BONES] = bone_indices;

	Ref<ArrayMesh> new_mesh;
	new_mesh.instance();
	new_mesh->add_surface_from_arrays(get_mesh()->surface_get_primitive_type(surface_id), array_mesh);
	new_mesh->surface_set_material(0, mesh->surface_get_material(0));

	set_mesh(new_mesh);

	set_surface_material(0, material);

	rendering_approach = RENDERING_UPDATE_APPROACH_SKELETON;

	_clear_pvparticles_drawing();
}

void ParticleBodyMeshInstance::_clear_pvparticles_drawing() {

	if (VS::get_singleton()->is_connected("frame_pre_draw", this, "_draw_mesh_pvparticles"))
		VS::get_singleton()->disconnect("frame_pre_draw", this, "_draw_mesh_pvparticles");

	if (!visual_server_handler)
		return;

	visual_server_handler->clear();
	memdelete(visual_server_handler);
	visual_server_handler = NULL;
}
