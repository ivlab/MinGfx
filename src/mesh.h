/*
 This file is part of the MinGfx Project.
 
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 
 Original Author(s) of this File:
	Dan Keefe, 2018, University of Minnesota
	
 Author(s) of Significant Updates/Modifications to the File:
	...
 */

#ifndef SRC_MESH_H_
#define SRC_MESH_H_



#include "bvh.h"
#include "color.h"
#include "opengl_headers.h"
#include "point2.h"
#include "point3.h"
#include "vector3.h"

#include <vector>


namespace mingfx {

class Matrix4;
    
/** A triangle mesh data structure that can be rendered with a ShaderProgram
 like DefaultShader.  The mesh can be created algorithmically by adding triangles
 one at a time or it can be loaded from an .obj file.
 
 Vertices are required -- you cannot have a mesh without vertices, but other
 attributes (normals, colors, texture coordinates) are optional.  When Draw()
 is called the mesh will automatically set these other attributes if available.
 
 Example of loading from a file:
 ~~~
 // during initialization
 Mesh m;
 m.LoadFromOBJ(Platform::FindMinGfxDataFile("teapot.obj"));
 // also create a shader to draw it.
 DefaultShader s;
 
 
 // later to draw
 Matrix4 M;
 Matrix4 V = Matrix4::LookAt(Point3(0,0,3), Point3(0,0,0), Vector3(0,1,0));
 Matrix4 P = Matrix4::Perspective(60.0, aspect_ratio(), 0.1, 10.0);
 s.Draw(M, V, P, m, DefaultShader::MaterialProperties());
 ~~~
 
 Example of creating a mesh algorithmically:
 ~~~
 Mesh mesh1;
 int tri_id;
 // add a first triangle
 tri_id = mesh1.AddTriangle(Point3(0,0,0), Point3(1,0,0), Point3(1,1,0));
 // set attributes for the vertices
 mesh1.SetNormals(tri_id, Vector3(0,0,1), Vector3(0,0,1), Vector3(0,0,1));
 mesh1.SetTexCoords(tri_id, 0, Point2(0,1), Point2(1,1), Point2(1,0));

 // add a second triangle and attributes
 tri_id = mesh1.AddTriangle(Point3(0,0,0), Point3(1,1,0), Point3(0,1,0));
 mesh1.SetNormals(tri_id, Vector3(0,0,1), Vector3(0,0,1), Vector3(0,0,1));
 mesh1.SetTexCoords(tri_id, 0, Point2(0,1), Point2(1,0), Point2(0,0));
 
 // call this when done to save to the GPU
 mesh1.UpdateGPUMemory();
 
 // then you can draw the same way as in the previous example.
 ~~~
 In the mode used above where you add one triangle at a time there is no way to
 reuse vertices in multiple triangles.  If you need to do this for efficiency
 or other reasons, then you can use an indexed faces mode where you set the
 mesh data structures directly.
 
 Example of creating a mesh that renders in an indexed faces mode:
 ~~~
 std::vector<unsigned int> indices;
 std::vector<Point3> vertices;
 std::vector<Vector3> normals;
 std::vector<Point2> texcoords;

 // four vertices, each requires 3 floats: (x,y,z)
 vertices.push_back(Point3(0,0,0));
 vertices.push_back(Point3(1,0,0));
 vertices.push_back(Point3(1,1,0));
 vertices.push_back(Point3(0,1,0));

 // four normals, each requires 3 floats: (x,y,z)
 normals.push_back(Vector3(0,0,1));
 normals.push_back(Vector3(0,0,1));
 normals.push_back(Vector3(0,0,1));
 normals.push_back(Vector3(0,0,1));

 // four texture coords, each requires 2 floats: (u,v)
 texcoords.push_back(Point2(0,1));
 texcoords.push_back(Point2(1,1));
 texcoords.push_back(Point2(1,0));
 texcoords.push_back(Point2(0,0));
 
 // indices into the arrays above for the first triangle
 indices.push_back(0); 
 indices.push_back(1); 
 indices.push_back(2);
 
 // indices for the second triangle, note some are reused
 indices.push_back(0); 
 indices.push_back(2); 
 indices.push_back(3);
 
 Mesh mesh1;
 mesh1.SetVertices(vertices);
 mesh1.SetNormals(normals);
 mesh1.SetTexCoords(0, texcoords);
 mesh1.SetIndices(indices);
 mesh1.UpdateGPUMemory();
 
 // then you can draw the same way as in the previous example.
 ~~~
 */
class Mesh {
public:
    /// Creates an empty mesh.
    Mesh();
    
    /// Copies all data and sets GPU dirty bit for the new mesh.
    Mesh(const Mesh &other);
    
    virtual ~Mesh();
    
    
    /** This reads a mesh stored in the common Wavefront Obj file format.  The
     loader here is simplistic and not guaranteed to work on all valid .obj
     files, but it should work on many simple ones. UpdateGPUMemory() is
     called automatically after the model is loaded. */
    void LoadFromOBJ(const std::string &filename);
    
    
    
    // ---- TRIANGLE LIST MODE ----
    // No indices are stored, each set of 3 vertices forms a triangle, and if the
    // triangles share vertices, those vertices need to be repeated.
    
    /** Adds a triangle to the mesh datastructure and returns a triangle ID.
     The ID should then be used as the first argument for follow-on calls to 
     SetNormals(), SetColors(), and SetTexCoords().  The vertices must be
     specified in counter-clockwise order so that the normal of the triangle
     can be determined following the right-hand rule. */
    int AddTriangle(Point3 v1, Point3 v2, Point3 v3);
    
    /** Updates the vertex positions for a triangle that has already been added
     to the mesh. */
    void UpdateTriangle(int triangle_id, Point3 v1, Point3 v2, Point3 v3);
    
    /** Sets the normals for the three vertices of a triangle that has already
     been added to the mesh */
    void SetNormals(int triangle_id, Vector3 n1, Vector3 n2, Vector3 n3);

    /** Sets per-vertex colors for the three vertices of a triangle that has already
     been added to the mesh */
    void SetColors(int triangle_id, Color c1, Color c2, Color c3);
    
    /** Sets the texture coordinates for the three vertices of a triangle that 
     has already been added to the mesh.  The first textureUnit is 0, and you
     should always use 0 for this parameter unless you are doing multi-texturing. */
    void SetTexCoords(int triangle_id, int texture_unit, Point2 uv1, Point2 uv2, Point2 uv3);
    
    

    // ---- INDEXED TRIANGLES MODE ----
    // Vertices are stored in an array and indices are stored in a separate array
    // each set of 3 indices into the vertex array defines one triangle.  Here,
    // you cannot add one triangle at a time to the mesh.  Instead you must set
    // the arrays of indices, vertices, and other attributes for the mesh at
    // once.
    
    /// Sets the vertex array for the mesh directly.
    void SetVertices(const std::vector<Point3> &verts);
    
    /// Sets the normal array for the mesh directly.
    void SetNormals(const std::vector<Vector3> &norms);
    
    /// Sets the per-vertex colors array for the mesh directly.
    void SetColors(const std::vector<Color> &colors);
    
    /// Sets a texture coordinates array for the mesh directly.
    void SetTexCoords(int texture_unit, const std::vector<Point2> &tex_coords);
    
    /// Sets the indices into the vertex array to use to create the triangles.
    /// Each consecutive set of 3 indices forms one triangle:
    /// (v1,v2,v3), (v1,v2,v3), (v1,v2,v3), ...
    void SetIndices(const std::vector<unsigned int> index_array);
    
    
    void SetInstanceTransforms(const std::vector<Matrix4> &xforms);
    
    
    // ---- These functions can be used instead of the above if you are working with
    // regular C-style arrays and floats rather than the higher level types like
    // Point3 and Vector3. ----
    
    /// Sets the vertex array for the mesh directly.  Vertices are stored as
    /// (x,y,z), (x,y,z), (x,y,z), ...
    /// This version of the function accepts a C-style array rather than std::vector<>
    void SetVertices(float *verts_array, int num_verts);

    /// Sets the normal array for the mesh directly.  Normals are stored as
    /// (x,y,z), (x,y,z), (x,y,z), ... following the same ordering as was used
    /// for SetVertices().
    /// This version of the function accepts a C-style array rather than std::vector<>
    void SetNormals(float *norms_array, int num_norms);

    /// Sets the per-vertex colors array for the mesh directly.  Colors are stored as
    /// (r,g,b,a), (r,g,b,a), (r,g,b,a), ... following the same ordering as was used
    /// for SetVertices().
    /// This version of the function accepts a C-style array rather than std::vector<>
    void SetColors(float *colors_array, int num_colors);

    /// Sets a texture coordinates array for the mesh directly.  Tex coords are stored as
    /// (u,v), (u,v), (u,v), ... following the same ordering as was used
    /// for SetVertices().
    /// This version of the function accepts a C-style array rather than std::vector<>
    void SetTexCoords(int texture_unit, float *tex_coords_array, int num_tex_coords);
    
    /// Sets the indices into the vertex array to use to create the triangles.
    /// Each consecutive set of 3 indices forms one triangle:
    /// (v1,v2,v3), (v1,v2,v3), (v1,v2,v3), ...
    /// This version of the function accepts a C-style array rather than std::vector<>
    void SetIndices(unsigned int *index_array, int num_indices);

    
    
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
    void Draw();
    

    
    /** This (re)calculates the normals for the mesh and stores them with the mesh
     data structure.  It assumes a faceted mesh, like a cube, where each triangle
     has separate vertices.  The normal is calculated for each triangle face and
     then the result is associated with each vertex that makes up the triangle.
     If you have a smooth mesh where vertices are shared between multiple faces
     then use CalcPerVertexNormals() instead. */
    void CalcPerFaceNormals();
    
    /** This (re)calculates the normals for the mesh and stores them with the mesh
     data structure.  It assumes a smooth mesh, like a sphere, where each vertex
     belongs to one or more triangles.  Each vertex normal is calculated as a
     weighted sum of the face normals for adjacent faces.  The weighting is based
     upon the relative areas of the neighboring faces (i.e., a large neighboring
     triangle contributes more to the vertex normal than a small one). */
    void CalcPerVertexNormals();
    
    
    /** This (re)calculates a Bounding Volume Hierarchy for the mesh, which can
     be used together with Ray::FastIntersectMesh() to do faster ray-mesh
     intersection testing. */
    void BuildBVH();
    
    /** Returns a pointer to the underlying BVH data structure.  If the data
     struture has not yet been build or needs to be updated due to a change in
     the geometry of the mesh, then the BVH is recalculated before returning
     the pointer. */
    BVH* bvh_ptr();
    
    // Access to properties indexed by vertex number
    
    /// The total number of vertices in the mesh.
    int num_vertices() const;
    
    /// Indexed by vertex number.  Also see num_vertices().
    Point3 vertex(int vertex_id) const;
    
    /// Indexed by vertex number.  Also see num_vertices().
    Vector3 normal(int vertex_id) const;
    
    /// Indexed by vertex number.  Also see num_vertices().
    Color color(int vertex_id) const;
    
    /// Indexed by vertex number.  Also see num_vertices().
    Point2 tex_coords(int texture_unit, int vertex_id) const;
    
    
    // Access to triangles
    
    /// The total number of triangles in the mesh.
    int num_triangles() const;
    
    /// Returns a 3 element array of the indices to the vertices that make up
    /// the specified triangle.
    std::vector<unsigned int> triangle_vertices(int triangle_id) const;
    
   
private:
    std::vector<float> verts_;
    std::vector<float> norms_;
    std::vector<float> colors_;
    std::vector< std::vector<float> > tex_coords_;
    std::vector<unsigned int> indices_;
    std::vector<float> instance_xforms_;
    
    bool gpu_dirty_;
    GLuint vertex_buffer_;
    GLuint vertex_array_;
    GLuint element_buffer_;
    
    bool bvh_dirty_;
    BVH bvh_;
};
    
    
} // end namespace


#endif
