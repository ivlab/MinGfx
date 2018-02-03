#ifndef SRC_GFXMATH_H_
#define SRC_GFXMATH_H_

#include "point2.h"
#include "point3.h"
#include "matrix4.h"

namespace MinGfx {

    
float clamp(float x, float a, float b);
    
float to_radians(float degrees);
    
float to_degrees(float radians);
    
/// Converts a 2D point on the filmplane represented in Normalized Device
/// Coorindates, which means (-1,1) for the top left corner of the screen and
/// (1,-1) for the bottom right corner, to a 3D point that lies on the camera's
/// near plane.  Useful for converting mouse coordinates into a 3D point.
/// Remember that this uses NORMALIZED device coordinates for the screenPt,
/// not pixels.  GraphicsApp and most other graphics engines report mouse move
/// events in pixels, so you need to convert these to normalized device coordinates
/// first.  If you are using GraphicsApp, you can do this with:
/// Point2 normPos = graphicsApp->pixels_to_normalized_coordinates(mousePos);
Point3 filmplane2D_to_nearplane3D(const Matrix4 &viewMatrix, const Matrix4 &projMatrix, const Point2 &normalizedScreenPt);

/// Similar to filmplane2D_to_nearplane3D() but here rather than using the
/// nearplane, you specify the depth of the plane to use as a distance away
/// from the camera's focal point.
Point3 filmplane2D_to_plane3D(const Matrix4 &viewMatrix, const Matrix4 &projMatrix, const Point2 &normalizedScreenPt, float planeDepth);

/// Converts a 2D point on the filmplane represented in Normalized Device
/// Coorindates, which means (-1,1) for the top left corner of the screen and
/// (1,-1) for the bottom right corner, to a 3D point in the world.  The depth
/// buffer value under the pixel must be supplied. If you are using GraphicsApp,
/// you can use the mouse pos in pixels to get the required arguments like this:
/// Point2 normPos = graphicsApp->pixels_to_normalized_coordinates(mousePos);
/// float normZ = graphicsApp->z_value_at_pixel(mousePos);
Point3 filmplane2D_to_world3D(const Matrix4 &viewMatrix, const Matrix4 &projMatrix, const Point2 &normalizedScreenPt, float normalizedZ);




    
} // end namespace

#endif