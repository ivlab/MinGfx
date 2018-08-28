#include "aabb.h"

#include "mesh.h"

#include <float.h>

namespace mingfx {

AABB::AABB() {
    min_[0] = min_[1] = min_[2] = std::numeric_limits<float>::max();
    max_[0] = max_[1] = max_[2] = -std::numeric_limits<float>::max();
}
    
    
AABB::AABB(const Point3 &a) {
    min_ = a;
    max_ = a;
}
    
AABB::AABB(const Vector3 &v) {
    min_ = Point3(-0.5*v[0], -0.5*v[1], -0.5*v[2]);
    max_ = Point3( 0.5*v[0],  0.5*v[1],  0.5*v[2]);
}
    
AABB::AABB(const Point3 &p, const Vector3 &v) {
    min_ = Point3(p[0] - 0.5*v[0], p[1] - 0.5*v[1], p[2] - 0.5*v[2]);
    max_ = Point3(p[0] + 0.5*v[0], p[1] + 0.5*v[1], p[2] + 0.5*v[2]);
}
    
AABB::AABB(const Point3 &a, const Point3 &b, const Point3 &c) {
    min_ = a;
    min_[0] = std::min(min_[0], b[0]);
    min_[1] = std::min(min_[1], b[1]);
    min_[2] = std::min(min_[2], b[2]);
    min_[0] = std::min(min_[0], c[0]);
    min_[1] = std::min(min_[1], c[1]);
    min_[2] = std::min(min_[2], c[2]);
    
    max_ = a;
    max_[0] = std::max(max_[0], b[0]);
    max_[1] = std::max(max_[1], b[1]);
    max_[2] = std::max(max_[2], b[2]);
    max_[0] = std::max(max_[0], c[0]);
    max_[1] = std::max(max_[1], c[1]);
    max_[2] = std::max(max_[2], c[2]);
}

    
AABB::AABB(const Mesh &mesh, unsigned int tri_id) {
    std::vector<unsigned int> indices = mesh.triangle_vertices(tri_id);
    Point3 a = mesh.vertex(indices[0]);
    Point3 b = mesh.vertex(indices[1]);
    Point3 c = mesh.vertex(indices[2]);
    
    min_ = a;
    min_[0] = std::min(min_[0], b[0]);
    min_[1] = std::min(min_[1], b[1]);
    min_[2] = std::min(min_[2], b[2]);
    min_[0] = std::min(min_[0], c[0]);
    min_[1] = std::min(min_[1], c[1]);
    min_[2] = std::min(min_[2], c[2]);
    
    max_ = a;
    max_[0] = std::max(max_[0], b[0]);
    max_[1] = std::max(max_[1], b[1]);
    max_[2] = std::max(max_[2], b[2]);
    max_[0] = std::max(max_[0], c[0]);
    max_[1] = std::max(max_[1], c[1]);
    max_[2] = std::max(max_[2], c[2]);
}

    
AABB::AABB(const Mesh &mesh) {
    min_[0] = min_[1] = min_[2] = std::numeric_limits<float>::max();
    max_[0] = max_[1] = max_[2] = -std::numeric_limits<float>::max();
    
    for (int i=0; i < mesh.num_vertices(); i++) {
        Point3 a = mesh.vertex(i);
        min_[0] = std::min(min_[0], a[0]);
        min_[1] = std::min(min_[1], a[1]);
        min_[2] = std::min(min_[2], a[2]);
        
        max_[0] = std::max(max_[0], a[0]);
        max_[1] = std::max(max_[1], a[1]);
        max_[2] = std::max(max_[2], a[2]);
    }
}
    
    
AABB::~AABB() {}

Vector3 AABB::Dimensions() const {
    return max_ - min_;
}
    
float AABB::Volume() const {
    if (max_[0] < min_[0]) {
        // empty box
        return -1.0;
    }
    
    Vector3 dims = max_ - min_;
    return (dims[0] * dims[1] * dims[2]);
}

    
Point3 AABB::min() const {
    return min_;
}

Point3 AABB::max() const {
    return max_;
}

    
void AABB::set_user_data(int data) {
    user_data_ = data;
}

int AABB::user_data() {
    return user_data_;
}
    
    
// Compute an AABB that contains both A and B completely
AABB operator+(const AABB &A, const AABB &B) {
    AABB C;
    
    C.min_[0] = std::min(A.min_[0], B.min_[0]);
    C.min_[1] = std::min(A.min_[1], B.min_[1]);
    C.min_[2] = std::min(A.min_[2], B.min_[2]);
    
    C.max_[0] = std::max(A.max_[0], B.max_[0]);
    C.max_[1] = std::max(A.max_[1], B.max_[1]);
    C.max_[2] = std::max(A.max_[2], B.max_[2]);

    return C;
}

    
} // end namespace
