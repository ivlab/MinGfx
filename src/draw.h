/**
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All rights reserved.
 
 Original Author(s) of this File:
	Dan Keefe, 2017, University of Minnesota
	
 Author(s) of Significant Updates/Modifications to the File:
	...
 */


#ifndef SRC_DRAW_H_
#define SRC_DRAW_H_

#include "color.h"
#include "mesh.h"
#include "point3.h"
#include "shader_program.h"
#include "texture2d.h"
#include "vector3.h"
#include "matrix4.h"

#include <vector>



namespace MinGfx {

/** This class provides a quick way to draw shapes for use in debugging or
    as cursors for VR input devices etc. using the GLSL pipeline.  You can specify
    the color for each shape as part of the rendering call.  Other lighting
    parameters (the intensity of the light, advanced material properties) are 
    pre-set to reasonable defaults that apply to all of the shapes drawn.  You can
    edit these if you wish, but note that the intent of this class is just to provide
    a quick way to draw shapes -- this is not the right tool to use if you wish to
    do quality renderings and use multiple types of materials.
 */
class Draw {
public:
    
    Draw();
    virtual ~Draw();
    

    // -------- 3D PRIMITIVES --------
    
	/** Draws a cube with extents -1 to 1 given the model, view, and projection 
        matrices provided and using the supplied color as a material property.
	 */
	void Cube(const Matrix4 &modelMatrix,
              const Matrix4 &viewMatrix,
              const Matrix4 &projectionMatrix,
              const Color &color);

    /** Draws a cylinder with radius 1 and height y=-1 to 1 given the model, 
        view, and projection matrices provided and using the supplied color as a
        material property.
     */
    void Cylinder(const Matrix4 &modelMatrix,
                  const Matrix4 &viewMatrix,
                  const Matrix4 &projectionMatrix,
                  const Color &color);

    /** Draws a cone with radius 1 and height y=-1 to 1 given the model,
        view, and projection matrices provided and using the supplied color as a
        material property.
     */
    void Cone(const Matrix4 &modelMatrix,
              const Matrix4 &viewMatrix,
              const Matrix4 &projectionMatrix,
              const Color &color);
    
    /** Draws a cylinder with radius 1 and height y=-1 to 1 given the model,
     view, and projection matrices provided and using the supplied color as a
     material property.
     */
    void Sphere(const Matrix4 &modelMatrix,
                const Matrix4 &viewMatrix,
                const Matrix4 &projectionMatrix,
                const Color &color);
    
    /** Draws the classic 3D paintbrush cursor from the 2001 Keefe et al.
        CavePainting paper. The tip of the brush is at (0,0,0), the front
        flat edge runs along the X axis, and the handle runs in the +Z direction.
     */
    void Brush(const Matrix4 &modelMatrix,
               const Matrix4 &viewMatrix,
               const Matrix4 &projectionMatrix,
               const Color &color);
    
    
    // -------- 3D COMPOSITE SHAPES --------
    
    /** Draws a cylinder between the two points.
     */
    void LineSegment(const Matrix4 &modelMatrix,
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

    
    void Lines(const Matrix4 &modelMatrix,
               const Matrix4 &viewMatrix,
               const Matrix4 &projectionMatrix,
               const Color &color,
               const std::vector<Point3> &points,
               LinesType ltype,
               float radius);
    
    /** Draws an arrow originating at point p and extending in the direction and
        length specified by dir.  radius is the radius of the cylinder used to
        draw the shaft of the arrow.
     */
    void Arrow(const Matrix4 &modelMatrix,
               const Matrix4 &viewMatrix,
               const Matrix4 &projectionMatrix,
               Point3 p, Vector3 dir, float radius);
    
    /** Draws a right handed set of axes at the coordinate frame specified by
        the modelMatrix.  The arrows are 1 unit in length and colored based
        on the axis: X=red, Y=green, Z=blue.
     */
    void Axes(const Matrix4 &modelMatrix,
              const Matrix4 &viewMatrix,
              const Matrix4 &projectionMatrix);
    
    
    // -------- 2D PRIMITIVES --------
    
    /** Draws a square in the X-Y plane with extents -1 to 1 and normal in the
        +Y direction.  Uses the model, view, and projection matrices provided
        and the supplied color as a material property.
     */
    void Square(const Matrix4 &modelMatrix, const Matrix4 &viewMatrix,
                const Matrix4 &projectionMatrix, const Color &color);

    /** Draws a square, which you can deform into some other shape by adjusting
        the model matrix, and applies a texture to it.  The texture must already
        be bound to the OpenGL textureID provided.  The square lies in the X-Y 
        plane with extents -1 to 1 and normal in the +Y direction.  No lighting
        is applied.
     */
    void Square(const Matrix4 &modelMatrix, const Matrix4 &viewMatrix,
                const Matrix4 &projectionMatrix, const Color &color,
                const Texture2D &texture);
    
    /** Draws a background texture across the whole screen.  Typically, you will
        want to do this before any other draw calls.
     */
    void FullscreenTexture(const Color &color, const Texture2D &texture);
    
    

	void set_light_pos(const Point3 &pos) {
		lightPos_[0] = pos[0];  lightPos_[1] = pos[1]; lightPos_[2] = pos[2];
	}

	void set_light_intensity_ambient(const Color &i) {
		lightAmb_[0] = i[0];  lightAmb_[1] = i[1]; lightAmb_[2] = i[2];
	}

	void set_light_intensity_diffuse(const Color &i) {
        lightDiff_[0] = i[0];  lightDiff_[1] = i[1]; lightDiff_[2] = i[2];
	}

	void set_light_intensity_specular(const Color &i) {
		lightSpec_[0] = i[0];  lightSpec_[1] = i[1]; lightSpec_[2] = i[2];
	}

	void set_material_reflectance_specular(const Color &r) {
		matSpec_[0] = r[0];  matSpec_[1] = r[1]; matSpec_[2] = r[2];
	}

	void set_material_reflectance_shinniness(float s) {
		matShin_ = s;
	}

private:

    void DrawWithPhong(const Matrix4 &modelMatrix, const Matrix4 &viewMatrix,
                       const Matrix4 &projectionMatrix, const Color &color,
                       Mesh *mesh);
    
    void DrawWithPhongTex(const Matrix4 &modelMatrix, const Matrix4 &viewMatrix,
                          const Matrix4 &projectionMatrix, const Color &color,
                          Mesh *mesh, const Texture2D &tex);
    
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

    ShaderProgram phongShader_;
    ShaderProgram phongTexShader_;
    ShaderProgram fullscreenShader_;
    
	Point3 lightPos_;
	Color lightAmb_;
	Color lightDiff_;
	Color lightSpec_;
	Color matSpec_;
	float matShin_;
};

} // end namespace

#endif