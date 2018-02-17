/*
 This file is part of the MinGfx Project.
 
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 
 Original Author(s) of this File:
	Dan Keefe, 2018, University of Minnesota
	
 Author(s) of Significant Updates/Modifications to the File:
	...
 */

#ifndef SRC_GFXMATH_H_
#define SRC_GFXMATH_H_

#include "point2.h"
#include "point3.h"
#include "vector3.h"
#include "matrix4.h"

namespace mingfx {


/** This class holds a variety of static math functions that are useful to have
 defined with creating graphics programs.
 */
class GfxMath {
public:

    /// Returns a if x is less than a and b if x is greater than b.
    static float Clamp(float x, float a, float b);
        
    static float ToRadians(float degrees);
        
    static float ToDegrees(float radians);
    
    static Vector3 ToRadians(Vector3 degrees);
    
    static Vector3 ToDegrees(Vector3 radians);
    
    static float Lerp(float a, float b, float alpha);

    static int iLerp(int a, int b, float alpha);

    /// Converts a 2D point on the filmplane represented in Normalized Device
    /// Coorindates, which means (-1,1) for the top left corner of the screen and
    /// (1,-1) for the bottom right corner, to a 3D point that lies on the camera's
    /// near plane.  Useful for converting mouse coordinates into a 3D point.
    /// Remember that this uses NORMALIZED device coordinates for the screenPt,
    /// not pixels.  GraphicsApp and most other graphics engines report mouse move
    /// events in pixels, so you need to convert these to normalized device coordinates
    /// first.  If you are using GraphicsApp, you can do this with:
    /// Point2 normPos = graphicsApp->pixels_to_normalized_coordinates(mousePos);
    static Point3 ScreenToNearPlane(const Matrix4 &viewMatrix, const Matrix4 &projMatrix, const Point2 &normalizedScreenPt);

    /// Similar to filmplane2D_to_nearplane3D() but here rather than using the
    /// nearplane, you specify the depth of the plane to use as a distance away
    /// from the camera's focal point.
    static Point3 ScreenToDepthPlane(const Matrix4 &viewMatrix, const Matrix4 &projMatrix, const Point2 &normalizedScreenPt, float planeDepth);

    /// Converts a 2D point on the filmplane represented in Normalized Device
    /// Coorindates, which means (-1,1) for the top left corner of the screen and
    /// (1,-1) for the bottom right corner, to a 3D point in the world.  The depth
    /// buffer value under the pixel must be supplied. If you are using GraphicsApp,
    /// you can use the mouse pos in pixels to get the required arguments like this:
    /// Point2 normPos = graphicsApp->pixels_to_normalized_coordinates(mousePos);
    /// float normZ = graphicsApp->z_value_at_pixel(mousePos);
    static Point3 ScreenToWorld(const Matrix4 &viewMatrix, const Matrix4 &projMatrix, const Point2 &normalizedScreenPt, float normalizedZ);

};


    
} // end namespace

#endif