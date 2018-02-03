/**
 */

#ifndef SRC_RAY_H_
#define SRC_RAY_H_

#include <iostream>

#include "point3.h"
#include "vector3.h"
#include "mesh.h"


namespace MinGfx {
    
    
    /** @class Ray
     * @brief 3D ray (origin and direction).
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
        float length() const;
        
        /// Returns the origin
        Point3 origin() const;
        
        /// Returns the direction
        Vector3 direction() const;
        
        
        bool IntersectPlane(const Point3 &planePt, const Vector3 &planeNormal,
                            float *iTime, Point3 *iPoint) const;
        
        bool IntersectTriangle(const Point3 &v1, const Point3 &v2, const Point3 &v3,
                               float *iTime, Point3 *iPoint) const;
        
        bool IntersectQuad(const Point3 &v1, const Point3 &v2, const Point3 &v3, const Point3 &v4,
                           float *iTime, Point3 *iPoint) const;

        bool IntersectSphere(const Point3 &center, float radius,
                             float *iTime, Point3 *iPoint) const;
        
        bool IntersectMesh(const Mesh &mesh, float *iTime,
                           Point3 *iPoint, int *iTriangleID) const;
        
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
