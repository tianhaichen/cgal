/*!
\ingroup PkgMesh_3Concepts
\cgalConcept

The concept `MeshDomainWithFeatures_3` refines the concept `MeshDomain_3`.
While the concept
`MeshDomain_3` only exposes the 2-dimensional and 3-dimensional features of
the domain through different queries, the concept `MeshDomainWithFeatures_3` also exposes 0 and
1-dimensional features. The exposed features of the domain are respectively called
subdomains, surface patches, curve segments
and corners according to their respective dimensions 3,2,1 and 0.

\cgalRefines `MeshDomain_3`

\cgalHasModel `CGAL::Mesh_domain_with_polyline_features_3<MeshDomain_3>`
\cgalHasModel `CGAL::Polyhedral_mesh_domain_with_features_3<IGT>`

\sa `MeshDomain_3`

*/

class MeshDomainWithFeatures_3 {
public:

/// \name Types
/// @{

/*!
A type to distinguish
`MeshDomain_3` models from `MeshDomainWithFeatures_3` models.
*/
typedef CGAL::Tag_true Has_features;

/*!
Numerical type.
*/
typedef unspecified_type FT;

/*!
Type of indices for curve segments (\f$ 1\f$-dimensional features)
of the input domain.
Must be a model of CopyConstructible, Assignable, DefaultConstructible and
LessThanComparable. The default constructed value must be the value of an edge which
does not approximate a 1-dimensional feature of the input domain.
*/
typedef unspecified_type Curve_segment_index;

/*!
Type of indices for corners (i.e.\ 0-dimensional features)
of the input domain.
Must be a model of CopyConstructible, Assignable, DefaultConstructible and
LessThanComparable.
*/
typedef unspecified_type Corner_index;

/// @}

/*! \name Operations
Each connected component of a curve segment is assumed
to be oriented. The orientation is defined by the ordering
of the two incident corners at the origin and endpoint.
Therefore it is possible
to defined the signed geodesic distance between two ordered
points on the same connected component of a curve segment.
A cycle is a connected component of a curve segment incident to
0 or 1 corner.

\note `construct_point_on_curve segment` is assumed to return
a uniquely defined point. Therefore it is not possible to handle as a single
curve segment, a singular curve with several branches incident
to the same point.
*/
/// @{

/*!

Returns a point on the curve segment with index `ci`
at signed geodesic distance `d` from point `p`.
\pre Point `p` is supposed to be on curve segment `ci`. If ` d > 0`, the signed geodesic distance from `p` to the endpoint of the connected component of `ci` including \f$ p\f$, should be greater than \f$ d\f$. If ` d < 0`, the signed geodesic distance from `p` to the origin of the connected component should be less than \f$ d\f$ from the origin of the connected component.
*/
Point_3 construct_point_on_curve_segment(
const Point_3& p, const Curve_segment_index& ci, FT d) const;

/// @}

/// \name Queries
/// @{

/*!
Returns the length of the curve segment, on the curve with index
\c  curve_index, from \c p to \c q, in the orientation
\c orientation

If the curve connected component containing \c p and \c q is a cycle,
the orientation gives identifies which portion of the cycle
corresponds to the arc, otherwise \c orientation must be compatible
with the orientation of \c p and \c q on the curve segment.
*/
FT arc_length(const Point_3& p, const Point_3 q,
              const Curve_segment_index& curve_index,
              CGAL::Orientation orientation) const;
/*!

Returns `CGAL::POSITIVE` if the signed geodesic distance from
`p` to `q` on the way through `r` along cycle with index `ci`
is positive, `CGAL::NEGATIVE` if the distance is negative.
\pre `p != q && p != r && r != q`
*/
CGAL::Sign distance_sign_along_cycle(const Point_3& p, const Point_3& q,
const Point_3& r, const Curve_segment_index& ci) const;

/*!
Returns the sign of the geodesic distance from `p` to `q`, on the curve
with index `ci`.
*/
CGAL::Sign distance_sign(const Point_3& p, const Point_3& q,
                         const Curve_segment_index& ci) const;

/*!
Returns the length of the connected component of curve with index
\c curve_index that includes the point \c p
*/
FT curve_segment_length(const Point_3& p,
                        const Curve_segment_index& curve_index) const;
/*!
Returns `true` if the portion of the curve segment of index \c index,
between the points \c c1 and \c c2, is covered by the spheres of
centers \c c1 and \c c2 and squared radii \c sq_r1 and \c sq_r2
respectively. The points \c c1 and \c c2 are assumed to lie on the curve
segment.
*/
bool is_curve_segment_covered(const Curve_segment_index& index,
                              CGAL::Orientation orientation,
                              const Point_3& c1, const Point_3& c2,
                              const FT sq_r1, const FT sq_r2) const;
/*!

Returns `true` if the connected component of curve segment
`ci` including point `p` is a cycle.
*/
bool is_cycle(const Point_3& p, const Curve_segment_index& ci) const;

/// @}

/// \name Retrieval of the input features
/// @{

/*!
Fills `corners` with the corners of the input domain.
The value type of `corners` must be `std::pair<Corner_index,Point_3>`.
*/
template <typename OutputIterator>
OutputIterator
get_corners(OutputIterator corners) const;

/*!

Fills `curves` with the curve segments
of the input domain.
`curves` value type must be
`CGAL::cpp11::tuple<Curve_segment_index,std::pair<Point_3,%Index>,std::pair<Point_3,%Index> >`.
If the curve segment corresponding to an entry
in curves is not a cycle, the pair of associated points should
belong to
two corners incident on the curve segment.
If it is a cycle, then the same `Point_3` should be given twice and must be any
point on the cycle.
The `%Index` values associated to the points are their indices w.r.t.\ their dimension.
*/
template <typename OutputIterator>
OutputIterator
get_curve_segments(OutputIterator curves) const;

/// @}

/// \name Indices converters
/// @{

/*!

Returns the index to be stored at a vertex lying on the curve segment identified
by `curve_segment_index`.
*/
Index index_from_curve_segment_index(const Curve_segment_index& curve_segment_index) const;

/*!

Returns the `Curve_segment_index` of the curve segment where lies a vertex with
dimension 1 and index `index`.
*/
Curve_segment_index curve_segment_index(const Index& index) const;

/*!

Returns the index to be stored at a vertex lying on the corner identified
by `corner_index`.
*/
Index index_from_corner_index(const Corner_index& corner_index) const;

/*!

Returns the `Corner_index` of the corner where lies a vertex with
dimension 0 and index `index`.
*/
Corner_index corner_index(const Index& index) const;

/// @}

}; /* end MeshDomainWithFeatures_3 */
