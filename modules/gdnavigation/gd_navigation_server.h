/*************************************************************************/
/*  gd_navigation_server.h                                               */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2019 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2019 Godot Engine contributors (cf. AUTHORS.md)    */
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

#ifndef GD_NAVIGATION_SERVER_H
#define GD_NAVIGATION_SERVER_H

#include "servers/navigation_server.h"

#include "nav_map.h"
#include "nav_region.h"
#include "rvo_agent.h"
#include "rvo_obstacle.h"

/// The commands are functions executed during the `sync` phase.

#define COMMAND_2(F_NAME, T_0, D_0, T_1, D_1)    \
    virtual void F_NAME(T_0 D_0, T_1 D_1) const; \
    void __CONCAT(_cmd_, F_NAME)(T_0 D_0, T_1 D_1)

#define COMMAND_4_DEF(F_NAME, T_0, D_0, T_1, D_1, T_2, D_2, T_3, D_3, D_3_DEF) \
    virtual void F_NAME(T_0 D_0, T_1 D_1, T_2 D_2, T_3 D_3 = D_3_DEF) const;   \
    void __CONCAT(_cmd_, F_NAME)(T_0 D_0, T_1 D_1, T_2 D_2, T_3 D_3)

class GdNavigationServer;

struct SetCommand {
    virtual void exec(GdNavigationServer *server) = 0;
};

class GdNavigationServer : public NavigationServer {
    std::vector<SetCommand *> commands;

    mutable RID_Owner<NavMap> map_owner;
    mutable RID_Owner<NavRegion> region_owner;
    mutable RID_Owner<RvoAgent> agent_owner;

    bool active;
    Vector<NavMap *> active_maps;

public:
    GdNavigationServer();
    virtual ~GdNavigationServer();

    void add_command(SetCommand *command);

    virtual RID map_create();
    COMMAND_2(map_set_active, RID, p_map, bool, p_active);
    virtual bool map_is_active(RID p_map) const;

    COMMAND_2(map_set_up, RID, p_map, Vector3, p_up);
    virtual Vector3 map_get_up(RID p_map) const;

    COMMAND_2(map_set_cell_size, RID, p_map, real_t, p_cell_size);
    virtual real_t map_get_cell_size(RID p_map) const;

    COMMAND_2(map_set_edge_connection_margin, RID, p_map, real_t, p_connection_margin);
    virtual real_t map_get_edge_connection_margin(RID p_map) const;

    virtual Vector<Vector3> map_get_path(RID p_map, Vector3 p_origin, Vector3 p_destination, bool p_optimize) const;

    virtual RID region_create();
    COMMAND_2(region_set_map, RID, p_region, RID, p_map);
    COMMAND_2(region_set_transform, RID, p_region, Transform, p_transform);
    COMMAND_2(region_set_navmesh, RID, p_region, Ref<NavigationMesh>, p_nav_mesh);

    virtual RID agent_create();
    COMMAND_2(agent_set_map, RID, p_agent, RID, p_map);
    COMMAND_2(agent_set_neighbor_dist, RID, p_agent, real_t, p_dist);
    COMMAND_2(agent_set_max_neighbors, RID, p_agent, int, p_count);
    COMMAND_2(agent_set_time_horizon, RID, p_agent, real_t, p_time);
    COMMAND_2(agent_set_radius, RID, p_agent, real_t, p_radius);
    COMMAND_2(agent_set_max_speed, RID, p_agent, real_t, p_max_speed);
    COMMAND_2(agent_set_velocity, RID, p_agent, Vector3, p_velocity);
    COMMAND_2(agent_set_target_velocity, RID, p_agent, Vector3, p_velocity);
    COMMAND_2(agent_set_position, RID, p_agent, Vector3, p_position);
    COMMAND_4_DEF(agent_set_callback, RID, p_agent, Object *, p_receiver, StringName, p_method, Variant, p_udata, Variant());

    virtual void free(RID p_object);

    virtual void set_active(bool p_active);
    virtual void step(real_t p_delta_time);
};

#endif // GD_NAVIGATION_SERVER_H
