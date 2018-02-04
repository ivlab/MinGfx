/*
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 See corresponding header file for details.
 */

#include "ray.h"

namespace mingfx {
    
    Ray::Ray() : p_(Point3::Origin()), d_(-Vector3::UnitZ()) {
    }
    
    Ray::Ray(const Point3 &origin, const Vector3 &direction) : p_(origin), d_(direction) {
    }
    
    Ray::~Ray() {
    }

    
    bool Ray::operator==(const Ray& other) const {
        return (p_ == other.origin()) && (d_ == other.direction());
    }

    
    bool Ray::operator!=(const Ray& other) const {
        return (p_ != other.origin()) || (d_ != other.direction());
    }

    
    float Ray::Length() const {
        return d_.Length();
    }

    
    Point3 Ray::origin() const {
        return p_;
    }

    
    Vector3 Ray::direction() const {
        return d_;
    }

    
    
    bool Ray::IntersectPlane(const Point3 &planePt, const Vector3 &planeNormal,
                             float *iTime, Point3 *iPoint) const
    {
        float dot = planeNormal.Dot(d_);
        
        // return false if we would hit the back face of the plane
        if (dot > 0.0) {
            return false;
        }
        
        // return false if the ray and plane are parallel
        if (std::fabs(dot) < MINGFX_MATH_EPSILON) {
            return false;
        }
        
        float denom = planeNormal.Dot(d_);
        if (std::abs(denom) > MINGFX_MATH_EPSILON) {
            *iTime = (planePt - p_).Dot(planeNormal) / denom;
            if (*iTime >= 0) {
                *iPoint = p_ + (*iTime)*d_;
                return true;
            }
        }
        return false;
    }
    
    
    bool Ray::IntersectTriangle(const Point3 &A, const Point3 &B, const Point3 &C,
                                float *iTime, Point3 *iPoint) const
    {
        // find the point of intersection of the ray with the plane of the triangle
        Vector3 AB = B - A;
        Vector3 AC = C - A;
        Vector3 cross = AB.Cross(AC);
        Vector3 N = cross.ToUnit();
        if (!IntersectPlane(A, N, iTime, iPoint)) {
            return false;
        }
        
        // check to see if iPoint lies within the triangle
        Vector3 edge1 = B - A;
        Vector3 v1 = *iPoint - A;
        Vector3 check1 = edge1.Cross(v1);
        if (N.Dot(check1) < 0.0) {
            return false;
        }

        Vector3 edge2 = C - B;
        Vector3 v2 = *iPoint - B;
        Vector3 check2 = edge2.Cross(v2);
        if (N.Dot(check2) < 0.0) {
            return false;
        }

        Vector3 edge3 = A - C;
        Vector3 v3 = *iPoint - C;
        Vector3 check3 = edge3.Cross(v3);
        if (N.Dot(check3) < 0.0) {
            return false;
        }

        return true;
    }

    
    bool Ray::IntersectQuad(const Point3 &A, const Point3 &B, const Point3 &C, const Point3 &D,
                            float *iTime, Point3 *iPoint) const
    {
        // find the point of intersection of the ray with the plane of the quad
        Vector3 AB = B - A;
        Vector3 AC = C - A;
        Vector3 cross = AB.Cross(AC);
        Vector3 N = cross.ToUnit();
        if (!IntersectPlane(A, N, iTime, iPoint)) {
            return false;
        }
        
        // check to see if iPoint lies within the quad
        Vector3 edge1 = B - A;
        Vector3 v1 = *iPoint - A;
        Vector3 check1 = edge1.Cross(v1);
        if (N.Dot(check1) < 0.0) {
            return false;
        }
        
        Vector3 edge2 = C - B;
        Vector3 v2 = *iPoint - B;
        Vector3 check2 = edge2.Cross(v2);
        if (N.Dot(check2) < 0.0) {
            return false;
        }
        
        Vector3 edge3 = D - C;
        Vector3 v3 = *iPoint - C;
        Vector3 check3 = edge3.Cross(v3);
        if (N.Dot(check3) < 0.0) {
            return false;
        }

        Vector3 edge4 = A - D;
        Vector3 v4 = *iPoint - D;
        Vector3 check4 = edge4.Cross(v4);
        if (N.Dot(check4) < 0.0) {
            return false;
        }
        
        return true;
    }

    
    bool Ray::IntersectSphere(const Point3 &center, float radius,
                              float *iTime, Point3 *iPoint) const
    {
        
        Point3 P = p_ + (Point3::Origin() - center);
        Vector3 D = d_;
        
        // A = (Dx^2 + Dy^2 + Dz^2)
        const double A = (D[0]*D[0] + D[1]*D[1] + D[2]*D[2]);
        
        // B = (Px * Dx + Py * Dy + Pz * Dz)
        const double B = (P[0]*D[0] + P[1]*D[1] + P[2]*D[2]);
        
        // C = (Px^2 + Py^2 + Pz^2 - (sphere radius)^2)
        const double C = (P[0]*P[0] + P[1]*P[1] + P[2]*P[2] - radius*radius);
        
        // Discriminant of quadratic = B^2 - A * C
        double discriminant =  B*B - A*C;
        
        if (discriminant < 0.0) {
            return false;
        }
        else {
            double discRoot = sqrt(discriminant);
            double t1 = (-B - discRoot) / A;
            double t2 = (-B + discRoot) / A;
            bool hit1 = false;
            bool hit2 = false;
            if (t1 > MINGFX_MATH_EPSILON) {
                hit1 = true;
                *iTime = t1;
            }
            if (t2 > MINGFX_MATH_EPSILON) {
                hit2 = true;
                *iTime = t2;
            }
            if ((!hit1) && (!hit2)) {
                return false;
            }
            if ((hit1) && (hit2)) {
                if (t1 < t2) {
                    *iTime = t1;
                }
            }
            
            *iPoint = p_ + (*iTime)*d_;
            return true;
        }
    }

    
    bool Ray::IntersectMesh(const Mesh &mesh,
                            float *iTime, Point3 *iPoint, int *iTriangleID) const
    {
        *iTime = -1.0;
        for (int i=0; i<mesh.num_triangles(); i++) {
            Point3 p;
            float t;
            std::vector<unsigned int> indices = mesh.triangle_vertices(i);
            if (IntersectTriangle(mesh.vertex(indices[0]), mesh.vertex(indices[1]), mesh.vertex(indices[2]), &t, &p)) {
                if ((*iTime < 0.0) || (t < *iTime)) {
                    *iPoint = p;
                    *iTime = t;
                    *iTriangleID = i;
                }
            }
        }
        return (*iTime > 0.0);
    }

    
    void Ray::set(Point3 newOrigin, Vector3 newDir) {
        p_ = newOrigin;
        d_ = newDir;
    }

    std::ostream & operator<< ( std::ostream &os, const Ray &r) {
        return os << r.origin() << " " << r.direction();
    }
    
    std::istream & operator>> ( std::istream &is, Ray &r) {
        // format:  (x, y, z)
        char dummy;
        Point3 p;
        Vector3 d;
        is >> p >> dummy >> d;
        r.set(p, d);
        return is;
    }
    
} // end namespace
