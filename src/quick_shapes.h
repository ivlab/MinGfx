/*
 This file is part of the MinGfx Project.
 
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 
 Original Author(s) of this File:
	Dan Keefe, 2017, University of Minnesota
	
 Author(s) of Significant Updates/Modifications to the File:
	...
 */

#ifndef SRC_QUICK_SHAPES_H_
#define SRC_QUICK_SHAPES_H_

#include "color.h"
#include "default_shader.h"
#include "mesh.h"
#include "point3.h"
#include "shader_program.h"
#include "texture2d.h"
#include "vector3.h"
#include "matrix4.h"

#include <vector>



namespace mingfx {

/** This class provides a quick way to draw shapes for use in debugging or
    simple scenes.  You can specify the color for each shape as part of the 
    Draw...() call.  Other lighting parameters (the intensity of the light, 
    advanced material properties) are pre-set to reasonable defaults that apply 
    to all of the shapes drawn.  You can edit these if you wish, but note that 
    the intent of this class is just to provide a quick way to draw shapes -- 
    this is not the right tool to use if you wish to do quality renderings and 
    use multiple types of materials.
 
    Example usage:
    ~~~
    // define a new QuickShapes object during initialization, or as a class 
    // member variable
    QuickShapes quick_shapes;
 

    void DrawUsingOpenGL() {
        // later, in your draw routine, use it to draw shapes
        Matrix4 view = Matrix4::LookAt(Point3(0,0,3), Point3(0,0,0), Vector3(0,1,0));
        Matrix4 proj = Matrix4::Perspective(60.0, aspect_ratio(), 0.1, 10.0);
     
        Matrix4 m_cube = Matrix4::Translation(Vector3(-2.5,0,0)) * Matrix4::Scale(Vector3(0.5, 0.5, 0.5));
        quick_shapes.DrawCube(m_cube, view, proj, Color(1,1,1));

        Matrix4 m_sphere = Matrix4::Scale(Vector3(2.5, 2.5, 2.5));
        quick_shapes.DrawSphere(m_sphere, view, proj, Color(1,1,1));
 
        Matrix4 m_loop;
        std::vector<Point3> loop;
        loop.push_back(Point3( 4.0, 4.0, -4.0));
        loop.push_back(Point3(-4.0, 4.0, -4.0));
        loop.push_back(Point3(-4.0, 4.0,  4.0));
        loop.push_back(Point3( 4.0, 4.0,  4.0));
        quick_shapes.DrawLines(m_loop, view, proj, Color(1,1,1), loop, QuickShapes::LinesType::LINE_LOOP, 0.1);
    }
    ~~~
 */
class QuickShapes {
public:
    
    QuickShapes();
    virtual ~QuickShapes();
    

    // -------- 3D PRIMITIVES --------
    
	/** Draws a cube with extents -1 to 1 given the model, view, and projection 
        matrices provided and using the supplied color as a material property.
	 */
	void DrawCube(const Matrix4 &modelMatrix,
                  const Matrix4 &viewMatrix,
                  const Matrix4 &projectionMatrix,
                  const Color &color);

    /** Draws a cylinder with radius 1 and height y=-1 to 1 given the model, 
        view, and projection matrices provided and using the supplied color as a
        material property.
     */
    void DrawCylinder(const Matrix4 &modelMatrix,
                      const Matrix4 &viewMatrix,
                      const Matrix4 &projectionMatrix,
                      const Color &color);

    /** Draws a cone with radius 1 and height y=-1 to 1 given the model,
        view, and projection matrices provided and using the supplied color as a
        material property.
     */
    void DrawCone(const Matrix4 &modelMatrix,
                  const Matrix4 &viewMatrix,
                  const Matrix4 &projectionMatrix,
                  const Color &color);
    
    /** Draws a sphere with radius 1 given the model, view, and projection 
        matrices provided and using the supplied color as a material property.
     */
    void DrawSphere(const Matrix4 &modelMatrix,
                    const Matrix4 &viewMatrix,
                    const Matrix4 &projectionMatrix,
                    const Color &color);
    
    /** Draws the classic 3D paintbrush cursor from the 2001 Keefe et al.
        CavePainting paper. The tip of the brush is at (0,0,0), the front
        flat edge runs along the X axis, and the handle runs in the +Z direction.
     */
    void DrawBrush(const Matrix4 &modelMatrix,
                   const Matrix4 &viewMatrix,
                   const Matrix4 &projectionMatrix,
                   const Color &color);
    
    
    // -------- 3D COMPOSITE SHAPES --------
    
    /** Draws a cylinder between the two points.
     */
    void DrawLineSegment(const Matrix4 &modelMatrix,
                         const Matrix4 &viewMatrix,
                         const Matrix4 &projectionMatrix,
                         const Color &color,
                         const Point3 &p1,
                         const Point3 &p2,
                         float radius);
    
    enum LinesType {
        LINES,
        LINE_STRIP,
        LINE_LOOP
    };

    /** Draws a series of line segments.  Using linesType=LINES connects each
     consecutive pair of points in the points array with a line.  A linesType=LINE_STRIP
     will connect each point to the next.  And, a linesType=LINE_LOOP will connect
     each point to the next and in addition connect the last to the first.  Example:
     ~~~
     Matrix4 model;
     Matrix4 view = Matrix4::LookAt(Point3(0,0,3), Point3(0,0,0), Vector3(0,1,0));
     Matrix4 proj = Matrix4::Perspective(60.0, aspect_ratio(), 0.1, 10.0);
     std::vector<Point3> loop;
     loop.push_back(Point3( 4.0, 4.0, -4.0));
     loop.push_back(Point3(-4.0, 4.0, -4.0));
     loop.push_back(Point3(-4.0, 4.0,  4.0));
     loop.push_back(Point3( 4.0, 4.0,  4.0));
     quick_shapes.DrawLines(model, view, proj, Color(1,1,1), loop, QuickShapes::LinesType::LINE_LOOP, 0.1);
     ~~~
     */
    void DrawLines(const Matrix4 &modelMatrix,
                   const Matrix4 &viewMatrix,
                   const Matrix4 &projectionMatrix,
                   const Color &color,
                   const std::vector<Point3> &points,
                   LinesType linesType,
                   float radius);
    
    /** Draws an arrow originating at point p and extending in the direction and
        length specified by dir.  radius is the radius of the cylinder used to
        draw the shaft of the arrow.
     */
    void DrawArrow(const Matrix4 &modelMatrix,
                   const Matrix4 &viewMatrix,
                   const Matrix4 &projectionMatrix,
                   const Color &color,
                   Point3 p, Vector3 dir, float radius);
    
    /** Draws a right handed set of axes at the coordinate frame specified by
        the modelMatrix.  The arrows are 1 unit in length and colored based
        on the axis: X=red, Y=green, Z=blue.
     */
    void DrawAxes(const Matrix4 &modelMatrix,
                  const Matrix4 &viewMatrix,
                  const Matrix4 &projectionMatrix);
    
    
    // -------- 2D PRIMITIVES --------
    
    /** Draws a square in the X-Y plane with extents -1 to 1 and normal in the
        +Y direction.  Uses the model, view, and projection matrices provided
        and the supplied color as a material property.
     */
    void DrawSquare(const Matrix4 &modelMatrix, const Matrix4 &viewMatrix,
                    const Matrix4 &projectionMatrix, const Color &color);

    /** Draws a square, which you can deform into some other shape by adjusting
        the model matrix, and applies a texture to it.  The texture must already
        be bound to the OpenGL textureID provided.  The square lies in the X-Y 
        plane with extents -1 to 1 and normal in the +Y direction.  No lighting
        is applied.
     */
    void DrawSquare(const Matrix4 &modelMatrix, const Matrix4 &viewMatrix,
                    const Matrix4 &projectionMatrix, const Color &color,
                    const Texture2D &texture);
    
    /** Draws a background texture across the whole screen.  Typically, you will
        want to do this before any other draw calls.
     */
    void DrawFullscreenTexture(const Color &color, const Texture2D &texture);
    
    
    /** Returns a pointer to the default shader used internally by the Draw class
        so that you may change the default lighting properties if you wish. 
     */
    DefaultShader* default_shader();

    /** Returns a pointer to the default material properties for the shapes so
        that you may adjust the reflectance properties used by all the shapes
        if needed.
     */
    DefaultShader::MaterialProperties* material();

     
private:
        
    void DrawWithFullscreen(const Color &color, Mesh *mesh, const Texture2D &tex);
    
    Mesh cubeMesh_;
	void initCube();

    Mesh squareMesh_;
    void initSquare();

    Mesh fullMesh_;
    void initFull();
    
    Mesh cylMesh_;
    void initCyl();
    
    Mesh coneMesh_;
    void initCone();

    Mesh sphereMesh_;
    void initSph();
    
    Mesh brushMesh_;
    void initBrush();

    DefaultShader defaultShader_;
    DefaultShader::MaterialProperties defaultMaterial_;
    Texture2D emptyTex_;
    
    ShaderProgram fullscreenShader_;
};

} // end namespace

#endif