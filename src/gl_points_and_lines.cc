/*
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 See corresponding header file for details.
 */

#include "gl_points_and_lines.h"

namespace mingfx {
    
#define MAX_TEX_ATTRIBS 5


GLPointsAndLines::GLPointsAndLines() : gpu_dirty_(true), vertex_buffer_(0), vertex_array_(0), element_buffer_(0) {
}

GLPointsAndLines::GLPointsAndLines(const GLPointsAndLines &other) {
    verts_ = other.verts_;
    colors_ = other.colors_;
    gpu_dirty_ = true;
}

GLPointsAndLines::~GLPointsAndLines() {
}
    

void GLPointsAndLines::SetVertices(const std::vector<Point3> &verts, DrawMode draw_mode) {
    draw_mode_ = draw_mode;
    gpu_dirty_ = true;

    verts_.clear();
    for (int i=0; i<verts.size(); i++) {
        verts_.push_back(verts[i][0]);
        verts_.push_back(verts[i][1]);
        verts_.push_back(verts[i][2]);
    }
}


void GLPointsAndLines::SetColors(const std::vector<Color> &colors) {
    gpu_dirty_ = true;
    colors_.clear();
    for (int i=0; i<colors.size(); i++) {
        colors_.push_back(colors[i][0]);
        colors_.push_back(colors[i][1]);
        colors_.push_back(colors[i][2]);
        colors_.push_back(colors[i][3]);
    }
}



void GLPointsAndLines::UpdateGPUMemory() {
    if (gpu_dirty_) {
        // sanity check -- for each attribute that is added (normals, colors, texcoords)
        // make sure the number of triangles is equal to the number of tris in the verts
        // array.
        if ((colors_.size() != 0) && (colors_.size() / 4 != num_vertices())) {
            std::cerr << "GLPointsAndLines::UpdateGPUMemory() -- warning: the number of per vertex colors in the mesh is not equal to the number vertices in the mesh. (C = " << colors_.size() / 4 << ", V = " << num_vertices() << ")" << std::endl;
        }

        GLsizeiptr totalMemSize = 0;

        GLsizeiptr vertsMemSize = verts_.size() * sizeof(float);
        GLsizeiptr vertsMemOffset = 0;
        totalMemSize += vertsMemSize;

        GLsizeiptr colorsMemSize = colors_.size() * sizeof(float);
        GLsizeiptr colorsMemOffset = totalMemSize;
        totalMemSize += colorsMemSize;
        
        glGenBuffers(1, &vertex_buffer_);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
        glBufferData(GL_ARRAY_BUFFER, totalMemSize, NULL, GL_STATIC_DRAW);

        glBufferSubData(GL_ARRAY_BUFFER, vertsMemOffset, vertsMemSize, &verts_[0]);
        if (colors_.size() > 0) {
            glBufferSubData(GL_ARRAY_BUFFER, colorsMemOffset, colorsMemSize, &colors_[0]);
        }
        glGenVertexArrays(1, &vertex_array_);
        glBindVertexArray(vertex_array_);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
        
        // attribute 0 = vertices (required)
        int attribID = 0;
        int nComponents = 3;
        glEnableVertexAttribArray(attribID);
        glVertexAttribPointer(attribID, nComponents, GL_FLOAT, GL_FALSE, nComponents*sizeof(GLfloat), (char*)0 + vertsMemOffset);

        // attribute 1 = colors (optional)
        attribID = 1;
        if (colors_.size()) {
            nComponents = 4;
            glEnableVertexAttribArray(attribID);
            glVertexAttribPointer(attribID, nComponents, GL_FLOAT, GL_TRUE, nComponents*sizeof(GLfloat), (char*)0 + colorsMemOffset);
        }
        else {
            glDisableVertexAttribArray(attribID);
        }
        
        glBindVertexArray(0);
                
        gpu_dirty_ = false;
    }
}


void GLPointsAndLines::Init() {
    const char *vertexShaderSource = "#version 330 core\n"
                "layout (location = 0) in vec3 position;\n"
                "layout (location = 1) in vec4 color;\n"
                "uniform mat4 MVP;\n"
                "out vec4 colInterp;\n"
                "void main() {\n"
                "   gl_Position = MVP * vec4(position, 1);\n"
                "   colInterp = color;\n"
                "}\0";
    const char *fragmentShaderSource = "#version 330 core\n"
                "in vec4 colInterp;\n"
                "out vec4 fragColor;\n"
                "void main() {\n"
                "   fragColor = colInterp;\n"
                "}\n\0";
        
    line_shader_.AddVertexShaderFromSource(vertexShaderSource);
    line_shader_.AddFragmentShaderFromSource(fragmentShaderSource);
    line_shader_.LinkProgram();
}

void GLPointsAndLines::Draw(Matrix4 model_matrix, Matrix4 view_matrix, Matrix4 proj_matrix) {
    if (!line_shader_.initialized()) {
        Init();
    }
    
    if (gpu_dirty_) {
        UpdateGPUMemory();
    }
    
    line_shader_.UseProgram();
    Matrix4 mvp = proj_matrix * view_matrix * model_matrix;
    //ortho2D(Point2(0, window_height()), Point2(window_width(), 0));
    line_shader_.SetUniform("MVP", mvp);
    line_shader_.SetUniform("lineColor", Color(1,1,1));
    
    // set defaults to pass to shaders any for optional attribs
    glVertexAttrib4f(1, 1.0, 1.0, 1.0, 1.0);  // color = opaque white
    
    glBindVertexArray(vertex_array_);
    
    GLenum mode = GL_LINES;
    if (draw_mode_ == DrawMode::LINE_LOOP) {
        mode = GL_LINE_LOOP;
    }
    else if (draw_mode_ == DrawMode::LINE_STRIP) {
        mode = GL_LINE_LOOP;
    }
    if (draw_mode_ == DrawMode::POINTS) {
        mode = GL_POINTS;
    }
    
    glDrawArrays  (GL_LINES, 0, num_vertices());
    
    glBindVertexArray(0);

    line_shader_.StopProgram();
}


int GLPointsAndLines::num_vertices() const {
    return (int)verts_.size()/3;
}


    
} // end namespace
