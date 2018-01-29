
#include "draw.h"
#include "platform.h"

#include <cmath>
#include <iostream>
#include <string>


namespace MinGfx {
    
    
    
#define PI 3.14159265359
#define TWOPI 6.28318530718
    
    
    
// Helper datastructure for building shapes algorithmically
class Vertex {
public:
    Vertex(GLfloat xx, GLfloat yy, GLfloat zz, GLfloat nnx, GLfloat nny, GLfloat nnz) :
    x(xx), y(yy), z(zz), nx(nnx), ny(nny), nz(nnz) {}
    
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat nx;
    GLfloat ny;
    GLfloat nz;
};

    
    
    
Draw::Draw() :
    lightPos_(10.0, 10.0, 10.0),
    lightAmb_(0.2, 0.2, 0.2),
    lightDiff_(0.7, 0.7, 0.7),
    matSpec_(0.6, 0.6, 0.6),
    matShin_(10.0) {
}

Draw::~Draw() {
}
    
    
    
    
// ------------  CUBE  ------------


void Draw::initCube() {
    GLfloat vertices[]  = {
        1.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 1.0f,  -1.0f,-1.0f, 1.0f,      // v0-v1-v2 (front)
        -1.0f,-1.0f, 1.0f,   1.0f,-1.0f, 1.0f,   1.0f, 1.0f, 1.0f,      // v2-v3-v0
        
        1.0f, 1.0f, 1.0f,   1.0f,-1.0f, 1.0f,   1.0f,-1.0f,-1.0f,      // v0-v3-v4 (right)
        1.0f,-1.0f,-1.0f,   1.0f, 1.0f,-1.0f,   1.0f, 1.0f, 1.0f,      // v4-v5-v0
        
        1.0f, 1.0f, 1.0f,   1.0f, 1.0f,-1.0f,  -1.0f, 1.0f,-1.0f,      // v0-v5-v6 (top)
        -1.0f, 1.0f,-1.0f,  -1.0f, 1.0f, 1.0f,   1.0f, 1.0f, 1.0f,      // v6-v1-v0
        
        -1.0f, 1.0f, 1.0f,  -1.0f, 1.0f,-1.0f,  -1.0f,-1.0f,-1.0f,      // v1-v6-v7 (left)
        -1.0f,-1.0f,-1.0f,  -1.0f,-1.0f, 1.0f,  -1.0f, 1.0f, 1.0f,      // v7-v2-v1.0
        
        -1.0f,-1.0f,-1.0f,   1.0f,-1.0f,-1.0f,   1.0f,-1.0f, 1.0f,      // v7-v4-v3 (bottom)
        1.0f,-1.0f, 1.0f,  -1.0f,-1.0f, 1.0f,  -1.0f,-1.0f,-1.0f,      // v3-v2-v7
        
        1.0f,-1.0f,-1.0f,  -1.0f,-1.0f,-1.0f,  -1.0f, 1.0f,-1.0f,      // v4-v7-v6 (back)
        -1.0f, 1.0f,-1.0f,   1.0f, 1.0f,-1.0f,   1.0f,-1.0f,-1.0f       // v6-v5-v4
    };
    
    GLfloat normals[]   = {
        0, 0, 1,   0, 0, 1,   0, 0, 1,      // v0-v1-v2 (front)
        0, 0, 1,   0, 0, 1,   0, 0, 1,      // v2-v3-v0
        
        1, 0, 0,   1, 0, 0,   1, 0, 0,      // v0-v3-v4 (right)
        1, 0, 0,   1, 0, 0,   1, 0, 0,      // v4-v5-v0
        
        0, 1, 0,   0, 1, 0,   0, 1, 0,      // v0-v5-v6 (top)
        0, 1, 0,   0, 1, 0,   0, 1, 0,      // v6-v1-v0
        
        -1, 0, 0,  -1, 0, 0,  -1, 0, 0,     // v1-v6-v7 (left)
        -1, 0, 0,  -1, 0, 0,  -1, 0, 0,     // v7-v2-v1
        
        0,-1, 0,   0,-1, 0,   0,-1, 0,      // v7-v4-v3 (bottom)
        0,-1, 0,   0,-1, 0,   0,-1, 0,      // v3-v2-v7
        
        0, 0,-1,   0, 0,-1,   0, 0,-1,      // v4-v7-v6 (back)
        0, 0,-1,   0, 0,-1,   0, 0,-1       // v6-v5-v4
    };
    
    cubeMesh_.SetVertices(vertices, 36);
    cubeMesh_.SetNormals(normals, 36);
    cubeMesh_.UpdateGPUMemory();
}


void Draw::Cube(const Matrix4 &modelMatrix, const Matrix4 &viewMatrix,
                const Matrix4 &projectionMatrix, const Color &color)
{
    if (cubeMesh_.num_vertices() == 0) {
        initCube();
    }
    DrawWithPhong(modelMatrix, viewMatrix, projectionMatrix, color, &cubeMesh_);
}




// ------------  SQUARE  ------------


void Draw::initSquare() {
    GLfloat vertices[] = {
        1.0f, 0.0f, 1.0f,   1.0f, 0.0f,-1.0f,  -1.0f, 0.0f,-1.0f,      // v0-v5-v6 (top)
        -1.0f, 0.0f,-1.0f,  -1.0f, 0.0f, 1.0f,   1.0f, 0.0f, 1.0f      // v6-v1-v0
    };
    
    GLfloat normals[] = {
        0, 1, 0,   0, 1, 0,   0, 1, 0,
        0, 1, 0,   0, 1, 0,   0, 1, 0
    };
    
    GLfloat texcoords[] = {
        1.0f, 1.0f,   1.0f, 0.0f,   0.0f, 0.0f,
        0.0f, 0.0f,   0.0f, 1.0f,   1.0f, 1.0f
    };
    
    squareMesh_.SetVertices(vertices, 6);
    squareMesh_.SetNormals(normals, 6);
    squareMesh_.SetTexCoords(0, texcoords, 6);
    squareMesh_.UpdateGPUMemory();
}


void Draw::Square(const Matrix4 &modelMatrix, const Matrix4 &viewMatrix,
                  const Matrix4 &projectionMatrix, const Color &color)
{
    if (squareMesh_.num_vertices() == 0) {
        initSquare();
    }
    DrawWithPhong(modelMatrix, viewMatrix, projectionMatrix, color, &squareMesh_);
}
    

void Draw::Square(const Matrix4 &modelMatrix, const Matrix4 &viewMatrix,
                  const Matrix4 &projectionMatrix, const Color &color,
                  const Texture2D &tex)
{
    if (squareMesh_.num_vertices() == 0) {
        initSquare();
    }
    DrawWithPhongTex(modelMatrix, viewMatrix, projectionMatrix, color, &squareMesh_, tex);
}
    
    
    


// ------------  CYLINDER  ------------


void Draw::initCyl() {
    
    std::vector<Vertex> verts;
    
    Vertex top(0,1,0, 0,1,0);
    Vertex bot(0,-1,0, 0,-1,0);
    
    const int nslices = 20;
    for (int s=1; s<nslices+1; s++) {
        GLfloat xlast = std::cos(-TWOPI * (float)(s-1)/(float)nslices);
        GLfloat zlast = std::sin(-TWOPI * (float)(s-1)/(float)nslices);
        GLfloat xnew = std::cos(-TWOPI * (float)(s)/(float)nslices);
        GLfloat znew = std::sin(-TWOPI * (float)(s)/(float)nslices);
        
        // one triangle on the top
        verts.push_back(top);
        verts.push_back(Vertex(xlast,1,zlast, 0,1,0));
        verts.push_back(Vertex(xnew,1,znew, 0,1,0));
        
        // two triangles to create a rect on the side
        verts.push_back(Vertex(xlast,1,zlast, xlast,0,zlast));
        verts.push_back(Vertex(xlast,-1,zlast, xlast,0,zlast));
        verts.push_back(Vertex(xnew,1,znew, xnew,0,znew));
        
        verts.push_back(Vertex(xnew,-1,znew, xnew,0,znew));
        verts.push_back(Vertex(xnew,1,znew, xnew,0,znew));
        verts.push_back(Vertex(xlast,-1,zlast, xlast,0,zlast));
        
        // one triangle on the bottom
        verts.push_back(bot);
        verts.push_back(Vertex(xnew,-1,znew, 0,-1,0));
        verts.push_back(Vertex(xlast,-1,zlast, 0,-1,0));
    }
    
    GLfloat *vertices = new GLfloat[3*verts.size()];
    GLfloat *normals  = new GLfloat[3*verts.size()];
    
    int index = 0;
    for (int i=0; i<verts.size(); i++) {
        vertices[index]   = verts[i].x;
        vertices[index+1] = verts[i].y;
        vertices[index+2] = verts[i].z;
        normals[index]    = verts[i].nx;
        normals[index+1]  = verts[i].ny;
        normals[index+2]  = verts[i].nz;
        index += 3;
    }
    
    cylMesh_.SetVertices(vertices, verts.size());
    cylMesh_.SetNormals(normals, verts.size());
    cylMesh_.UpdateGPUMemory();
}


void Draw::Cylinder(const Matrix4 &modelMatrix, const Matrix4 &viewMatrix,
                    const Matrix4 &projectionMatrix, const Color &color)
{
    if (cylMesh_.num_vertices() == 0) {
        initCyl();
    }
    DrawWithPhong(modelMatrix, viewMatrix, projectionMatrix, color, &cylMesh_);
}




// ------------  CONE  ------------


void Draw::initCone() {
    
    std::vector<Vertex> verts;
    
    Vertex top(0,1,0, 0,1,0);
    Vertex bot(0,-1,0, 0,-1,0);
    
    const int nslices = 20;
    for (int s=1; s<nslices+1; s++) {
        GLfloat xlast = std::cos(-TWOPI * (float)(s-1)/(float)nslices);
        GLfloat zlast = std::sin(-TWOPI * (float)(s-1)/(float)nslices);
        GLfloat xnew = std::cos(-TWOPI * (float)(s)/(float)nslices);
        GLfloat znew = std::sin(-TWOPI * (float)(s)/(float)nslices);
        
        // one triangle on the side
        // normals are a bit more complex than for other shapes...
        Vector3 nlast = Vector3(xlast, 2, zlast).to_unit();
        Vector3 nnew = Vector3(xnew, 2, znew).to_unit();
        Vector3 ntop = 0.5*(nlast + nnew);
        
        verts.push_back(Vertex(top.x, top.y, top.z, ntop[0], ntop[1], ntop[2]));
        verts.push_back(Vertex(xlast,-1,zlast, nlast[0], nlast[1], nlast[2]));
        verts.push_back(Vertex(xnew,-1,znew, nnew[0], nnew[1], nnew[2]));
        
        // one triangle on the bottom
        verts.push_back(bot);
        verts.push_back(Vertex(xnew,-1,znew, 0,-1,0));
        verts.push_back(Vertex(xlast,-1,zlast, 0,-1,0));
    }
    
    GLfloat *vertices = new GLfloat[3*verts.size()];
    GLfloat *normals  = new GLfloat[3*verts.size()];
    
    int index = 0;
    for (int i=0; i<verts.size(); i++) {
        vertices[index]   = verts[i].x;
        vertices[index+1] = verts[i].y;
        vertices[index+2] = verts[i].z;
        normals[index]    = verts[i].nx;
        normals[index+1]  = verts[i].ny;
        normals[index+2]  = verts[i].nz;
        index += 3;
    }
    
    coneMesh_.SetVertices(vertices, verts.size());
    coneMesh_.SetNormals(normals, verts.size());
    coneMesh_.UpdateGPUMemory();
}


void Draw::Cone(const Matrix4 &modelMatrix, const Matrix4 &viewMatrix,
                const Matrix4 &projectionMatrix, const Color &color)
{
    if (coneMesh_.num_vertices() == 0) {
        initCone();
    }
    DrawWithPhong(modelMatrix, viewMatrix, projectionMatrix, color, &coneMesh_);
}





// ------------  SPHERE  ------------


void Draw::initSph() {
    
    std::vector<Vertex> verts;
    
    Vertex top(0,1,0, 0,1,0);
    Vertex bot(0,-1,0, 0,-1,0);
    
    const int nslices = 40;
    const int nstacks = 40;
    for (int s=1; s<nslices+1; s++) {
        GLfloat xlast = std::cos(-TWOPI * (float)(s-1)/(float)nslices);
        GLfloat zlast = std::sin(-TWOPI * (float)(s-1)/(float)nslices);
        GLfloat xnew = std::cos(-TWOPI * (float)(s)/(float)nslices);
        GLfloat znew = std::sin(-TWOPI * (float)(s)/(float)nslices);
        
        float stackstep = PI/(float)nstacks;
        
        // one triangle on the top
        verts.push_back(top);
        verts.push_back(Vertex(std::sin(stackstep)*xlast,std::cos(stackstep),std::sin(stackstep)*zlast,
                               std::sin(stackstep)*xlast,std::cos(stackstep),std::sin(stackstep)*zlast));
        verts.push_back(Vertex(std::sin(stackstep)*xnew,std::cos(stackstep),std::sin(stackstep)*znew,
                               std::sin(stackstep)*xnew,std::cos(stackstep),std::sin(stackstep)*znew));
        
        for (int t=2; t<nstacks; t++) {
            GLfloat ylast = std::cos(PI*(float)(t-1)/(float)nstacks);
            GLfloat ynew = std::cos(PI*(float)(t)/(float)nstacks);
            
            GLfloat rlast = std::sin(PI * (float)(t-1)/(float)nstacks);
            GLfloat rnew = std::sin(PI * (float)(t)/(float)nstacks);
            
            // two triangles to create a rect on the side
            verts.push_back(Vertex(rlast*xlast,ylast,rlast*zlast, rlast*xlast,ylast,rlast*zlast));
            verts.push_back(Vertex(rnew*xlast,ynew,rnew*zlast, rnew*xlast,ynew,rnew*zlast));
            verts.push_back(Vertex(rnew*xnew,ynew,rnew*znew, rnew*xnew,ynew,rnew*znew));
            
            verts.push_back(Vertex(rnew*xnew,ynew,rnew*znew, rnew*xnew,ynew,rnew*znew));
            verts.push_back(Vertex(rlast*xnew,ylast,rlast*znew, rlast*xnew,ylast,rlast*znew));
            verts.push_back(Vertex(rlast*xlast,ylast,rlast*zlast, rlast*xlast,ylast,rlast*zlast));
        }
        
        // one triangle on the bottom
        verts.push_back(bot);
        verts.push_back(Vertex(std::sin(stackstep)*xlast,std::cos(PI-stackstep),std::sin(stackstep)*zlast,
                               std::sin(stackstep)*xlast,std::cos(PI-stackstep),std::sin(stackstep)*zlast));
        verts.push_back(Vertex(std::sin(stackstep)*xnew,std::cos(PI-stackstep),std::sin(stackstep)*znew,
                               std::sin(stackstep)*xnew,std::cos(PI-stackstep),std::sin(stackstep)*znew));
    }
    
    GLfloat *vertices = new GLfloat[3*verts.size()];
    GLfloat *normals  = new GLfloat[3*verts.size()];
    
    int index = 0;
    for (int i=0; i<verts.size(); i++) {
        vertices[index]   = verts[i].x;
        vertices[index+1] = verts[i].y;
        vertices[index+2] = verts[i].z;
        normals[index]    = verts[i].nx;
        normals[index+1]  = verts[i].ny;
        normals[index+2]  = verts[i].nz;
        index += 3;
    }
    
    sphereMesh_.SetVertices(vertices, verts.size());
    sphereMesh_.SetNormals(normals, verts.size());
    sphereMesh_.UpdateGPUMemory();
}


void Draw::Sphere(const Matrix4 &modelMatrix, const Matrix4 &viewMatrix,
                  const Matrix4 &projectionMatrix, const Color &color)
{
    if (sphereMesh_.num_vertices() == 0) {
        initSph();
    }
    DrawWithPhong(modelMatrix, viewMatrix, projectionMatrix, color, &sphereMesh_);
}




// ------------  BRUSH  ------------


void Draw::initBrush() {
    
    // Raw vertices -- points that make up the brush geometry
    const GLfloat v[19][3] = {
        { 0.5,   0.0,   0.0},  // 0
        {-0.5,   0.0,   0.0},  // 1
        
        { 0.5,   0.1,   0.25}, // 2
        {-0.5,   0.1,   0.25}, // 3
        { 0.5,   0.1,   0.75}, // 4
        {-0.5,   0.1,   0.75}, // 5
        { 0.1,   0.06,  1.0},  // 6
        {-0.1,   0.06,  1.0},  // 7
        { 0.15,  0.1,   1.75}, // 8
        {-0.15,  0.1,   1.75}, // 9
        
        { 0.0,   0.0,   1.85}, // 10
        
        { 0.5,  -0.1,   0.25}, // 11
        {-0.5,  -0.1,   0.25}, // 12
        { 0.5,  -0.1,   0.75}, // 13
        {-0.5,  -0.1,   0.75}, // 14
        { 0.1,  -0.06,  1.0},  // 15
        {-0.1,  -0.06,  1.0},  // 16
        { 0.15, -0.1,   1.75}, // 17
        {-0.15, -0.1,   1.75}  // 18
    };
    
    
    // Vertices arranged into triangles
    const GLfloat verts[34][3][3] = {
        // top
        {{v[0][0], v[0][1], v[0][2]},  {v[1][0], v[1][1], v[1][2]},  {v[2][0], v[2][1], v[2][2]}},
        {{v[1][0], v[1][1], v[1][2]},  {v[3][0], v[3][1], v[3][2]},  {v[2][0], v[2][1], v[2][2]}},
        
        {{v[2][0], v[2][1], v[2][2]},  {v[3][0], v[3][1], v[3][2]},  {v[4][0], v[4][1], v[4][2]}},
        {{v[3][0], v[3][1], v[3][2]},  {v[5][0], v[5][1], v[5][2]},  {v[4][0], v[4][1], v[4][2]}},
        
        {{v[4][0], v[4][1], v[4][2]},  {v[5][0], v[5][1], v[5][2]},  {v[6][0], v[6][1], v[6][2]}},
        {{v[5][0], v[5][1], v[5][2]},  {v[7][0], v[7][1], v[7][2]},  {v[6][0], v[6][1], v[6][2]}},
        
        {{v[6][0], v[6][1], v[6][2]},  {v[7][0], v[7][1], v[7][2]},  {v[8][0], v[8][1], v[8][2]}},
        {{v[7][0], v[7][1], v[7][2]},  {v[9][0], v[9][1], v[9][2]},  {v[8][0], v[8][1], v[8][2]}},
        
        {{v[8][0], v[8][1], v[8][2]},  {v[9][0], v[9][1], v[9][2]},  {v[10][0], v[10][1], v[10][2]}},
        
        // bottom
        {{v[0][0], v[0][1], v[0][2]},  {v[12][0], v[12][1], v[12][2]},  {v[1][0], v[1][1], v[1][2]}},
        {{v[11][0], v[11][1], v[11][2]},  {v[12][0], v[12][1], v[12][2]},  {v[0][0], v[0][1], v[0][2]}},
        
        {{v[11][0], v[11][1], v[11][2]},  {v[14][0], v[14][1], v[14][2]},  {v[12][0], v[12][1], v[12][2]}},
        {{v[13][0], v[13][1], v[13][2]},  {v[14][0], v[14][1], v[14][2]},  {v[11][0], v[11][1], v[11][2]}},
        
        {{v[13][0], v[13][1], v[13][2]},  {v[16][0], v[16][1], v[16][2]},  {v[14][0], v[14][1], v[14][2]}},
        {{v[15][0], v[15][1], v[15][2]},  {v[16][0], v[16][1], v[16][2]},  {v[13][0], v[13][1], v[13][2]}},
        
        {{v[15][0], v[15][1], v[15][2]},  {v[18][0], v[18][1], v[18][2]},  {v[16][0], v[16][1], v[16][2]}},
        {{v[17][0], v[17][1], v[17][2]},  {v[18][0], v[18][1], v[18][2]},  {v[15][0], v[15][1], v[15][2]}},
        
        {{v[18][0], v[18][1], v[18][2]},  {v[17][0], v[17][1], v[17][2]},  {v[10][0], v[10][1], v[10][2]}},
        
        // one side
        {{v[11][0], v[11][1], v[11][2]},  {v[0][0], v[0][1], v[0][2]},  {v[2][0], v[2][1], v[2][2]}},
        
        {{v[11][0], v[11][1], v[11][2]},  {v[2][0], v[2][1], v[2][2]},  {v[4][0], v[4][1], v[4][2]}},
        {{v[4][0], v[4][1], v[4][2]},  {v[13][0], v[13][1], v[13][2]},  {v[11][0], v[11][1], v[11][2]}},
        
        {{v[13][0], v[13][1], v[13][2]},  {v[4][0], v[4][1], v[4][2]},  {v[6][0], v[6][1], v[6][2]}},
        {{v[6][0], v[6][1], v[6][2]},  {v[15][0], v[15][1], v[15][2]},  {v[13][0], v[13][1], v[13][2]}},
        
        {{v[15][0], v[15][1], v[15][2]},  {v[6][0], v[6][1], v[6][2]},  {v[8][0], v[8][1], v[8][2]}},
        {{v[8][0], v[8][1], v[8][2]},  {v[17][0], v[17][1], v[17][2]},  {v[15][0], v[15][1], v[15][2]}},
        
        {{v[17][0], v[17][1], v[17][2]},  {v[8][0], v[8][1], v[8][2]},  {v[10][0], v[10][1], v[10][2]}},
        
        // other side
        {{v[3][0], v[3][1], v[3][2]},  {v[1][0], v[1][1], v[1][2]},  {v[12][0], v[12][1], v[12][2]}},
        
        {{v[3][0], v[3][1], v[3][2]},  {v[12][0], v[12][1], v[12][2]},  {v[14][0], v[14][1], v[14][2]}},
        {{v[14][0], v[14][1], v[14][2]},  {v[5][0], v[5][1], v[5][2]},  {v[3][0], v[3][1], v[3][2]}},
        
        {{v[5][0], v[5][1], v[5][2]},  {v[14][0], v[14][1], v[14][2]},  {v[16][0], v[16][1], v[16][2]}},
        {{v[16][0], v[16][1], v[16][2]},  {v[7][0], v[7][1], v[7][2]},  {v[5][0], v[5][1], v[5][2]}},
        
        {{v[7][0], v[7][1], v[7][2]},  {v[16][0], v[16][1], v[16][2]},  {v[18][0], v[18][1], v[18][2]}},
        {{v[18][0], v[18][1], v[18][2]},  {v[9][0], v[9][1], v[9][2]},  {v[7][0], v[7][1], v[7][2]}},
        
        {{v[9][0], v[9][1], v[9][2]},  {v[18][0], v[18][1], v[18][2]},  {v[10][0], v[10][1], v[10][2]}}
        
    };
    
    
    // Normals defined so as to make each face of the brush a flat surface
    const GLfloat norms[34][3][3] = {
        // top
        {{0.0, 0.93, -0.37}, {0.0, 0.93, -0.37}, {0.0, 0.93, -0.37}},
        {{0.0, 0.93, -0.37}, {0.0, 0.93, -0.37}, {0.0, 0.93, -0.37}},
        
        {{0.0, 1.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 1.0, 0.0}},
        {{0.0, 1.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 1.0, 0.0}},
        
        {{0.0, 0.988, 0.158}, {0.0, 0.988, 0.158}, {0.0, 0.988, 0.158}},
        {{0.0, 0.988, 0.158}, {0.0, 0.988, 0.158}, {0.0, 0.988, 0.158}},
        
        {{0.0, 0.999, -0.0533}, {0.0, 0.999, -0.0533}, {0.0, 0.999, -0.0533}},
        {{0.0, 0.999, -0.0533}, {0.0, 0.999, -0.0533}, {0.0, 0.999, -0.0533}},
        
        {{0.0, 0.709, 0.709}, {0.0, 0.709, 0.709}, {0.0, 0.709, 0.709}},
        
        // bottom
        {{0.0, -0.93, -0.37}, {0.0, -0.93, -0.37}, {0.0, -0.93, -0.37}},
        {{0.0, -0.93, -0.37}, {0.0, -0.93, -0.37}, {0.0, -0.93, -0.37}},
        
        {{0.0, -1.0, 0.0}, {0.0, -1.0, 0.0}, {0.0, -1.0, 0.0}},
        {{0.0, -1.0, 0.0}, {0.0, -1.0, 0.0}, {0.0, -1.0, 0.0}},
        
        {{0.0, -0.988, 0.158}, {0.0, -0.988, 0.158}, {0.0, -0.988, 0.158}},
        {{0.0, -0.988, 0.158}, {0.0, -0.988, 0.158}, {0.0, -0.988, 0.158}},
        
        {{0.0, -0.999, -0.0533}, {0.0, -0.999, -0.0533}, {0.0, -0.999, -0.0533}},
        {{0.0, -0.999, -0.0533}, {0.0, -0.999, -0.0533}, {0.0, -0.999, -0.0533}},
        
        {{0.0, -0.709, 0.709}, {0.0, -0.709, 0.709}, {0.0, -0.709, 0.709}},
        
        // one side
        {{1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}},
        
        {{1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}},
        {{1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}},
        
        {{0.848, 0.0, 0.530}, {0.848, 0.0, 0.530}, {0.848, 0.0, 0.530}},
        {{0.848, 0.0, 0.530}, {0.848, 0.0, 0.530}, {0.848, 0.0, 0.530}},
        
        {{1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}},
        {{1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}},
        
        {{0.709, 0.0, 0.709}, {0.709, 0.0, 0.709}, {0.709, 0.0, 0.709}},
        
        // other side
        {{-1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0}},
        
        {{-1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0}},
        {{-1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0}},
        
        {{-0.848, 0.0, 0.530}, {-0.848, 0.0, 0.530}, {-0.848, 0.0, 0.530}},
        {{-0.848, 0.0, 0.530}, {-0.848, 0.0, 0.530}, {-0.848, 0.0, 0.530}},
        
        {{-1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0}},
        {{-1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0}},
        
        {{-0.709, 0.0, 0.709}, {-0.709, 0.0, 0.709}, {-0.709, 0.0, 0.709}}
    };
    
    brushMesh_.SetVertices((float*)verts, 102);
    brushMesh_.SetNormals((float*)norms, 102);
    brushMesh_.UpdateGPUMemory();
}


void Draw::Brush(const Matrix4 &modelMatrix, const Matrix4 &viewMatrix,
                 const Matrix4 &projectionMatrix, const Color &color)
{
    if (brushMesh_.num_vertices() == 0) {
        initBrush();
    }
    DrawWithPhong(modelMatrix, viewMatrix, projectionMatrix, color, &brushMesh_);
}



// ----------------


void Draw::LineSegment(const Matrix4 &modelMatrix,
                       const Matrix4 &viewMatrix,
                       const Matrix4 &projectionMatrix,
                       const Color &color,
                       const Point3 &p1,
                       const Point3 &p2,
                       float radius)
{
    Matrix4 S = Matrix4::scale(Vector3(radius, 0.5*(p2-p1).length(), radius));
    Vector3 y = (p2-p1).to_unit();
    Vector3 z = Vector3(1,0,0).cross(y).to_unit();
    if (z == Vector3(0,0,0)) {
        z = Vector3(0,0,1).cross(y).to_unit();
    }
    Vector3 x = y.cross(z);
    Matrix4 R = Matrix4::fromRowMajorElements(
        x[0], y[0], z[0], 0,
        x[1], y[1], z[1], 0,
        x[2], y[2], z[2], 0,
        0,    0,    0, 1
    );
    Matrix4 T = Matrix4::translation(0.5 * Vector3(p1[0]+p2[0], p1[1]+p2[1], p1[2]+p2[2]));
    
    Matrix4 M = T * R * S;
    
    Cylinder(modelMatrix * M, viewMatrix, projectionMatrix, color);
}



void Draw::Lines(const Matrix4 &modelMatrix,
                 const Matrix4 &viewMatrix,
                 const Matrix4 &projectionMatrix,
                 const Color &color,
                 const std::vector<Point3> &points,
                 LinesType ltype,
                 float radius)
{
    if (ltype == LinesType::LINES) {
        for (int i=0; i<points.size(); i+=2) {
            LineSegment(modelMatrix, viewMatrix, projectionMatrix, color, points[i], points[i+1], radius);
        }
    }
    else {
        for (int i=0; i<points.size()-1; i++) {
            LineSegment(modelMatrix, viewMatrix, projectionMatrix, color, points[i], points[i+1], radius);
        }
        if (ltype == LinesType::LINE_LOOP) {
            LineSegment(modelMatrix, viewMatrix, projectionMatrix, color, points[points.size()-1], points[0], radius);
        }
    }
}

    
    
    
void Draw::initFull() {
    GLfloat vertices[] = {
        -1, -1, 0,   1, -1, 0,  1, 1, 0,
        -1, -1, 0,   1, 1, 0,   -1, 1, 0
    };
    
    GLfloat normals[] = {
        0, 0, 1,   0, 0, 1,   0, 0, 1,
        0, 0, 1,   0, 0, 1,   0, 0, 1
    };
    
    GLfloat texcoords[] = {
        0.0f, 1.0f,   1.0f, 1.0f,   1.0f, 0.0f,
        0.0f, 1.0f,   1.0f, 0.0f,   0.0f, 0.0f
    };
    
    fullMesh_.SetVertices(vertices, 6);
    fullMesh_.SetNormals(normals, 6);
    fullMesh_.SetTexCoords(0, texcoords, 6);
    fullMesh_.UpdateGPUMemory();
}
    

void Draw::FullscreenTexture(const Color &color, const Texture2D &tex) {
    if (fullMesh_.num_vertices() == 0) {
        initFull();
    }
    DrawWithFullscreen(color, &fullMesh_, tex);
}
    
    
    

void Draw::DrawWithPhong(const Matrix4 &modelMatrix, const Matrix4 &viewMatrix,
                         const Matrix4 &projectionMatrix, const Color &color, Mesh *mesh)
{
    if (!phongShader_.initialized()) {
        phongShader_.AddVertexShaderFromFile(Platform::findMinGfxShaderFile("phong.vert"));
        phongShader_.AddFragmentShaderFromFile(Platform::findMinGfxShaderFile("phong.frag"));
        phongShader_.LinkProgram();
    }
    
    Matrix4 normalMatrix = (modelMatrix*viewMatrix).inverse().transpose();
    
    // Activate the shader program
    phongShader_.UseProgram();
    
    // Pass uniforms and textures from C++ to the GPU Shader Program
    phongShader_.SetUniform("ModelMatrix", modelMatrix);
    phongShader_.SetUniform("ViewMatrix", viewMatrix);
    phongShader_.SetUniform("ProjectionMatrix", projectionMatrix);
    phongShader_.SetUniform("NormalMatrix", normalMatrix);
    phongShader_.SetUniform("LightPosition", lightPos_);
    phongShader_.SetUniform("LightIntensityAmbient", lightAmb_);
    phongShader_.SetUniform("LightIntensityDiffuse", lightDiff_);
    phongShader_.SetUniform("LightIntensitySpecular", lightSpec_);
    phongShader_.SetUniform("MatReflectanceAmbient", color);
    phongShader_.SetUniform("MatReflectanceDiffuse", color);
    phongShader_.SetUniform("MatReflectanceSpecular", matSpec_);
    phongShader_.SetUniform("MatReflectanceShininess", matShin_);
    
    // Draw the mesh using the shader program
    mesh->Draw();
    
    // Deactivate the shader program
    phongShader_.StopProgram();
}


void Draw::DrawWithPhongTex(const Matrix4 &modelMatrix, const Matrix4 &viewMatrix,
                            const Matrix4 &projectionMatrix, const Color &color,
                            Mesh *mesh, const Texture2D &tex)
{
    if (!phongTexShader_.initialized()) {
        phongTexShader_.AddVertexShaderFromFile(Platform::findMinGfxShaderFile("phongtex.vert"));
        phongTexShader_.AddFragmentShaderFromFile(Platform::findMinGfxShaderFile("phongtex.frag"));
        phongTexShader_.LinkProgram();
    }

    Matrix4 normalMatrix = (modelMatrix*viewMatrix).inverse().transpose();
    
    // Activate the shader program
    phongTexShader_.UseProgram();
    
    // Pass uniforms and textures from C++ to the GPU Shader Program
    phongTexShader_.SetUniform("ModelMatrix", modelMatrix);
    phongTexShader_.SetUniform("ViewMatrix", viewMatrix);
    phongTexShader_.SetUniform("ProjectionMatrix", projectionMatrix);
    phongTexShader_.SetUniform("NormalMatrix", normalMatrix);
    phongTexShader_.SetUniform("LightPosition", lightPos_);
    phongTexShader_.SetUniform("LightIntensityAmbient", lightAmb_);
    phongTexShader_.SetUniform("LightIntensityDiffuse", lightDiff_);
    phongTexShader_.SetUniform("LightIntensitySpecular", lightSpec_);
    phongTexShader_.SetUniform("MatReflectanceAmbient", color);
    phongTexShader_.SetUniform("MatReflectanceDiffuse", color);
    phongTexShader_.SetUniform("MatReflectanceSpecular", matSpec_);
    phongTexShader_.SetUniform("MatReflectanceShininess", matShin_);
    phongTexShader_.BindTexture("SurfaceTexture", tex);
    
    // Draw the mesh using the shader program
    mesh->Draw();
    
    // Deactivate the shader program
    phongTexShader_.StopProgram();
}
    
    
    

void Draw::DrawWithFullscreen(const Color &color, Mesh *mesh, const Texture2D &tex) {
    if (!fullscreenShader_.initialized()) {
        fullscreenShader_.AddVertexShaderFromFile(Platform::findMinGfxShaderFile("fullscreen.vert"));
        fullscreenShader_.AddFragmentShaderFromFile(Platform::findMinGfxShaderFile("fullscreen.frag"));
        fullscreenShader_.LinkProgram();
    }

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    
    // Activate the shader program
    fullscreenShader_.UseProgram();
    
    // Pass uniforms and textures from C++ to the GPU Shader Program
    fullscreenShader_.SetUniform("TintColor", color);
    fullscreenShader_.BindTexture("SurfaceTexture", tex);
    
    // Draw the mesh using the shader program
    mesh->Draw();
    
    // Deactivate the shader program
    fullscreenShader_.StopProgram();
    
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
}

    
} // end namespace
