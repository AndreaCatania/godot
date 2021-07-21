/*************************************************************************/
/*  register_types.cpp                                                   */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2021 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2021 Godot Engine contributors (cf. AUTHORS.md).   */
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
	@author AndreaCatania
*/

#include "register_types.h"

#include "data_buffer.h"
#include "interpolator.h"
#include "networked_controller.h"
#include "scene_diff.h"
#include "scene_synchronizer.h"

void register_network_synchronizer_types() {
	NetworkedController::sn_rpc_server_send_inputs = SNAME("_rpc_server_send_inputs");
	NetworkedController::sn_rpc_send_tick_additional_speed = SNAME("_rpc_send_tick_additional_speed");
	NetworkedController::sn_rpc_doll_notify_sync_pause = SNAME("_rpc_doll_notify_sync_pause");
	NetworkedController::sn_rpc_doll_send_epoch_batch = SNAME("_rpc_doll_send_epoch_batch");

	NetworkedController::sn_controller_process = SNAME("_controller_process");
	NetworkedController::sn_count_input_size = SNAME("_count_input_size");
	NetworkedController::sn_are_inputs_different = SNAME("_are_inputs_different");
	NetworkedController::sn_collect_epoch_data = SNAME("_collect_epoch_data");
	NetworkedController::sn_collect_inputs = SNAME("_collect_inputs");
	NetworkedController::sn_setup_interpolator = SNAME("_setup_interpolator");
	NetworkedController::sn_parse_epoch_data = SNAME("_parse_epoch_data");
	NetworkedController::sn_apply_epoch = SNAME("_apply_epoch");

	GDREGISTER_CLASS(DataBuffer);
	GDREGISTER_CLASS(SceneDiff);
	GDREGISTER_CLASS(Interpolator);
	GDREGISTER_CLASS(NetworkedController);
	GDREGISTER_CLASS(SceneSynchronizer);

	GLOBAL_DEF("NetworkSynchronizer/debug_server_speedup", false);
	GLOBAL_DEF("NetworkSynchronizer/debug_doll_speedup", false);
}

void unregister_network_synchronizer_types() {
	// Cleanup the used `StringName` here so to avoid that the destructor is
	// called too late.
	NetworkedController::sn_rpc_server_send_inputs = StringName();
	NetworkedController::sn_rpc_send_tick_additional_speed = StringName();
	NetworkedController::sn_rpc_doll_notify_sync_pause = StringName();
	NetworkedController::sn_rpc_doll_send_epoch_batch = StringName();

	NetworkedController::sn_controller_process = StringName();
	NetworkedController::sn_count_input_size = StringName();
	NetworkedController::sn_are_inputs_different = StringName();
	NetworkedController::sn_collect_epoch_data = StringName();
	NetworkedController::sn_collect_inputs = StringName();
	NetworkedController::sn_setup_interpolator = StringName();
	NetworkedController::sn_parse_epoch_data = StringName();
	NetworkedController::sn_apply_epoch = StringName();
}
