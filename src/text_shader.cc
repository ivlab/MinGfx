/*
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 See corresponding header file for details.
 */

#include "text_shader.h"

#include "platform.h"
#include <fstream>


#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>


namespace mingfx {

// Reference implementation: https://github.com/nothings/stb/blob/master/tests/oversample/main.c

        
TextShader::TextShader() {
    std::cout << "created text shader" << std::endl;
}

TextShader::~TextShader() {
}

    
bool TextShader::Init(const std::string &filename, int font_size) {
    // load shader
    shader_.AddVertexShaderFromFile(Platform::FindMinGfxShaderFile("text.vert"));
    shader_.AddFragmentShaderFromFile(Platform::FindMinGfxShaderFile("text.frag"));
    shader_.LinkProgram();

    // load font
    native_font_size_ = font_size;
    std::ifstream is(filename.c_str(), std::ifstream::binary);
    if (is) {
        is.seekg(0, is.end);
        int length = is.tellg();
        is.seekg(0, is.beg);
        
        char *ttf_buffer = new char[length];
        is.read(ttf_buffer, length);
        if (is)
            std::cout << "all characters read successfully.";
        else
            std::cout << "error: only " << is.gcount() << " could be read";
        is.close();

        // todo: calc an appropriate pow of 2 size given the font_size
        int atlas_width = 1024;
        int atlas_height = 1024;

        stbtt_pack_context pc;
        unsigned char *bitmap = new unsigned char[atlas_width * atlas_height];

        stbtt_PackBegin(&pc, bitmap, atlas_width, atlas_height, 0, 1, NULL);
        stbtt_PackSetOversampling(&pc, 2, 2);
        stbtt_PackFontRange(&pc, (unsigned char*)ttf_buffer, 0, font_size, 32, 95, chardata_+32);
        stbtt_PackEnd(&pc);
        
        // convert to 4-channel since that is all that Texture2D currently supports
        unsigned char *bitmap4D = new unsigned char[4 * atlas_width * atlas_height];
        for (int i=0; i < atlas_width * atlas_height; i++) {
            bitmap4D[4*i + 0] = bitmap[i];
            bitmap4D[4*i + 1] = bitmap[i];
            bitmap4D[4*i + 2] = bitmap[i];
            bitmap4D[4*i + 3] = bitmap[i];
        }
        
        atlas_.InitFromBytes(atlas_width, atlas_height, bitmap4D);
        
        delete [] ttf_buffer;
        delete [] bitmap;
        delete [] bitmap4D;
     
        return true;
    }
    else {
        std::cerr << "TextShader: Error font file does not exist: " << filename << std::endl;
        return false;
    }
    
}


void TextShader::Draw3D(const Matrix4 &model, const Matrix4 &view, const Matrix4 &projection,
                        const std::string &text, TextFormat format, bool cache)
{
    MeshData *md = NULL;
    std::map<std::string, MeshData>::iterator it = cache_.find(text);
    if (it != cache_.end()) {
        // use an existing cached mesh
        md = &(it->second);
    }
    else {
        // need to create a new mesh, add a new one to the cache or use the tmp_mesh
        if (cache) {
            MeshData new_md;
            cache_[text] = new_md;
            md = &(cache_[text]);
        }
        else {
            md = &tmp_md_;
        }
        
        // set appropriate vertices and texcoords for this text string
        SetTextMesh(text, md);
    }
    
    Vector3 offset;
    if (format.h_align == HORIZ_ALIGN_LEFT) {
        offset[0] = 0;
    }
    else if (format.h_align == HORIZ_ALIGN_CENTER) {
        offset[0] = -0.5 * (md->max[0] - md->min[0]);
    }
    else if (format.h_align == HORIZ_ALIGN_RIGHT) {
        offset[0] = -(md->max[0] - md->min[0]);
    }
    
    if (format.v_align == VERT_ALIGN_TOP) {
        offset[1] = -md->max[1];
    }
    else if (format.v_align == VERT_ALIGN_CENTER) {
        offset[1] = -0.5 * md->max[1];
    }
    else if (format.v_align == VERT_ALIGN_BASELINE) {
        offset[1] = 0;
    }
    else if (format.v_align == VERT_ALIGN_BOTTOM) {
        offset[1] = -md->min[1];
    }
    
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    shader_.UseProgram();
    Matrix4 mvp = projection * view * model;
    shader_.SetUniform("mvp_matrix", mvp);
    shader_.SetUniform("scale", format.size / native_font_size_);
    shader_.SetUniform("offset", offset);
    shader_.SetUniform("color", format.color);
    shader_.BindTexture("font_atlas", atlas_);
    md->mesh.Draw();
    shader_.StopProgram();

    glEnable(GL_CULL_FACE);
}
    

void TextShader::SetTextMesh(const std::string &text, MeshData *md) {
    std::vector<Point3> verts;
    std::vector<Point2> uvs;
    std::vector<unsigned int> indices;
    
    const char *c = text.c_str();
    float x = 0.0;
    float y = 0.0;
    while (*c) {
        stbtt_aligned_quad q;
        stbtt_GetPackedQuad(chardata_, atlas_.width(), atlas_.height(), *c++, &x, &y, &q, 0);
        
        // top left
        verts.push_back(Point3(q.x0, -q.y0, 0.0));
        uvs.push_back(Point2(q.s0, q.t0));
        // top right
        verts.push_back(Point3(q.x1, -q.y0, 0.0));
        uvs.push_back(Point2(q.s1, q.t0));
        // bot right
        verts.push_back(Point3(q.x1, -q.y1, 0.0));
        uvs.push_back(Point2(q.s1, q.t1));
        // bot left
        verts.push_back(Point3(q.x0, -q.y1, 0.0));
        uvs.push_back(Point2(q.s0, q.t1));
    
        
        indices.push_back(verts.size()-2);
        indices.push_back(verts.size()-3);
        indices.push_back(verts.size()-4);

        indices.push_back(verts.size()-2);
        indices.push_back(verts.size()-4);
        indices.push_back(verts.size()-1);
    }
    
    md->mesh.SetVertices(verts);
    md->mesh.SetTexCoords(0, uvs);
    md->mesh.SetIndices(indices);
    
    md->min = Point2(verts[0][0], verts[0][1]);
    md->max = md->min;
    for (int i=0; i<verts.size(); i++) {
        Point3 p = verts[i];
        if (p[0] < md->min[0]) md->min[0] = p[0];
        if (p[0] > md->max[0]) md->max[0] = p[0];
        if (p[1] < md->min[1]) md->min[1] = p[1];
        if (p[1] > md->max[1]) md->max[1] = p[1];
    }
}

Vector2 TextShader::TextExtents(const std::string &text, TextFormat format, bool cache) {
    MeshData *md = NULL;
    std::map<std::string, MeshData>::iterator it = cache_.find(text);
    if (it != cache_.end()) {
        // use an existing cached mesh
        md = &(it->second);
    }
    else {
        // need to create a new mesh, add a new one to the cache or use the tmp_mesh
        if (cache) {
            MeshData new_md;
            cache_[text] = new_md;
            md = &(cache_[text]);
        }
        else {
            md = &tmp_md_;
        }
        
        // set appropriate vertices and texcoords for this text string
        SetTextMesh(text, md);
    }

    return format.size / native_font_size_ * (md->max - md->min);
}
    

} // end namespace