/*
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 See corresponding header file for details.
 */

#include "mesh.h"

#include "matrix4.h"
#include "opengl_headers.h"

#include <sstream>
#include <fstream>

namespace mingfx {
    
#define MAX_TEX_ATTRIBS 5


Mesh::Mesh() : gpu_dirty_(true), bvh_dirty_(true) {
}

Mesh::Mesh(const Mesh &other) {
    verts_ = other.verts_;
    norms_ = other.norms_;
    colors_ = other.colors_;
    tex_coords_ = other.tex_coords_;
    indices_ = other.indices_;
    gpu_dirty_ = true;
    bvh_dirty_ = true;
}

Mesh::~Mesh() {
}
    
int Mesh::AddTriangle(Point3 v1, Point3 v2, Point3 v3) {
    gpu_dirty_ = true;
    bvh_dirty_ = true;

    verts_.push_back(v1[0]);
    verts_.push_back(v1[1]);
    verts_.push_back(v1[2]);
    
    verts_.push_back(v2[0]);
    verts_.push_back(v2[1]);
    verts_.push_back(v2[2]);

    verts_.push_back(v3[0]);
    verts_.push_back(v3[1]);
    verts_.push_back(v3[2]);

    return num_triangles()-1;
}

void Mesh::UpdateTriangle(int triangle_id, Point3 v1, Point3 v2, Point3 v3) {
    gpu_dirty_ = true;
    bvh_dirty_ = true;
    
    int index = triangle_id * 9;
    verts_[index + 0] = v1[0];
    verts_[index + 1] = v1[1];
    verts_[index + 2] = v1[2];
    
    verts_[index + 3] = v2[0];
    verts_[index + 4] = v2[1];
    verts_[index + 5] = v2[2];

    verts_[index + 6] = v3[0];
    verts_[index + 7] = v3[1];
    verts_[index + 8] = v3[2];
}


void Mesh::SetNormals(int triangle_id, Vector3 n1, Vector3 n2, Vector3 n3) {
    gpu_dirty_ = true;

    if (triangle_id >= num_triangles()) {
        std::cerr << "Mesh::SetNormals() -- warning: cannot set normals for non-existant triangle with ID=" << triangle_id << ".  Make sure the triangle has been added first." << std::endl;
        return;
    }

    int requiredSize = (triangle_id+1)*9;
    if (norms_.size() < requiredSize) {
        norms_.resize(requiredSize);
    }
    int index = triangle_id * 9;
    norms_[index + 0] = n1[0];
    norms_[index + 1] = n1[1];
    norms_[index + 2] = n1[2];
    
    norms_[index + 3] = n2[0];
    norms_[index + 4] = n2[1];
    norms_[index + 5] = n2[2];
    
    norms_[index + 6] = n3[0];
    norms_[index + 7] = n3[1];
    norms_[index + 8] = n3[2];
}

void Mesh::SetColors(int triangle_id, Color c1, Color c2, Color c3) {
    gpu_dirty_ = true;

    if (triangle_id >= num_triangles()) {
        std::cerr << "Mesh::SetColors() -- warning: cannot set colors for non-existant triangle with ID=" << triangle_id << ".  Make sure the triangle has been added first." << std::endl;
        return;
    }
    
    int requiredSize = (triangle_id+1)*12;
    if (colors_.size() < requiredSize) {
        colors_.resize(requiredSize);
    }
    int index = triangle_id * 12;
    colors_[index + 0] = c1[0];
    colors_[index + 1] = c1[1];
    colors_[index + 2] = c1[2];
    colors_[index + 3] = c1[3];

    colors_[index + 4] = c2[0];
    colors_[index + 5] = c2[1];
    colors_[index + 6] = c2[2];
    colors_[index + 7] = c2[3];

    colors_[index + 8] = c3[0];
    colors_[index + 9] = c3[1];
    colors_[index + 10] = c3[2];
    colors_[index + 11] = c3[3];
}

void Mesh::SetTexCoords(int triangle_id, int textureUnit, Point2 uv1, Point2 uv2, Point2 uv3) {
    gpu_dirty_ = true;

    if (triangle_id >= num_triangles()) {
        std::cerr << "Mesh::SetTexCoords() -- warning: cannot set texture coordinates for non-existant triangle with ID=" << triangle_id << ".  Make sure the triangle has been added first." << std::endl;
        return;
    }
    
    // resize as needed based on the number of textureUnits used
    if (tex_coords_.size() < textureUnit+1) {
        tex_coords_.resize(textureUnit+1);
    }

    // resize the textureUnit-specific array based on the number of triangles
    int requiredSize = (triangle_id+1)*6;
    if (tex_coords_[textureUnit].size() < requiredSize) {
        tex_coords_[textureUnit].resize(requiredSize);
    }
    int index = triangle_id * 6;
    tex_coords_[textureUnit][index + 0] = uv1[0];
    tex_coords_[textureUnit][index + 1] = uv1[1];

    tex_coords_[textureUnit][index + 2] = uv2[0];
    tex_coords_[textureUnit][index + 3] = uv2[1];

    tex_coords_[textureUnit][index + 4] = uv3[0];
    tex_coords_[textureUnit][index + 5] = uv3[1];
}


void Mesh::SetVertices(const std::vector<Point3> &verts) {
    gpu_dirty_ = true;
    bvh_dirty_ = true;

    verts_.clear();
    for (int i=0; i<verts.size(); i++) {
        verts_.push_back(verts[i][0]);
        verts_.push_back(verts[i][1]);
        verts_.push_back(verts[i][2]);
    }
}

void Mesh::SetNormals(const std::vector<Vector3> &norms) {
    gpu_dirty_ = true;
    norms_.clear();
    for (int i=0; i<norms.size(); i++) {
        norms_.push_back(norms[i][0]);
        norms_.push_back(norms[i][1]);
        norms_.push_back(norms[i][2]);
    }
}

void Mesh::SetColors(const std::vector<Color> &colors) {
    gpu_dirty_ = true;
    colors_.clear();
    for (int i=0; i<colors.size(); i++) {
        colors_.push_back(colors[i][0]);
        colors_.push_back(colors[i][1]);
        colors_.push_back(colors[i][2]);
        colors_.push_back(colors[i][3]);
    }
}

void Mesh::SetTexCoords(int texture_unit, const std::vector<Point2> &tex_coords) {
    gpu_dirty_ = true;
    // resize as needed based on the number of textureUnits used
    if (tex_coords_.size() < texture_unit+1) {
        tex_coords_.resize(texture_unit+1);
    }
    tex_coords_[texture_unit].clear();
    for (int i=0; i<tex_coords.size(); i++) {
        tex_coords_[texture_unit].push_back(tex_coords[i][0]);
        tex_coords_[texture_unit].push_back(tex_coords[i][1]);
    }
}


void Mesh::SetIndices(const std::vector<unsigned int> indices) {
    gpu_dirty_ = true;
    bvh_dirty_ = true;

    indices_.clear();
    for (int i=0; i<indices.size(); i++) {
        indices_.push_back(indices[i]);
    }
}


void Mesh::SetInstanceTransforms(const std::vector<Matrix4> &xforms) {
    gpu_dirty_ = true;
    instance_xforms_.clear();
    for (int i=0; i<xforms.size(); i++) {
        for (int j=0; j<16; j++) {
            std::cout << xforms[i][j] << std::endl;
            instance_xforms_.push_back(xforms[i][j]);
        }
    }
}
    

void Mesh::SetVertices(float *vertsArray, int numVerts) {
    gpu_dirty_ = true;
    bvh_dirty_ = true;

    verts_.clear();
    int numFloats = numVerts * 3;
    for (int i=0; i<numFloats; i++) {
        verts_.push_back(vertsArray[i]);
    }
}

void Mesh::SetNormals(float *normsArray, int numNorms) {
    gpu_dirty_ = true;
    norms_.clear();
    int numFloats = numNorms * 3;
    for (int i=0; i<numFloats; i++) {
        norms_.push_back(normsArray[i]);
    }
}

void Mesh::SetColors(float *colorsArray, int numColors) {
    gpu_dirty_ = true;
    colors_.clear();
    int numFloats = numColors * 4;
    for (int i=0; i<numFloats; i++) {
        colors_.push_back(colorsArray[i]);
    }
}

void Mesh::SetTexCoords(int textureUnit, float *texCoordsArray, int numTexCoords) {
    gpu_dirty_ = true;
    // resize as needed based on the number of textureUnits used
    if (tex_coords_.size() < textureUnit+1) {
        tex_coords_.resize(textureUnit+1);
    }
    tex_coords_[textureUnit].clear();
    int numFloats = numTexCoords * 2;
    for (int i=0; i<numFloats; i++) {
        tex_coords_[textureUnit].push_back(texCoordsArray[i]);
    }
}

void Mesh::SetIndices(unsigned int *indexArray, int numIndices) {
    gpu_dirty_ = true;
    bvh_dirty_ = true;

    indices_.clear();
    for (int i=0; i<numIndices; i++) {
        indices_.push_back(indexArray[i]);
    }
}
    
    




void Mesh::UpdateGPUMemory() {
    if (gpu_dirty_) {
        // sanity check -- for each attribute that is added (normals, colors, texcoords)
        // make sure the number of triangles is equal to the number of tris in the verts
        // array.
        if ((norms_.size() != 0) && (norms_.size() / 3 != num_vertices())) {
            std::cerr << "Mesh::UpdateGPUMemory() -- warning: the number of per vertex normals in the mesh is not equal to the number vertices in the mesh. (N = " << norms_.size() / 3 << ", V = " << num_vertices() << ")" << std::endl;
        }
        if ((colors_.size() != 0) && (colors_.size() / 4 != num_vertices())) {
            std::cerr << "Mesh::UpdateGPUMemory() -- warning: the number of per vertex colors in the mesh is not equal to the number vertices in the mesh. (C = " << colors_.size() / 4 << ", V = " << num_vertices() << ")" << std::endl;
        }
        for (int i = 0; i < tex_coords_.size(); i++) {
            if ((tex_coords_[i].size() != 0) && (tex_coords_[i].size() / 2 != num_vertices())) {
                std::cerr << "Mesh::UpdateGPUMemory() -- warning: the number of per vertex texture coordinates (for texture unit #" << i << ") is not equal to the number vertices in the mesh.  (UVs = " << tex_coords_[i].size() / 2 << ", V = " << num_vertices() << ")" << std::endl;
            }
        }

        GLsizeiptr totalMemSize = 0;

        GLsizeiptr vertsMemSize = verts_.size() * sizeof(float);
        GLsizeiptr vertsMemOffset = 0;
        totalMemSize += vertsMemSize;

        GLsizeiptr normsMemSize = norms_.size() * sizeof(float);
        GLsizeiptr normsMemOffset = totalMemSize;
        totalMemSize += normsMemSize;

        GLsizeiptr colorsMemSize = colors_.size() * sizeof(float);
        GLsizeiptr colorsMemOffset = totalMemSize;
        totalMemSize += colorsMemSize;

        std::vector<GLsizeiptr> texCoordsMemSize;
        std::vector<GLsizeiptr> texCoordsMemOffset;
        for (int i = 0; i < std::min((int)tex_coords_.size(),(int)MAX_TEX_ATTRIBS); i++) {
            texCoordsMemSize.push_back(tex_coords_[i].size() * sizeof(float));
            texCoordsMemOffset.push_back(totalMemSize);
            totalMemSize += texCoordsMemSize[i];
        }

        GLsizeiptr instanceXformsMemSize = instance_xforms_.size() * sizeof(float);
        GLsizeiptr instanceXformsMemOffset = totalMemSize;
        totalMemSize += instanceXformsMemSize;

        
        glGenBuffers(1, &vertex_buffer_);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
        glBufferData(GL_ARRAY_BUFFER, totalMemSize, NULL, GL_STATIC_DRAW);

        glBufferSubData(GL_ARRAY_BUFFER, vertsMemOffset, vertsMemSize, &verts_[0]);
        if (norms_.size() > 0) {
            glBufferSubData(GL_ARRAY_BUFFER, normsMemOffset, normsMemSize, &norms_[0]);
        }
        if (colors_.size() > 0) {
            glBufferSubData(GL_ARRAY_BUFFER, colorsMemOffset, colorsMemSize, &colors_[0]);
        }
        for (int i=0; i<tex_coords_.size(); i++) {
            glBufferSubData(GL_ARRAY_BUFFER, texCoordsMemOffset[i], texCoordsMemSize[i], &(tex_coords_[i][0]));
        }
        if (instance_xforms_.size() > 0) {
            glBufferSubData(GL_ARRAY_BUFFER, instanceXformsMemOffset, instanceXformsMemSize, &instance_xforms_[0]);
        }
        glGenVertexArrays(1, &vertex_array_);
        glBindVertexArray(vertex_array_);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
        
        // attribute 0 = vertices (required)
        int attribID = 0;
        int nComponents = 3;
        glEnableVertexAttribArray(attribID);
        glVertexAttribPointer(attribID, nComponents, GL_FLOAT, GL_FALSE, nComponents*sizeof(GLfloat), (char*)0 + vertsMemOffset);

        // attribute 1 = normals (optional)
        attribID = 1;
        if (norms_.size()) {
            nComponents = 3;
            glEnableVertexAttribArray(attribID);
            glVertexAttribPointer(attribID, nComponents, GL_FLOAT, GL_TRUE, nComponents*sizeof(GLfloat), (char*)0 + normsMemOffset);
        }
        else {
            glDisableVertexAttribArray(attribID);
        }

        // attribute 2 = colors (optional)
        attribID = 2;
        if (colors_.size()) {
            nComponents = 4;
            glEnableVertexAttribArray(attribID);
            glVertexAttribPointer(attribID, nComponents, GL_FLOAT, GL_TRUE, nComponents*sizeof(GLfloat), (char*)0 + colorsMemOffset);
        }
        else {
            glDisableVertexAttribArray(attribID);
        }
        
        // attribute(s) 3 to 7 = texture coordinates (optional)
        nComponents = 2;
        for (int i=0; i<std::min((int)tex_coords_.size(),(int)MAX_TEX_ATTRIBS); i++) {
            attribID = 3+i;
            if (tex_coords_[i].size()) {
                glEnableVertexAttribArray(attribID);
                glVertexAttribPointer(attribID, nComponents, GL_FLOAT, GL_FALSE, nComponents*sizeof(GLfloat), (char*)0 + texCoordsMemOffset[i]);
            }
            else {
                glDisableVertexAttribArray(attribID);
            }
        }

        // attribute 8-11 (takes 4 vec4 attribs to represent a single mat4) = instance transform matrices (optional)
        attribID = 8;
        if (instance_xforms_.size()) {
            glEnableVertexAttribArray(8);
            glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 16*sizeof(GLfloat), (char*)0 + instanceXformsMemOffset);
            glVertexAttribDivisor(8, 1);
            glEnableVertexAttribArray(9);
            glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, 16*sizeof(GLfloat), (char*)0 + instanceXformsMemOffset + 4*sizeof(GLfloat));
            glVertexAttribDivisor(9, 1);
            glEnableVertexAttribArray(10);
            glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, 16*sizeof(GLfloat), (char*)0 + instanceXformsMemOffset + 8*sizeof(GLfloat));
            glVertexAttribDivisor(10, 1);
            glEnableVertexAttribArray(11);
            glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, 16*sizeof(GLfloat), (char*)0 + instanceXformsMemOffset + 12*sizeof(GLfloat));
            glVertexAttribDivisor(11, 1);
        }
        else {
            glDisableVertexAttribArray(8);
            glDisableVertexAttribArray(9);
            glDisableVertexAttribArray(10);
            glDisableVertexAttribArray(11);
        }
        
        glBindVertexArray(0);
        
        if (indices_.size()) {
            glGenBuffers(1, &element_buffer_);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), &indices_[0], GL_STATIC_DRAW);
        }
        
        
        gpu_dirty_ = false;
    }
}


void Mesh::BuildBVH() {
    bvh_.CreateFromMesh(*this);
    bvh_dirty_ = false;
}


BVH* Mesh::bvh_ptr() {
    if (bvh_dirty_) {
        BuildBVH();
    }
    return &bvh_;
}


void Mesh::Draw() {
    if (gpu_dirty_) {
        UpdateGPUMemory();
    }
    
    // set defaults to pass to shaders any for optional attribs
    glVertexAttrib3f(1, 0.0, 0.0, 1.0);        // normal = +Z
    glVertexAttrib4f(2, 1.0, 1.0, 1.0, 1.0);  // color = opaque white
    glVertexAttrib2f(3, 0.0, 0.0);             // uv = 0,0 for texture unit 0
    glVertexAttrib2f(4, 0.0, 0.0);             // uv = 0,0 for texture unit 1
    glVertexAttrib2f(5, 0.0, 0.0);             // uv = 0,0 for texture unit 2
    glVertexAttrib2f(6, 0.0, 0.0);             // uv = 0,0 for texture unit 3
    glVertexAttrib2f(7, 0.0, 0.0);             // uv = 0,0 for texture unit 4
    glVertexAttrib4f(8, 1.0, 0.0, 0.0, 0.0);  // instance transform col 1
    glVertexAttrib4f(9, 0.0, 1.0, 0.0, 0.0);  // instance transform col 2
    glVertexAttrib4f(10, 0.0, 0.0, 1.0, 0.0); // instance transform col 3
    glVertexAttrib4f(11, 0.0, 0.0, 0.0, 1.0); // instance transform col 4
    
    
    glBindVertexArray(vertex_array_);
    
    if (instance_xforms_.size()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
        glDrawElementsInstanced(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, (void*)0, instance_xforms_.size()/16);
    }
    else if (indices_.size()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
        glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, (void*)0);
    }
    else {
        glDrawArrays(GL_TRIANGLES, 0, num_vertices());
    }
    
    glBindVertexArray(0);
}


int Mesh::num_vertices() const {
    return verts_.size()/3;
}

int Mesh::num_triangles() const {
    if (indices_.size()) {
        return indices_.size()/3;
    }
    else {
        return verts_.size()/9;
    }
}



void Mesh::LoadFromOBJ(const std::string &filename) {
    std::fstream file(filename.c_str(), std::ios::in);
    if (!file) {
        std::cerr << "Failed to load " + filename << std::endl;
        exit(1);
    }
    
    // tmp arrays
    std::vector<Point3> vertices;
    std::vector<Vector3> normals;
    std::vector<Point2> texCoords;
    
    while (file) {
        std::string line;
        do
            getline(file, line);
        while (file && (line.length() == 0 || line[0] == '#'));
        std::stringstream linestream(line);
        std::string keyword;
        linestream >> keyword;
        if (keyword == "v") {
            Point3 vertex;
            linestream >> vertex[0] >> vertex[1] >> vertex[2];
            vertices.push_back(vertex);
        } else if (keyword == "vn") {
            Vector3 normal;
            linestream >> normal[0] >> normal[1] >> normal[2];
            normals.push_back(normal);
        } else if (keyword == "vt") {
            Point2 texCoord;
            linestream >> texCoord[0] >> texCoord[1];
            texCoords.push_back(texCoord);
        } else if (keyword == "f") {
            std::vector<int> polygon;
            std::string word;
            while (linestream >> word) {
                std::stringstream wstream(word);
                int v;
                wstream >> v;
                polygon.push_back(v-1); // In OBJ files, indices start from 1
            }
            for (int i = 2; i < polygon.size(); i++) {
                //triangles.push_back(ivec3(polygon[0], polygon[i-1], polygon[i]));
                int i1 = polygon[0];
                int i2 = polygon[i-1];
                int i3 = polygon[i];
                //int t = AddTriangle(vertices[i1], vertices[i2], vertices[i3]);
                //if (normals.size()) {
                //    SetNormals(t, normals[i1], normals[i2], normals[i3]);
                //}
                //if (texCoords.size()) {
                //    SetTexCoords(t, 0, texCoords[i1], texCoords[i2], texCoords[i3]);
                //}
                
                indices_.push_back(i1);
                indices_.push_back(i2);
                indices_.push_back(i3);
            }
        }
    }
    
    gpu_dirty_ = true;
    std::vector<float> verts, norms, uvs;
    for (int i=0;i<vertices.size();i++) {
        verts_.push_back(vertices[i][0]);
        verts_.push_back(vertices[i][1]);
        verts_.push_back(vertices[i][2]);
        if (normals.size()) {
            norms_.push_back(normals[i][0]);
            norms_.push_back(normals[i][1]);
            norms_.push_back(normals[i][2]);
        }
        if (texCoords.size()) {
            uvs.push_back(texCoords[i][0]);
            uvs.push_back(texCoords[i][1]);
        }
    }
    if (uvs.size()) {
        tex_coords_.push_back(uvs);
    }
}



Point3 Mesh::vertex(int i) const {
    return Point3(verts_[3*i], verts_[3*i+1], verts_[3*i+2]);
}

Vector3 Mesh::normal(int i) const {
    return Vector3(norms_[3*i], norms_[3*i+1], norms_[3*i+2]);
}

Color Mesh::color(int i) const {
    return Color(colors_[4*i], colors_[4*i+1], colors_[4*i+2], colors_[4*i+3]);
    
}

Point2 Mesh::tex_coords(int textureUnit, int i) const {
    return Point2(tex_coords_[textureUnit][2*i], tex_coords_[textureUnit][2*i+1]);
}

std::vector<unsigned int> Mesh::triangle_vertices(int triangle_id) const {
    std::vector<unsigned int> tri;
    int i = 3*triangle_id;
    if (indices_.size()) {
        // indexed faces mode
        tri.push_back(indices_[i+0]);
        tri.push_back(indices_[i+1]);
        tri.push_back(indices_[i+2]);
    }
    else {
        // ordered faces mode
        tri.push_back(i);
        tri.push_back(i+1);
        tri.push_back(i+2);
    }
    return tri;
}


void Mesh::CalcPerFaceNormals() {
    std::vector<Vector3> norms(num_vertices());
    for (int i=0; i<num_triangles(); i++) {
        std::vector<unsigned int> indices = triangle_vertices(i);
        Point3 a = vertex(indices[0]);
        Point3 b = vertex(indices[1]);
        Point3 c = vertex(indices[2]);
        Vector3 n = Vector3::Cross(b-a, c-a).ToUnit();
        norms[indices[0]] = n;
        norms[indices[1]] = n;
        norms[indices[2]] = n;
    }
    SetNormals(norms);
}


void Mesh::CalcPerVertexNormals() {
    std::vector<Vector3> norms(num_vertices());
    for (int i=0; i<num_triangles(); i++) {
        std::vector<unsigned int> indices = triangle_vertices(i);
        Point3 a = vertex(indices[0]);
        Point3 b = vertex(indices[1]);
        Point3 c = vertex(indices[2]);
        Vector3 n = Vector3::Cross(b-a, c-a);
        norms[indices[0]] = norms[indices[0]] + n;
        norms[indices[1]] = norms[indices[1]] + n;
        norms[indices[2]] = norms[indices[2]] + n;
    }
    
    for (int i=0; i<norms.size(); i++) {
        norms[i] = norms[i].ToUnit();
    }
    
    SetNormals(norms);
}

    
} // end namespace
