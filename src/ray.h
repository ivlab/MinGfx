/*
 This file is part of the MinGfx Project.
 
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 
 Original Author(s) of this File:
	Dan Keefe, 2018, University of Minnesota
	
 Author(s) of Significant Updates/Modifications to the File:
	...
 */

#ifndef SRC_RAY_H_
#define SRC_RAY_H_

#include <iostream>

#include "aabb.h"
#include "point3.h"
#include "vector3.h"
#include "mesh.h"


namespace mingfx {

    
/** Stores the mathematical object of a ray that begins at an origin (a 3D
 point) and points in a direction (a unit 3D vector).  Rays can intersect
 a variety of other computer graphics objects, such as planes, triangles,
 spheres, 3D meshes, etc.  These intersections can be tested with the
 Intersect...() methods.  The Ray can also be transformed by a Matrix4.
 Example:
 ~~~
 // Create a pick ray from the mouse position
 void MyGraphicsApp::OnLeftMouseDown(const Point2 &pos) {
    Point2 mouse_xy = PixelsToNormalizedDeviceCoords(pos);
    float mouse_z = ReadZValueAtPixel(pos);
    Point3 mouse_3d = GfxMath::ScreenToNearPlane(view_matrix, proj_matrix, mouse_xy, mouse_z);
    Matrix4 camera_matrix = view_matrix.Inverse();
    Point3 eye = camera_matrix.ColumnToPoint3(3);
 
    Ray pick_ray(eye, mouse_3d - eye);
 
    // check to see if the ray intersects a sphere
    float t;
    Point3 p;
    if (pick_ray.IntersectSphere(Point3(0,0,0), 2.0, &t, &p)) {
        std::cout << "Mouse pointing at sphere!  Intersection point = " << p << std::endl;
    }
 }
 ~~~
 */
class Ray {
public:
    
    /// Defaults to a ray at the origin and pointing in the -Z direction
    Ray();
    
    /// Creates a ray from a 3D origin and direction
    Ray(const Point3 &origin, const Vector3 &direction);
    
    /// Ray destructor
    virtual ~Ray();
    
    /// Check for "equality", taking floating point imprecision into account
    bool operator==(const Ray& other) const;
    
    /// Check for "inequality", taking floating point imprecision into account
    bool operator!=(const Ray& other) const;
    
    /// Returns the length of the direction vector
    float Length() const;
    
    /** Checks to see if the ray intersects a plane defined by a point and a normal.
     If there was an intersection, true is returned, iTime is set to the intersection
     time, and iPoint is set to the intersection point.  The plane is considered
     to be 1-sided.  That is the intersection will only occur if the ray hits the
     plane from its front side as determined by the plane's normal.
     */
    bool IntersectPlane(const Point3 &planePt, const Vector3 &planeNormal,
                        float *iTime, Point3 *iPoint) const;
    
    /** Checks to see if the ray intersects a triangle defined by the vertices v1, v2, and v3.
     The vertices must be provided in counter-clockwise order so that the normal of the
     triangle can be determined via the right-hand rule.  The intersection will only happen
     if the ray hits the front side of the triangle.  If there was an intersection,
     true is returned, iTime is set to the intersection time, and iPoint is set to the intersection point.
     */
    bool IntersectTriangle(const Point3 &v1, const Point3 &v2, const Point3 &v3,
                           float *iTime, Point3 *iPoint) const;
    
    /** Checks to see if the ray intersects a quad defined by the vertices v1, v2, v3, and v4.
     The vertices must be provided in counter-clockwise order so that the normal of the
     triangle can be determined via the right-hand rule.  The intersection will only happen
     if the ray hits the front side of the triangle.  If there was an intersection,
     true is returned, iTime is set to the intersection time, and iPoint is set to the intersection point.
     */
    bool IntersectQuad(const Point3 &v1, const Point3 &v2, const Point3 &v3, const Point3 &v4,
                       float *iTime, Point3 *iPoint) const;

    /** Checks to see if the ray intersects a sphere defined by a center point and a radius.
     If there was an intersection, true is returned, iTime is set to the intersection time, 
     and iPoint is set to the intersection point.
     */
    bool IntersectSphere(const Point3 &center, float radius,
                         float *iTime, Point3 *iPoint) const;
    
    /** Checks to see if the ray intersects a triangle mesh.  This is a brute-force
     check over each triangle in the mesh. If there was an intersection, true is returned, 
     iTime is set to the intersection time, iPoint is set to the intersection point,
     and iTriangleID is set to the ID of the closest intersected triangle along the ray.
     */
    bool IntersectMesh(const Mesh &mesh, float *iTime,
                       Point3 *iPoint, int *iTriangleID) const;
    
    /** Checks to see if the ray intersects a triangle mesh.  This uses a BVH
     (Bounding Volume Hierarchy) to accelerate the ray-triangle intersection tests.
     Each mesh can optionally store a BVH.  If a BVH has already been calculated
     for the mesh (done with Mesh::CalculateBVH()), then this function will be 
     much faster than the brute-force IntersectMesh() function.  If a BVH has 
     not already been calculated for the mesh, the first call to FastIntersectMesh()
     will trigger the mesh to create a BVH (not a fast operation) but then 
     subsequent calls to FastIntersectMesh() will be fast.
     */
    bool FastIntersectMesh(Mesh *mesh, float *iTime,
                           Point3 *iPoint, int *iTriangleID) const;
    
    /** Checks to see if the ray intersects an AABB (Axis-Aligned Bounding Box).
     Typically, this is the first step of a more detailed intersection test and
     we don't care about the actual point of intersection, just whether it
     intersects or not.  So, we don't bother calculating the iPoint.  We get the
     iTime for free though, so we do return that.  You can calc the iPoint if
     you want using:
     ~~~
     float t;
     if (ray.IntersectAABB(box, &t)) {
       Point3 iPoint = ray.origin() + t*ray.direction();
     }
     ~~~
     */
     bool IntersectAABB(const AABB &box, float *iTime) const;
    
    /// Returns the origin
    Point3 origin() const;
    
    /// Returns the direction
    Vector3 direction() const;

    /// Sets a new origin and direction
    void set(Point3 newOrigin, Vector3 newDir);
    
private:
    Point3 p_;
    Vector3 d_;
};


// --- Stream operators ---

std::ostream & operator<< ( std::ostream &os, const Ray &r);
std::istream & operator>> ( std::istream &is, Ray &r);

    
} // end namespace

#endif
