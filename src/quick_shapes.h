/**
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All rights reserved.
 
 Original Author(s) of this File:
	Dan Keefe, 2017, University of Minnesota
	
 Author(s) of Significant Updates/Modifications to the File:
	...
 */


#ifndef QUICKSHAPES_H
#define QUICKSHAPES_H


#include "opengl_headers.h"
#include "color.h"
#include "point3.h"
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
class QuickShapes {
public:
    
    QuickShapes();
    virtual ~QuickShapes();
    

    // -------- 3D PRIMITIVES --------
    
	/** Draws a cube with extents -1 to 1 given the model, view, and projection 
        matrices provided and using the supplied color as a material property.
	 */
	void drawCube(const Matrix4 &modelMatrix,
                  const Matrix4 &viewMatrix,
                  const Matrix4 &projectionMatrix,
                  const Color &color);

    /** Draws a cylinder with radius 1 and height y=-1 to 1 given the model, 
        view, and projection matrices provided and using the supplied color as a
        material property.
     */
    void drawCylinder(const Matrix4 &modelMatrix,
                      const Matrix4 &viewMatrix,
                      const Matrix4 &projectionMatrix,
                      const Color &color);

    /** Draws a cone with radius 1 and height y=-1 to 1 given the model,
        view, and projection matrices provided and using the supplied color as a
        material property.
     */
    void drawCone(const Matrix4 &modelMatrix,
                  const Matrix4 &viewMatrix,
                  const Matrix4 &projectionMatrix,
                  const Color &color);
    
    /** Draws a cylinder with radius 1 and height y=-1 to 1 given the model,
     view, and projection matrices provided and using the supplied color as a
     material property.
     */
    void drawSphere(const Matrix4 &modelMatrix,
                    const Matrix4 &viewMatrix,
                    const Matrix4 &projectionMatrix,
                    const Color &color);
    
    /** Draws the classic 3D paintbrush cursor from the 2001 Keefe et al.
        CavePainting paper. The tip of the brush is at (0,0,0), the front
        flat edge runs along the X axis, and the handle runs in the +Z direction.
     */
    void drawBrush(const Matrix4 &modelMatrix,
                   const Matrix4 &viewMatrix,
                   const Matrix4 &projectionMatrix,
                   const Color &color);
    
    
    // -------- 3D COMPOSITE SHAPES --------
    
    /** Draws a cylinder between the two points.
     */
    void drawLineSegment(const Matrix4 &modelMatrix,
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

    
    void drawLines(const Matrix4 &modelMatrix,
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
    void drawArrow(const Matrix4 &modelMatrix,
                   const Matrix4 &viewMatrix,
                   const Matrix4 &projectionMatrix,
                   Point3 p, Vector3 dir, float radius);
    
    /** Draws a right handed set of axes at the coordinate frame specified by
        the modelMatrix.  The arrows are 1 unit in length and colored based
        on the axis: X=red, Y=green, Z=blue.
     */
    void drawAxes(const Matrix4 &modelMatrix,
                  const Matrix4 &viewMatrix,
                  const Matrix4 &projectionMatrix);
    
    
    // -------- 2D PRIMITIVES --------
    
    /** Draws a square in the X-Y plane with extents -1 to 1 and normal in the
        +Y direction.  Uses the model, view, and projection matrices provided
        and the supplied color as a material property.
     */
    void drawSquare(const Matrix4 &modelMatrix, const Matrix4 &viewMatrix,
                    const Matrix4 &projectionMatrix, const Color &color);

    /** Draws a square, which you can deform into some other shape by adjusting
        the model matrix, and applies a texture to it.  The texture must already
        be bound to the OpenGL textureID provided.  The square lies in the X-Y 
        plane with extents -1 to 1 and normal in the +Y direction.  No lighting
        is applied.
     */
    void drawTexturedSquare(const Matrix4 &modelMatrix, const Matrix4 &viewMatrix,
                            const Matrix4 &projectionMatrix, const Color &color,
                            const int textureID);
    
    /** Draws a background texture across the whole screen.  Typically, you will
        want to do this before any other draw calls.
     */
    void drawFullscreenTexture(const Color &color, const int textureID);
    
    

	void setLightPos(const Point3 &pos) {
		s_lightPos[0] = pos[0];  s_lightPos[1] = pos[1]; s_lightPos[2] = pos[2];
	}

	void setLightIntensityAmbient(const Color &i) {
		s_lightAmb[0] = i[0];  s_lightAmb[1] = i[1]; s_lightAmb[2] = i[2];
	}

	void setLightIntensityDiffuse(const Color &i) {
		s_lightDiff[0] = i[0];  s_lightDiff[1] = i[1]; s_lightDiff[2] = i[2];
	}

	void setLightIntensitySpecular(const Color &i) {
		s_lightSpec[0] = i[0];  s_lightSpec[1] = i[1]; s_lightSpec[2] = i[2];
	}

	void setMaterialReflectanceSpecular(const Color &r) {
		s_matSpec[0] = r[0];  s_matSpec[1] = r[1]; s_matSpec[2] = r[2];
	}

	void setMaterialReflectanceShinniness(float s) {
		s_matShin = s;
	}

private:

    void freeGPUMemory();
    
	void initCube();
	GLuint s_cubeVAO;
	GLuint s_cubeVBO;

    void initSquare();
    GLuint s_squareVAO;
    GLuint s_squareVBO;
    
    void initCyl();
    GLuint s_cylVAO;
    GLuint s_cylVBO;
    int s_cylNVerts;
    
    void initCone();
    GLuint s_coneVAO;
    GLuint s_coneVBO;
    int s_coneNVerts;
    
    void initSph();
    GLuint s_sphVAO;
    GLuint s_sphVBO;
    int s_sphNVerts;
    
    void initBrush();
    GLuint s_brushVAO;
    GLuint s_brushVBO;
    
	float s_lightPos[3];
	float s_lightAmb[3];
	float s_lightDiff[3];
	float s_lightSpec[3];
	float s_matSpec[3];
	float s_matShin;
};

} // end namespace

#endif