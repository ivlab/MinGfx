
#include "texture2d.h"
#include "platform.h"

#include <stb_image.h>

#include <iostream>


namespace MinGfx {

Texture2D::Texture2D(const std::string &filename, GLenum wrapMode, GLenum filterMode) :
    data_(NULL), width_(0), height_(0), handleMemInternally_(true), texID_(0),
    wrapMode_(wrapMode), filterMode_(filterMode)
{
    if (Platform::fileExists(filename)) {
        stbi_set_unpremultiply_on_load(1);
        stbi_convert_iphone_png_to_rgb(1);
        int numChannels;
        data_ = stbi_load(filename.c_str(), &width_, &height_, &numChannels, 4);
        if (data_ == NULL) {
            std::cerr << "Texture2D: Failed to load file " << filename << " - " << stbi_failure_reason() << std::endl;
        }
    }
    else {
        std::cerr << "Texture2D: File " << filename << " does not exist." << std::endl;
    }
}

Texture2D::Texture2D(int width, int height, unsigned char* data, GLenum wrapMode, GLenum filterMode) :
    data_(data), width_(width), height_(height), handleMemInternally_(false), texID_(0),
    wrapMode_(wrapMode), filterMode_(filterMode)
{
}
    
void Texture2D::InitOpenGL() {
    glGenTextures(1, &texID_);
    glBindTexture(GL_TEXTURE_2D, texID_);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode_);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_);

    if (handleMemInternally_) {
        stbi_image_free(data_);
    }
}

Texture2D::~Texture2D() {
    // TODO: free texture from OpenGL
}


int Texture2D::width() {
    return width_;
}

int Texture2D::height() {
    return height_;
}


GLuint Texture2D::opengl_id() {
    if (!initialized()) {
        std::cerr << "Texture2D: Warning, accessing opengl_id() before it has been initialized." << std::endl
        << "You might be calling opengl_id() before InitOpenGL().  Or, there might have been a" << std::endl
        << "error loading texture data or binding it to OpenGL." << std::endl;
    }
    return texID_;
}

GLenum Texture2D::wrap_mode() {
    return wrapMode_;
}

GLenum Texture2D::filter_mode() {
    return filterMode_;
}

void Texture2D::set_wrap_mode(GLenum wrapMode) {
    wrapMode_ = wrapMode;
    glBindTexture(GL_TEXTURE_2D, texID_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode_);
}

void Texture2D::set_filter_mode(GLenum filterMode) {
    filterMode_ = filterMode;
    glBindTexture(GL_TEXTURE_2D, texID_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode_);
}

    
} // end namespace

