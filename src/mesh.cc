#include "mesh.h"

#include "opengl_headers.h"

#include <sstream>
#include <fstream>

namespace MinGfx {
    
    Mesh::Mesh() : gpuDirty_(true) {
    }
        
    Mesh::~Mesh() {
    }
        
    int Mesh::AddTriangle(Point3 v1, Point3 v2, Point3 v3) {
        gpuDirty_ = true;
        gpuDirty_ = true;

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

    void Mesh::UpdateTriangle(int triangleID, Point3 v1, Point3 v2, Point3 v3) {
        gpuDirty_ = true;
        
        int index = triangleID * 9;
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

    
    void Mesh::SetNormals(int triangleID, Vector3 n1, Vector3 n2, Vector3 n3) {
        gpuDirty_ = true;

        if (triangleID >= num_triangles()) {
            std::cerr << "Mesh::SetNormals() -- warning: cannot set normals for non-existant triangle with ID=" << triangleID << ".  Make sure the triangle has been added first." << std::endl;
            return;
        }

        int requiredSize = (triangleID+1)*9;
        if (norms_.size() < requiredSize) {
            norms_.resize(requiredSize);
        }
        int index = triangleID * 9;
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

    void Mesh::SetColors(int triangleID, Color c1, Color c2, Color c3) {
        gpuDirty_ = true;

        if (triangleID >= num_triangles()) {
            std::cerr << "Mesh::SetColors() -- warning: cannot set colors for non-existant triangle with ID=" << triangleID << ".  Make sure the triangle has been added first." << std::endl;
            return;
        }
        
        int requiredSize = (triangleID+1)*12;
        if (colors_.size() < requiredSize) {
            colors_.resize(requiredSize);
        }
        int index = triangleID * 12;
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

    void Mesh::SetTexCoords(int triangleID, int textureUnit, Point2 uv1, Point2 uv2, Point2 uv3) {
        gpuDirty_ = true;

        if (triangleID >= num_triangles()) {
            std::cerr << "Mesh::SetTexCoords() -- warning: cannot set texture coordinates for non-existant triangle with ID=" << triangleID << ".  Make sure the triangle has been added first." << std::endl;
            return;
        }
        
        // resize as needed based on the number of textureUnits used
        if (texCoords_.size() < textureUnit+1) {
            texCoords_.resize(textureUnit+1);
        }

        // resize the textureUnit-specific array based on the number of triangles
        int requiredSize = (triangleID+1)*6;
        if (texCoords_[textureUnit].size() < requiredSize) {
            texCoords_[textureUnit].resize(requiredSize);
        }
        int index = triangleID * 6;
        texCoords_[textureUnit][index + 0] = uv1[0];
        texCoords_[textureUnit][index + 1] = uv1[1];

        texCoords_[textureUnit][index + 2] = uv2[0];
        texCoords_[textureUnit][index + 3] = uv2[1];

        texCoords_[textureUnit][index + 4] = uv3[0];
        texCoords_[textureUnit][index + 5] = uv3[1];
    }

    
    void Mesh::SetVertices(float *vertsArray, int numVerts) {
        gpuDirty_ = true;
        verts_.clear();
        int numFloats = numVerts * 3;
        for (int i=0; i<numFloats; i++) {
            verts_.push_back(vertsArray[i]);
        }
    }
    
    void Mesh::SetNormals(float *normsArray, int numNorms) {
        gpuDirty_ = true;
        norms_.clear();
        int numFloats = numNorms * 3;
        for (int i=0; i<numFloats; i++) {
            norms_.push_back(normsArray[i]);
        }
    }
    
    void Mesh::SetColors(float *colorsArray, int numColors) {
        gpuDirty_ = true;
        colors_.clear();
        int numFloats = numColors * 4;
        for (int i=0; i<numFloats; i++) {
            colors_.push_back(colorsArray[i]);
        }
    }
    
    void Mesh::SetTexCoords(int textureUnit, float *texCoordsArray, int numTexCoords) {
        gpuDirty_ = true;
        // resize as needed based on the number of textureUnits used
        if (texCoords_.size() < textureUnit+1) {
            texCoords_.resize(textureUnit+1);
        }
        texCoords_[textureUnit].clear();
        int numFloats = numTexCoords * 2;
        for (int i=0; i<numFloats; i++) {
            texCoords_[textureUnit].push_back(texCoordsArray[i]);
        }
    }
    
    
    void Mesh::UpdateGPUMemory() {
        if (gpuDirty_) {
            // sanity check -- for each attribute that is added (normals, colors, texcoords)
            // make sure the number of triangles is equal to the number of tris in the verts
            // array.
            if ((norms_.size() != 0) && (norms_.size()/3 != num_vertices())) {
                std::cerr << "Mesh::UpdateGPUMemory() -- warning: the number of per vertex normals in the mesh is not equal to the number vertices in the mesh. (N = " << norms_.size()/3 << ", V = " << num_vertices() << ")" << std::endl;
            }
            if ((colors_.size() != 0) && (colors_.size()/4 != num_vertices())) {
                std::cerr << "Mesh::UpdateGPUMemory() -- warning: the number of per vertex colors in the mesh is not equal to the number vertices in the mesh. (C = " << colors_.size()/4 << ", V = " << num_vertices() << ")" << std::endl;
            }
            for (int i=0; i<texCoords_.size(); i++) {
                if ((texCoords_[i].size() != 0) && (texCoords_[i].size()/2 != num_vertices())) {
                    std::cerr << "Mesh::UpdateGPUMemory() -- warning: the number of per vertex texture coordinates (for texture unit #" << i << ") is not equal to the number vertices in the mesh.  (UVs = " << texCoords_[i].size()/2 << ", V = " << num_vertices() << ")" << std::endl;
                }
            }
            
            GLsizeiptr totalMemSize = 0;
            
            GLsizeiptr vertsMemSize = verts_.size()*sizeof(float);
            GLsizeiptr vertsMemOffset = 0;
            totalMemSize += vertsMemSize;
            
            GLsizeiptr normsMemSize = norms_.size()*sizeof(float);
            GLsizeiptr normsMemOffset = totalMemSize;
            totalMemSize += normsMemSize;
            
            GLsizeiptr colorsMemSize = colors_.size()*sizeof(float);
            GLsizeiptr colorsMemOffset = totalMemSize;
            totalMemSize += colorsMemSize;
            
            std::vector<GLsizeiptr> texCoordsMemSize;
            std::vector<GLsizeiptr> texCoordsMemOffset;
            for (int i=0; i<texCoords_.size(); i++) {
                texCoordsMemSize.push_back(texCoords_[i].size()*sizeof(float));
                texCoordsMemOffset.push_back(totalMemSize);
                totalMemSize += texCoordsMemSize[i];
            }
            
            glGenBuffers(1, &vertexBuffer_);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
            glBufferData(GL_ARRAY_BUFFER, totalMemSize, NULL, GL_STATIC_DRAW);

            glBufferSubData(GL_ARRAY_BUFFER, vertsMemOffset, vertsMemSize, &verts_[0]);
            glBufferSubData(GL_ARRAY_BUFFER, normsMemOffset, normsMemSize, &norms_[0]);
            glBufferSubData(GL_ARRAY_BUFFER, colorsMemOffset, colorsMemSize, &colors_[0]);
            for (int i=0; i<texCoords_.size(); i++) {
                glBufferSubData(GL_ARRAY_BUFFER, texCoordsMemOffset[i], texCoordsMemSize[i], &(texCoords_[i][0]));
            }
            
            glGenVertexArrays(1, &vertexArray_);
            glBindVertexArray(vertexArray_);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
            
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
            
            // attribute(s) 3+ = texture coordinates (optional)
            nComponents = 2;
            for (int i=0; i<texCoords_.size(); i++) {
                attribID = 3+i;
                if (texCoords_[i].size()) {
                    glEnableVertexAttribArray(attribID);
                    glVertexAttribPointer(attribID, nComponents, GL_FLOAT, GL_FALSE, nComponents*sizeof(GLfloat), (char*)0 + texCoordsMemOffset[i]);
                }
                else {
                    glDisableVertexAttribArray(attribID);
                }
            }
            
            
            glBindVertexArray(0);
            gpuDirty_ = false;
        }
    }

    
    void Mesh::Draw() {
        if (gpuDirty_) {
            UpdateGPUMemory();
        }
        
        // set defaults to pass to shaders any for optional attribs
        glVertexAttrib3f(1, 0.0, 0.0, 1.0);      // normal = +Z
        glVertexAttrib4f(2, 1.0, 1.0, 1.0, 1.0); // color = opaque white
        glVertexAttrib2f(3, 0.0, 0.0);           // uv = 0,0 for texture unit 0
        glVertexAttrib2f(4, 0.0, 0.0);           // uv = 0,0 for texture unit 1 (assuming no need to go beyond that)
        
        glBindVertexArray(vertexArray_);
        glDrawArrays(GL_TRIANGLES, 0, num_vertices());
        glBindVertexArray(0);
    }
    
    
    int Mesh::num_vertices() const {
        return verts_.size()/3;
    }

    int Mesh::num_triangles() const {
        return verts_.size()/9;
    }

    
    
    void Mesh::LoadFromOBJ(const std::string &filename) {
        std::fstream file(filename.c_str(), std::ios::in);
        if (!file) {
            std::cerr << "Failed to load " + filename << std::endl;
            exit(1);
        }
        
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
                    int t = AddTriangle(vertices[i1], vertices[i2], vertices[i3]);
                    if (normals.size()) {
                        SetNormals(t, normals[i1], normals[i2], normals[i3]);
                    }
                    if (texCoords.size()) {
                        SetTexCoords(t, 0, texCoords[i1], texCoords[i2], texCoords[i3]);
                    }
                }
            }
        }
        UpdateGPUMemory();
    }
    
    
} // end namespace
