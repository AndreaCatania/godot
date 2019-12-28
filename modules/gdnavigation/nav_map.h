/*************************************************************************/
/*  rvo_space.h                                                          */
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

#ifndef RVO_SPACE_H
#define RVO_SPACE_H

#include "nav_rid.h"

#include "core/math/math_defs.h"
#include "nav_utils.h"
#include <KdTree.h>
#include <Obstacle.h>

class NavRegion;
class RvoAgent;
class NavRegion;

class NavMap : public NavRid {

    /// Map Up
    Vector3 up;

    /// To find the polygons edges the vertices are displaced in a grid where
    /// each cell has the following cell_size.
    float cell_size;

    /// This value is used to detect the near edges to connect.
    float edge_connection_margin;

    bool regenerate_polygons;
    bool regenerate_links;

    std::vector<NavRegion *> regions;

    /// Map polygons
    std::vector<Polygon> polygons;

    /// Rvo world
    RVO::KdTree rvo;

    /// Is agent array modified?
    bool agents_dirty;

    /// All the Agents (even the controlled one)
    std::vector<RvoAgent *> agents;

    /// Controlled agents
    std::vector<RvoAgent *> controlled_agents;

    /// Physics delta time
    real_t deltatime;

public:
    NavMap();

    void set_up(Vector3 p_up);
    Vector3 get_up() const {
        return up;
    }

    void set_cell_size(float p_cell_size);
    float get_cell_size() const {
        return cell_size;
    }

    void set_edge_connection_margin(float p_edge_connection_margin);
    float get_edge_connection_margin() const {
        return edge_connection_margin;
    }

    PointKey get_point_key(const Vector3 &p_pos) const;

    Vector<Vector3> get_path(Vector3 p_origin, Vector3 p_destination, bool p_optimize) const;

    void add_region(NavRegion *p_region);
    void remove_region(NavRegion *p_region);

    bool has_agent(RvoAgent *agent) const;
    void add_agent(RvoAgent *agent);
    void remove_agent(RvoAgent *agent);
    std::vector<RvoAgent *> &get_agents() {
        return agents;
    }

    void set_agent_as_controlled(RvoAgent *agent);
    void remove_agent_as_controlled(RvoAgent *agent);

    void sync();
    void step(real_t p_deltatime);
    void dispatch_callbacks();

private:
    void compute_single_step(uint32_t index, RvoAgent **agent);
    void clip_path(const std::vector<NavigationPoly> &p_navigation_polys, Vector<Vector3> &path, const NavigationPoly *from_poly, const Vector3 &p_to_point, const NavigationPoly *p_to_poly) const;
};

#endif // RVO_SPACE_H
