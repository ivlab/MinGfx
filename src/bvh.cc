#include "bvh.h"

#include "mesh.h"
#include "ray.h"

#include <float.h>
#include <algorithm>

namespace mingfx {

    
BVH::BVH() : root_(NULL) {
}
    
BVH::~BVH() {
    FreeNodeRecursive(root_);
}
    
void BVH::CreateFromMesh(const Mesh &mesh) {
    FreeNodeRecursive(root_);
    
    std::vector<AABB> tri_boxes;
    for (int i=0; i<mesh.num_triangles(); i++) {
        AABB box = AABB(mesh, i);
        box.set_user_data(i);
        tri_boxes.push_back(box);
    }
    
    root_ = new Node();
    BuildHierarchyRecursive(root_, tri_boxes);
}

void BVH::CreateFromListOfBoxes(const std::vector<AABB> &boxes) {
    FreeNodeRecursive(root_);

    root_ = new Node();
    BuildHierarchyRecursive(root_, boxes);
}
    
    
void BVH::FreeNodeRecursive(Node* node) {
    if (node == NULL) return;
    FreeNodeRecursive(node->child1);
    FreeNodeRecursive(node->child2);
    delete node;
}
    
bool sort_by_x(const AABB &lhs, const AABB &rhs) {
    return (lhs.min()[0] + lhs.max()[0]) < (rhs.min()[0] + rhs.max()[0]);
}

bool sort_by_y(const AABB &lhs, const AABB &rhs) {
    return (lhs.min()[1] + lhs.max()[1]) < (rhs.min()[1] + rhs.max()[1]);
}

bool sort_by_z(const AABB &lhs, const AABB &rhs) {
    return (lhs.min()[2] + lhs.max()[2]) < (rhs.min()[2] + rhs.max()[2]);
}
    
void BVH::BuildHierarchyRecursive(Node *node, std::vector<AABB> tri_boxes) {
    // got down to a leaf, a single box
    if (tri_boxes.size() == 1) {
        node->box = tri_boxes[0];
        return;
    }
    
    // calc the full bounding box for this node
    for (int i=0; i<tri_boxes.size(); i++) {
        node->box = node->box + tri_boxes[i];
    }
    
    // sort boxes along the longest axis
    Vector3 dims = node->box.Dimensions();
    dims[0] = fabsf(dims[0]);
    dims[1] = fabsf(dims[1]);
    dims[2] = fabsf(dims[2]);

    if ((dims[0] > dims[1]) && (dims[0] > dims[2])) {
        std::sort(tri_boxes.begin(), tri_boxes.end(), sort_by_x);
    }
    else if ((dims[1] > dims[0]) && (dims[1] > dims[2])) {
        std::sort(tri_boxes.begin(), tri_boxes.end(), sort_by_y);
    }
    else {
        std::sort(tri_boxes.begin(), tri_boxes.end(), sort_by_z);
    }
    
    // assign half to child1 and half to child2
    std::size_t const half_size = tri_boxes.size() / 2;
    std::vector<AABB> left_boxes(tri_boxes.begin(), tri_boxes.begin() + half_size);
    std::vector<AABB> right_boxes(tri_boxes.begin() + half_size, tri_boxes.end());
    
    node->child1 = new Node();
    BuildHierarchyRecursive(node->child1, left_boxes);
    node->child2 = new Node();
    BuildHierarchyRecursive(node->child2, right_boxes);
}
    
std::vector<int> BVH::IntersectAndReturnUserData(const Ray &r) const {
    std::vector<int> data_list;
    IntersectRecursive(r, root_, &data_list);
    return data_list;
}

void BVH::IntersectRecursive(const Ray &r, Node *node, std::vector<int> *data_list) const {
    float t;
    if (r.IntersectAABB(node->box, &t)) {
        if ((node->child1 == NULL) && (node->child2 == NULL)) {
            // reached a leaf node, add the object's user data to the list
            data_list->push_back(node->box.user_data());
        }
        else {
            // go deeper and check children
            IntersectRecursive(r, node->child1, data_list);
            IntersectRecursive(r, node->child2, data_list);
        }
	}
}

} // end namespace
