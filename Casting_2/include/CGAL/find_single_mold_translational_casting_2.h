// Copyright (c) 2005,2006,2007,2008,2009,2010,2011 Tel-Aviv University (Israel).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Author(s): Shahar    <shasha94@gmail.com>
//            Efi Fogel <efif@gmail.com>

#ifndef CGAL_FIND_SINGLE_MOLD_TRANSLATIONAL_CASTING_2_H
#define CGAL_FIND_SINGLE_MOLD_TRANSLATIONAL_CASTING_2_H
#include <CGAL/Polygon_2.h>
#include <CGAL/enum.h>

#include <iostream>
#include<list>

/* Legend:
 * point = Represented as Direction_2. It is the intersection between the
 *   fitting Direction_2 and the unit circle
 *
 * Arc = Represented as A pair of point. clockwise arc between the first
 *   point and the second point. (each of its sides might be open or closed)
 *
 * SegmentOuterCircle  = Arc that represent all the directions that points
 *   out from the polygon if it start from the
 *   fitting segment. This arc is always open half circle.
 */

/*! \Circle_arrangment
 * \brief This class represents an subdivision of the unit-circle into cells of
 * depth 0,1,2+ where depth is the number of inserted open half-circles inserted
 * that covers this cell in addition this class contains some static functions
 * that are in this class inorder of sharing its typedefs all the circle is
 * always covered by some cell. there can't be an hole
 */
template <typename Kernel>
class Circle_arrangment {
  typedef typename Kernel::Direction_2          Point;
  typedef std::pair<Point, Point>               Arc;
  /* Legend:
   * Point = Represented as Direction_2. It is the intersection between the
   *   fitting Direction_2 and the unit circle
   *
   * Arc = Represented as a pair of points. clockwise arc between the first
   *   point and the second point. (each of its sides might be open or closed)
   */

  /*! \fn bool is_open_direction_contained_in_arc(point p, bool is_counterclockwise, Arc a)
   * Checks whether an open epsilon area clockwise/counterclockwise from a point
   * p is contained in an arc s.
   * \param p a point .
   * \param is_counterclockwise true: we care about the counterclockwise epsilon
   *        area of p. false: same with clockwise
   * \param a an arc that should contain the epsilon area
   */
  static bool is_open_direction_contained_in_arc(Point p,
                                                 bool is_counterclockwise,
                                                 Arc a)
  {
    if ((is_counterclockwise && p == a.second) ||
        (!is_counterclockwise && p == a.first))
      return false;
    return !p.counterclockwise_in_between(a.first,a.second);
  }
  /*! \fn bool is_a_contained_in_b(bool is_a_start_closed,bool is_a_end_closed, arc A, arc B)
   * \brief checks whether an arc A is contained in an arc B
   * \param is_a_start_closed - do A contains its start point (clockwise)
   * \param is_a_end_closed - do A contains its end point (clockwise)
   * \param A - an arc
   * \param B - an *open* arc
   */
  static bool is_a_contained_in_b(bool is_a_start_closed,
                                  bool is_a_end_closed,
                                  Arc A, Arc B)
  {
    //A is closed, B is open and they share an vertex -> A not contained in B
    if ((is_a_start_closed &&A.first == B.first) ||
        (is_a_end_closed && A.second == B.second))
      return false;
    if (((A.first == B.second) || (B.first == A.second)) &&
        (A.first != A.second))
      return false;

    return (!A.first.counterclockwise_in_between(B.first, B.second) &&
            !A.second.counterclockwise_in_between(B.first, B.second) &&
            !A.first.counterclockwise_in_between(B.first, A.second));
  }

  /*! \Circle_arrangment_edge
   * This class represents a cells (a point or an arc) of depth 0,1,2+ in the
   * Circle_arrangment where depth the number of inserted open half-circles
   * inserted that cover this cell
   * This edge (cell) is described by the first point of the edge (clockwise).
   * The last point can be deduced by the next instance of
   * Circle_arrangment_edge in the list in Circle_arrangment
   * this class also keeps the cell depth.
   */
  class Circle_arrangment_edge {
  public:
    bool start_is_closed;
    Point edge_start_angle; // the end is the start of the next edge
    char count;             // no. of outer circles that cover the edge (0/1/2+)
    size_t edge_index;      // the index of the polygon edge the open
                            // half-circle of which covers this cell.
                            // only relevant if count ==1

    /*! \ctor Circle_arrangment_edge(point edge_start_angle, size_t edge_index, bool start_is_closed,bool set_count_to_one=true)
     * Creates a new edge (Arc), this edge count must be 0 or 1
     * \param edge_start_angle - the first point of the arc (clockwise)
     * \param edge_index - the index of the polygon edge who's open half-circle
     *        covers this cell - only relevant if count == 1
     * \param start_is_closed - is the point edge_start_angle contained in this
     *        cell
     * \param set_count_to_one - to set the count to one (or zero if this var is
     *        false)
     */
    Circle_arrangment_edge(Point edge_start_angle, size_t edge_index,
                           bool start_is_closed, bool set_count_to_one = true)
    {
      this->start_is_closed = start_is_closed;
      this->edge_start_angle = edge_start_angle;
      this->count = (int) set_count_to_one;
      this->edge_index = edge_index;
    }

    /*! \fn void plusplus(size_t edge_index)
     * Adds new polygon edge who's open half-circle covers this cell
     * \param edge_index - the index of this edge
     * increase the edge count by one (if it is 2+, it will stay 2+)
     * set this new edge to be the one covers the cell if the count was zero
     * before. (only relevant if now count == 1)
     */
    void plusplus(size_t edge_index) {
      if (this->count ==0) {
        this->edge_index = edge_index;
        this->count = 1;
      }
      else if(this->count ==1) this->count = 2;
    }
    bool is_covered() { return count == 2; }
  };

  typedef typename std::list<struct Circle_arrangment_edge> Circle_edges;

  Circle_edges edges;

  /*! \fn void insert_if_legal(const Circle_edge_iterator cur_it,const Circle_edge_iterator next_it,const struct Circle_arrangment_edge &edge)
   * Adds new edge to the arrangement if it won't create some empty edges
   * \param cur_it iterator to the edge before where the new edge should be
   *        inserted
   * \param next_it - iterator to the edge after where the new edge should be
   *        inserted
   * \param edge - the new edge that should be inserted
   *
   * Notice that next_it is redundant since it can be deduced from cur_it.
   * But it was easier for me to just send it as well.
   */
  template <typename InputIterator>
  void insert_if_legal(const InputIterator cur_it,
                       const InputIterator next_it,
                       const struct Circle_arrangment_edge& edge)
  {
    if ((edge.start_is_closed && !next_it->start_is_closed) ||
       edge.edge_start_angle != next_it->edge_start_angle)
      if ((cur_it->start_is_closed && !edge.start_is_closed) ||
         edge.edge_start_angle != cur_it->edge_start_angle)
        edges.insert(next_it, edge);
  }

  /*! \fn void merge_adjacent_2_edges_and_remove_empty()
   * \brief merge all the arcs that are adjacent and of depth 2+
   * it doesn't merge the first and last ones since it is easier this way.
   */
  void merge_adjacent_2_edges_and_remove_empty()
  {
    bool in_two_edge(false);
    for (auto it = edges.begin(); it != edges.end();) {
      if (it->is_covered()) {
        if (in_two_edge) {
          it = edges.erase(it);
          continue;
        }
        in_two_edge = true;
      }
      else {
        in_two_edge = false;
      }
      ++it;
    }
  }

public:
  /*! \ctor Circle_arrangment(arc first_segment_outer_circle)
   * Creates an arrangement on circle with two edges the one covered by
   * first_segment_outer_circle and the other one
   * \param first_segment_outer_circle - the outer circle of the first segment
   *        of the polygon.
   * Notice that you might consider implementing the ctor as an full circle of
   * depth 0, but it was much easier for me to ignore the case where the all
   * circle is a single arc, so I choose this implementation.
   */
  Circle_arrangment(Arc first_segment_outer_circle)
  {
    edges.push_back(Circle_arrangment_edge(first_segment_outer_circle.first, 0,
                                           false));
    edges.push_back(Circle_arrangment_edge(first_segment_outer_circle.second, 0,
                                           true,false));
  }

  /*! \fn add_segment_outer_circle(arc segment_outer_circle, size_t edge_index)
   * Updates the arrangement in respect to a new segment outer open circle
   * \param segment_outer_circle - the outer circle of the current segment of
   *        the polygon.
   * \param edge_index - this segment id
   * This is the main funtion of this code. It separates the cells in which the
   * endpoints of the new arc is contained to two parts and increase count
   * for all the cells that the new arc covers. In the end the function
   * merge_adjacent_2_edges_and_remove_empty is called to remove redundant cells
   */
  void add_segment_outer_circle(Arc segment_outer_circle, size_t edge_index)
  {
    Arc edge;
    bool is_start_closed_segment = edges.begin()->start_is_closed;
    bool is_end_closed_segment = edges.begin()->start_is_closed;
    edge.first = edges.begin()->edge_start_angle;
    edge.second = edges.begin()->edge_start_angle;
    auto next_it = edges.begin();
    auto it = edges.begin();
    bool done = false;
    while (!done) {
      it = next_it;
      next_it = it;
      next_it++;
      if (next_it == edges.end()) {
        done = true;
        next_it = edges.begin();
      }

      is_start_closed_segment = !is_end_closed_segment;
      is_end_closed_segment = !next_it->start_is_closed;
      edge.first = edge.second;
      edge.second =next_it->edge_start_angle;

      if (it->count == 2) continue;
      if (is_a_contained_in_b(is_start_closed_segment, is_end_closed_segment,
                              edge, segment_outer_circle))
      {
        it->plusplus(edge_index);
        continue;
      }
      bool is_start_contained =
        is_open_direction_contained_in_arc(segment_outer_circle.first, true,
                                           edge);
      bool is_end_contained =
        is_open_direction_contained_in_arc(segment_outer_circle.second, false,
                                           edge);
      // o~~~~~~~~~~~~o  = new arc
      // ?------------?  = "old" arc (the edge from the array)
      if (is_start_contained) {
        if (is_end_contained) {
          bool isordered =
            !segment_outer_circle.second.counterclockwise_in_between(segment_outer_circle.first, edge.second);
          if (isordered) {
            //                 o~~~~~~~~~~~~o
            // ?-----------------------?
            // __________________________
            // ?----c
            //                 o~-~-~-~-~-~-o
            //                                          c-----?
            struct Circle_arrangment_edge edge2 = *it;
            edge2.start_is_closed = false;
            edge2.edge_start_angle = segment_outer_circle.first;
            edge2.plusplus(edge_index);
            this->insert_if_legal(it, next_it, edge2);
            struct Circle_arrangment_edge edge3 = *it;
            edge3.start_is_closed = true;
            edge3.edge_start_angle = segment_outer_circle.second;
            this->insert_if_legal(it,next_it,edge3);
          }
          else {
            // ...~~~~~~~~~o        o~~~~~~~~~~... (round)
            //                   ?------------?
            // __________________________
            //                    ?~-~o
            //                           c----c
            //                                 o-~-?
            struct Circle_arrangment_edge edge2 = *it;
            edge2.start_is_closed = true;
            edge2.edge_start_angle = segment_outer_circle.second;
            this->insert_if_legal(it, next_it, edge2);
            struct Circle_arrangment_edge edge3 = *it;
            edge3.start_is_closed = false;
            edge3.edge_start_angle = segment_outer_circle.first;
            edge3.plusplus(edge_index);
            this->insert_if_legal(it, next_it, edge3);
            it->plusplus(edge_index);
          }
        }
        else {
          //                 o~~~~~~~~~~~~o
          // ?-----------?
          //_____________________
          // ?----c
          //                 o-~-~-~?
          struct Circle_arrangment_edge edge2 = *it;
          edge2.start_is_closed = false;
          edge2.edge_start_angle = segment_outer_circle.first;
          edge2.plusplus(edge_index);
          this->insert_if_legal(it, next_it, edge2);
        }
      }
      else {
        if (is_end_contained) {
          // o~~~~~~~~~~~~o
          //                 ?------------?
          //_____________________
          //                 ?-~-~-~-o
          //                                 c----?
          struct Circle_arrangment_edge edge2 = *it;
          edge2.start_is_closed = true;
          edge2.edge_start_angle = segment_outer_circle.second;
          it->plusplus(edge_index);
          this->insert_if_legal(it, next_it, edge2);
        }
        //else -  no intersection, do noting
      }
    }
    merge_adjacent_2_edges_and_remove_empty();
  }

#if 0
  // debug function
  void printArrangement()
  {
    for (auto it = edges.begin(); it != edges.end(); ++it) {
      if (it->start_is_closed) std::cout<<")[";
      else std::cout << "](";
      std::cout << it->edge_start_angle;
      std::cout << ","<<(int)it->count;
    }
    std::cout << "\n\n";
  }
#endif


  /*! \fn void get_all_1_edges(OutputIterator oi)
   * Insert to oi all the cells in depth 1 i.e. the cells that represent legal
   * pullout directions
   * \param oithe output iterator to put the cells in
   * Puts in oi var of type pair<size_t, std::pair<Kernel::Direction_2,
   * Kernel::Direction_2 > > foreach valid top edge.
   * Should only be called after all of the polygon edges where inserted.
   */
  template <typename OutputIterator>
  OutputIterator get_all_1_edges(OutputIterator oi)
  {
    for (auto it = edges.begin(); it != edges.end();) {
      if ((*it).count == 1) {
        std::pair<size_t, Arc> edge;
        edge.first = (*it).edge_index;
        edge.second.first = (*it).edge_start_angle;
        it++;
        edge.second.second =
          (*((it == edges.end()) ? edges.begin() : (it))).edge_start_angle;
        *oi++ = edge;
      }
      else
      {
        it++;
      }
    }
  }

  /*! \fn bool all_is_covered_twice()
   * Before running this run merge_adjacent_2_edges_and_remove_empty() or
   * add_segment_outer_circle() which calls
   * merge_adjacent_2_edges_and_remove_empty().
   *
   * The funtions checks that the whole circle is a single cell, which can
   * happen only if this cell is of depth 2, so there is no need to check the
   * depth as well.
   * \return if all of the arrangement is in depth 2+
   */
  bool all_is_covered_twice() { return edges.size() == 1; }
};

/*! \fn std::pair<typename Kernel::Direction_2,typename Kernel::Direction_2> get_segment_outer_circle(typename Kernel::Segment_2 seg, CGAL::Orientation orientation)
 * \param[in] seg the polygon segment
 * \param[in] orientation the orientation of the segment (and the polygon).
 *   if CLOCKWISE then the outer half circle is to the left.
 * \return the open outer half-circle of the edge.
 */
template <typename Kernel>
inline std::pair<typename Kernel::Direction_2, typename Kernel::Direction_2>
get_segment_outer_circle(typename Kernel::Segment_2 seg,
                         CGAL::Orientation orientation)
{
  typename Kernel::Direction_2 forward( seg);
  typename Kernel::Direction_2 backward(-forward);
  return (orientation == CGAL::Orientation::CLOCKWISE) ?
    std::make_pair(backward, forward) : std::make_pair(forward, backward);
}

/*! \fn OutputIterator find_single_mold_translational_casting_2(const CGAL::Polygon_2<Kernel>& pgn, OutputIterator oi)
 * \param[in] pgn the input polygon that we want to check if is castable or not.
 * \param[in] oi the output iterator to put the top edges in
 * \return all the possible top edges of the polygon and there pullout direction
 *   (with no rotation)
 */
template <typename Kernel, typename OutputIterator>
OutputIterator
find_single_mold_translational_casting_2(const CGAL::Polygon_2<Kernel>& pgn,
                                         OutputIterator oi)
{
  /* Legend
   * point = Represented as  Direction_2. It is the intersection between the
   *   fitting Direction_2 and the unit circle
   *
   * arc = Represented as A pair of point. clockwise arc between the first
   *   point and the second point. (each of its sides might be open or closed)
   */
  auto e_it = pgn.edges_begin();
  size_t edge_index = 0;
  auto poly_orientation = pgn.orientation();
  auto segment_outer_circle =
    get_segment_outer_circle<Kernel>(*e_it++, poly_orientation);
  Circle_arrangment<Kernel> circle_arrangment(segment_outer_circle);
  edge_index++;
  for (; e_it!= pgn.edges_end(); e_it++,edge_index++) {
    segment_outer_circle =
      get_segment_outer_circle<Kernel>(*e_it, poly_orientation);
    circle_arrangment.add_segment_outer_circle(segment_outer_circle, edge_index);
    if (circle_arrangment.all_is_covered_twice()) return oi;
  }
  circle_arrangment.get_all_1_edges(oi);
  return oi;
}
#endif
