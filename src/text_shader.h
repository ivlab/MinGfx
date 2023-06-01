/*
 This file is part of the MinGfx Project.
 
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 
 Original Author(s) of this File:
	Dan Keefe, 2018, University of Minnesota
	
 Author(s) of Significant Updates/Modifications to the File:
	...
 */

#ifndef SRC_TEXT_SHADER_H_
#define SRC_TEXT_SHADER_H_

#include <string>
#include <map>

#include "matrix4.h"
#include "mesh.h"
#include "shader_program.h"
#include "texture2d.h"

// disable warnings for this 3rd party code
#pragma warning ( push, 0 )
#include <stb_truetype.h>
#pragma warning ( pop )

namespace mingfx {
    


/**
 */
class TextShader {
public:
    TextShader();
    virtual ~TextShader();
    
    /// Call this from within the InitOpenGL() function since it will initialize
    /// not just the Font's internal data but also an OpenGL texture to be
    /// stored on the graphics card.  Internally, this uses the stb_truetype
    /// library to load true type fonts (files with a .ttf extension).
    bool Init(const std::string &font_file, int native_font_size);
    
    enum class HorizAlign {
        HORIZ_ALIGN_LEFT,
        HORIZ_ALIGN_CENTER,
        HORIZ_ALIGN_RIGHT
    };

    enum class VertAlign {
        VERT_ALIGN_TOP,
        VERT_ALIGN_CENTER,
        VERT_ALIGN_BASELINE,
        VERT_ALIGN_BOTTOM
    };
    
    class TextFormat {
    public:
        // constructor sets defaults
        TextFormat() :
            size(0.1f),
            color(1, 1, 1, 1),
            h_align(HorizAlign::HORIZ_ALIGN_CENTER),
            v_align(VertAlign::VERT_ALIGN_BASELINE),
            flip_y(false) {}

        float size;
        Color color;
        HorizAlign h_align;
        VertAlign v_align;
        bool flip_y;
    };
    

    void Draw2D(const Matrix4& projection, const Point2& pos,
        const std::string& text, TextFormat format, bool cache = false);

    void Draw2D(const Matrix4& projection, const float x_pos, const float y_pos,
        const std::string& text, TextFormat format, bool cache = false);
    
    void Draw3D(const Matrix4 &model, const Matrix4 &view, const Matrix4 &projection,
                const std::string &text, TextFormat format, bool cache=false);

    
    Vector2 TextExtents(const std::string &text, TextFormat format, bool cache=false);
    
    float native_font_size();
    
private:
    Texture2D atlas_;
    float native_font_size_;
    
    stbtt_packedchar chardata_[128];
    
    struct MeshData {
        Mesh mesh;
        Point2 min;
        Point2 max;
    };
    
    void SetTextMesh(const std::string &text, MeshData *md);

    std::map<std::string, MeshData> cache_;
    MeshData tmp_md_;

    ShaderProgram shader_;
};
    
} // end namespace

#endif
