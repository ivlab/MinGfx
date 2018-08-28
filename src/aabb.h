/*
 This file is part of the MinGfx Project.
 
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 
 Original Author(s) of this File:
	David Schroeder, 2010-ish, University of Minnesota
	
 Author(s) of Significant Updates/Modifications to the File:
    Dan Keefe, 2018, University of Minnesota
	...
 */

#ifndef SRC_AABB_H_
#define SRC_AABB_H_

#include "point3.h"
#include "vector3.h"

namespace mingfx {

// forward declaration
class Mesh;
    
/** A 3D axis-aligned bounding box defined by two corners (min and max).  AABBs
 can be added together using the + operator to make them grow to cover the 
 extents of both boxes.  Each box can also store a user_id (int), which can be 
 used to associate the box with some other object in your program.
 */
class AABB {
public:
    /// Creates an empty box.
    AABB();
    
    /// Box that contains a single point
    AABB(const Point3 &a);
    
    /// Box that contains a triangle defined by 3 points
    AABB(const Point3 &a, const Point3 &b, const Point3 &c);
    
    /// Box centered at the origin with width, height, and depth specified by
    /// the vector.
    AABB(const Vector3 &extents);

    /// Box centered at the center with width, height, and depth specified by
    /// the vector.
    AABB(const Point3 &center, const Vector3 &extents);
    
    /// Box that contains a whole mesh
    AABB(const Mesh &mesh);

    /// Box that contains just triangle number "tri_id" from the specified mesh.
    AABB(const Mesh &mesh, unsigned int tri_id);
    
    virtual ~AABB();
    
    /// Returns the dimensions of the box in x, y, and z as a 3D vector.
    Vector3 Dimensions() const;
    
    /// Returns the volume of the box or -1.0 when empty and 0.0 if the box
    /// contains just a single point.
    float Volume() const;
    
    /// Returns the coordinates for the minimum corner of the box.
    Point3 min() const;
    
    /// Returns the coordinates for the maximum corner of the box.
    Point3 max() const;
    
    /// You can set this to whatever you want in order to use it as a handle
    /// into your own program.  The intent is to make it possible for you to
    /// associate this AABB with the id of some object in your application.
    void set_user_data(int data);

    /// You can set this to whatever you want in order to use it as a handle
    /// into your own program.  The intent is to make it possible for you to
    /// associate this AABB with the id of some object in your application.
    int user_data();

private:
    
    Point3 min_, max_;
    
    int user_data_;
    
    friend AABB operator+(const AABB &A, const AABB &B);
};

// Returns an AABB that contains both A and B completely (kind of like a union)
AABB operator+(const AABB &A, const AABB &B);

    
} // end namespace

#endif
