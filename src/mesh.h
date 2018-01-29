

#ifndef SRC_MESH_H_
#define SRC_MESH_H_

#include "color.h"
#include "opengl_headers.h"
#include "point2.h"
#include "point3.h"
#include "vector3.h"

#include <vector>


namespace MinGfx {
    
class Mesh {
public:
    Mesh();
    virtual ~Mesh();
    
    int AddTriangle(Point3 v1, Point3 v2, Point3 v3);
    void UpdateTriangle(int triangleID, Point3 v1, Point3 v2, Point3 v3);
    
    void SetNormals(int triangleID, Vector3 n1, Vector3 n2, Vector3 n3);
    void SetColors(int triangleID, Color c1, Color c2, Color c3);
    void SetTexCoords(int triangleID, int textureUnit, Point2 uv1, Point2 uv2, Point2 uv3);
    
    void UpdateGPUMemory();
    
    
    int num_vertices() const;
    int num_triangles() const;
    

    // Access to properties indexed by vertex number
    Point3 vertex(int vertexID) const;
    Vector3 normal(int vertexID) const;
    Color color(int vertexID) const;
    Point2 tex_coords(int textureUnit, int vertexID) const;
    

    void Draw();

    
    // ---- Advanced: set vertices and other attributes for the entire mesh at once ----
    void SetVertices(float *vertsArray, int numVerts);
    void SetNormals(float *normsArray, int numNorms);
    void SetColors(float *colorsArray, int numColors);
    void SetTexCoords(int textureUnit, float *texCoordsArray, int numTexCoords);

    
    void LoadFromOBJ(const std::string &filename);
    
    
private:
    std::vector<float> verts_;
    std::vector<float> norms_;
    std::vector<float> colors_;
    std::vector< std::vector<float> > texCoords_;
    
    bool gpuDirty_;
    
    GLuint vertexBuffer_;
    GLuint vertexArray_;
};
    
    
} // end namespace


#endif
