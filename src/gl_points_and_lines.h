/*
 This file is part of the MinGfx Project.
 
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 
 Original Author(s) of this File:
	Dan Keefe, 2018, University of Minnesota
	
 Author(s) of Significant Updates/Modifications to the File:
	...
 */

#ifndef SRC_LINES_H_
#define SRC_LINES_H_



#include "color.h"
#include "opengl_headers.h"
#include "matrix4.h"
#include "point3.h"
#include "shader_program.h"
#include "vector3.h"

#include <vector>


namespace mingfx {

class Matrix4;
    
/**
 */
class GLPointsAndLines {
public:
    /// Creates an empty mesh.
    GLPointsAndLines();
    
    /// Copies all data and sets GPU dirty bit for the new mesh.
    GLPointsAndLines(const GLPointsAndLines &other);
    
    virtual ~GLPointsAndLines();
    
    enum DrawMode {
        POINTS,
        LINES,
        LINE_STRIP,
        LINE_LOOP
    };
    
    /// Sets the vertex array for the mesh directly.
    void SetVertices(const std::vector<Point3> &verts, DrawMode draw_mode);
    
    /// Sets the per-vertex colors array for the mesh directly.
    void SetColors(const std::vector<Color> &colors);
    

    /// Loads shaders
    void Init();
    
    /** This copies the entire mesh data structure to a vertex array in GPU memory,
     which must happen before you can draw the mesh.  For large meshes, this can
     take some time, so you may want to call this during initialization immediately
     after generating the mesh.  If you do not, it will be called automatically
     for you the first time Draw() is called. If the mesh contains normals, per-
     vertex colors and/or texture coordinates these are added as attributes within
     the vertex array. */
    void UpdateGPUMemory();
    
    /** This sends the mesh vertices and attributes down the graphics pipe using
     glDrawArrays() for the non-indexed mode and glDrawElements() for the indexed
     mode.  This is just the geometry -- for anything to show up on the screen,
     you must already have a ShaderProgram enabled before calling this function. */
    void Draw(Matrix4 model_matrix, Matrix4 view_matrix, Matrix4 proj_matrix);
    


    // Access to properties indexed by vertex number
    
    /// The total number of vertices in the mesh.
    int num_vertices() const;
    
   
private:
    DrawMode draw_mode_;
    std::vector<float> verts_;
    std::vector<float> colors_;
    
    bool gpu_dirty_;
    GLuint vertex_buffer_;
    GLuint vertex_array_;
    GLuint element_buffer_;
    
    ShaderProgram line_shader_;
};
    
    
} // end namespace


#endif
