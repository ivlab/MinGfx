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

#ifndef SRC_BVH_H_
#define SRC_BVH_H_

#include "aabb.h"
#include "point3.h"


namespace mingfx {
    
// forward declarations
class Mesh;
class Ray;
    

/** A Bounding Volume Hierarchy (BVH) data structure that can be used to
 accelerate ray-object intersection tests by carving up space into a hierarchy
 of partitions represented in a tree.  Each node of the tree is represented
 as an AABB (Axis-Aligned Bounding Box) that contains all of the nodes under it.
 Different objects can be stored inside each bounding box.  For example, when
 a BVH is created for a mesh, each leaf node can contain a AABB that contains
 just a single triangle.  Or, when a BVH is created for an entire scene, you
 could have each leaf node contain an entire mesh or other object within the
 scene.  In each case, use AABB's set_user_data() and user_data() methods to
 store a handle for whetever you want to store inside the nodes.
 */
class BVH {
public:
    /// Initializes the class with an empty hierarchy.
	BVH();
    
	virtual ~BVH();

    /** Creates a bounding volume hierarchy where each leaf node contains a single
     triangle from the mesh.  For leaf nodes, the triangle index can be retrieved
     with:
     ~~~
     int tri_id = leafnode->box.user_data();
     ~~~
     The user_data will be -1 for non-leaf nodes.  Once the structure has been
     created, it can be used to perform fast ray-mesh intersection tests.  See
     Ray::FastIntersectMesh().
     */
    void CreateFromMesh(const Mesh &mesh);
    
    
    /** Creates a BVH where each leaf node contains one of the boxes passed in
     to the function. 
     */
    void CreateFromListOfBoxes(const std::vector<AABB> &boxes);
    

	/** Traverse the BVH to find leaf nodes whose AABBs are intersected by the
     ray.  These are candidates to test more thoroughly using whatever ray-object
     intersection test is appropriate for the objects stored inside the AABB.
     This routine returns the user_data for each AABB leaf node.  In the case of
     a BVH created using CreateFromMesh, this means it stores the indices to
     the mesh triangles that should be tested for ray-triangle intersection.
     */
    std::vector<int> IntersectAndReturnUserData(const Ray &r) const;

    
private:
    
    // Simple internal data structure for storing each node of the BVH tree.
    class Node {
    public:
        Node() : child1(NULL), child2(NULL) {}
        
        // Links to children
        Node *child1;
        Node *child2;
        
        // Contains all geometry below this node.
        AABB box;
    };
    
    
    // for now, the copy constructor is private so no copies are allowed.
    // eventually, this would be good to implement and then it can be made public.
    BVH(const BVH &other);

    void BuildHierarchyRecursive(Node *node, std::vector<AABB> boxes);
    void IntersectRecursive(const Ray &r, Node *node, std::vector<int> *data_list) const;
    void FreeNodeRecursive(Node* node);
    
	Node* root_;
};

    
} // end namespace

#endif
