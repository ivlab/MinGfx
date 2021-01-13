/*
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 See corresponding header file for details.
 */

#include "texture2d.h"
#include "platform.h"

#ifdef WIN32
  // this is not needed on OSX or Linux, it must pick up the symbols from
  // libnanogui.so, but it appears to be needed on Windows.
  #define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb_image.h>


#include <iostream>


namespace mingfx {

    
    
Texture2D::Texture2D(GLenum wrapMode, GLenum filterMode) :
    dataType_(GL_UNSIGNED_BYTE), data_ubyte_(NULL), data_float_(NULL),
    width_(0), height_(0), handleMemInternally_(true), texID_(0),
    wrapMode_(wrapMode), filterMode_(filterMode)
{
}

Texture2D::~Texture2D() {
    
    // Mem handled internally is always of type data_ubyte_ because that is
    // what the stbi image loading library returns
    if ((handleMemInternally_) && (data_ubyte_ != NULL)) {
        // BUG, TODO: Not sure why the call below does not seem to work.
        // There will be a mem leak unless we can call this somehow.
        //stbi_image_free(data_);
    }

    // This is how to delete GL's version of the texture on the GPU
    // but you have to be very careful with this.  For example, if we cause
    // C++ to make a tmp copy of the Texture2D or we do an assignment tex1=tex2
    // we now have two Texture2D objects pointing to the same OpenGL texture id.
    // If one of them is deleted before the other, then the other will not be
    // able to draw itself because the OpenGL tex id will be invalid.  For now,
    // this is "addressed" by simply skipping the glDeleteTextures call.  This
    // leads to some wasted OpenGL memory, and that would be a good thing to
    // fix in the future, maybe via a shared_ptr or static refcount that maps
    // opengl texids to a count of Texture2D objects that reference them.  Then,
    // only delete the opengl tex if the refcount would go to 0.
    //glDeleteTextures(1, &texID_);
}

    
bool Texture2D::InitFromFile(const std::string &filename) {
    handleMemInternally_ = true;
    dataType_ = GL_UNSIGNED_BYTE;


    std::cout << "Loading texture from file: " << filename << std::endl;

    if (Platform::FileExists(filename)) {
        stbi_set_unpremultiply_on_load(1);
        stbi_convert_iphone_png_to_rgb(1);
        int numChannels;
        data_ubyte_ = stbi_load(filename.c_str(), &width_, &height_, &numChannels, 4);
        if (data_ubyte_ == NULL) {
            std::cerr << "Texture2D: Failed to load file " << filename << " - " << stbi_failure_reason() << std::endl;
            return false;
        }
    }
    else {
        std::cerr << "Texture2D: File " << filename << " does not exist." << std::endl;
        return false;
    }
    
    return InitOpenGL();
}

bool Texture2D::InitFromBytes(int width, int height, const unsigned char * data) {
    handleMemInternally_ = false;
    width_ = width;
    height_ = height;
    data_ubyte_ = data;
    dataType_ = GL_UNSIGNED_BYTE;
    
    return InitOpenGL();
}

bool Texture2D::InitFromFloats(int width, int height, const float * data) {
    handleMemInternally_ = false;
    width_ = width;
    height_ = height;
    data_float_ = data;
    dataType_ = GL_FLOAT;
    
    return InitOpenGL();
}
    
bool Texture2D::InitOpenGL() {
    glGenTextures(1, &texID_);
    glBindTexture(GL_TEXTURE_2D, texID_);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode_);
    
    if (dataType_ == GL_UNSIGNED_BYTE) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, dataType_, data_ubyte_);
    }
    else if (dataType_ == GL_FLOAT) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, dataType_, data_float_);
    }
    else {
        std::cerr << "Texture2D: Unsupported texture data type " << dataType_ << "." << std::endl;
        return false;
    }

    return true;
}

    
bool Texture2D::UpdateFromBytes(const unsigned char * data) {
    dataType_ = GL_UNSIGNED_BYTE;
    data_ubyte_ = data;
    glBindTexture(GL_TEXTURE_2D, texID_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, dataType_, data_ubyte_);
    // presumably glTexSubImage2D is faster, but this crashes on OSX for some reason
    //glActiveTexture(texID_);
    //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width_, height_, GL_RGBA, dataType_, data_ubyte_);
    return true;
}

bool Texture2D::UpdateFromFloats(const float * data) {
    dataType_ = GL_FLOAT;
    data_float_ = data;
    glBindTexture(GL_TEXTURE_2D, texID_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, dataType_, data_float_);
    // presumably glTexSubImage2D is faster, but this crashes on OSX for some reason
    //glActiveTexture(texID_);
    //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width_, height_, GL_RGBA, dataType_, data_ubyte_);
    return true;
}
    

    
int Texture2D::width() const {
    return width_;
}

int Texture2D::height() const {
    return height_;
}


GLuint Texture2D::opengl_id() const {
    if (!initialized()) {
        std::cerr << "Texture2D: Warning, accessing opengl_id() before it has been initialized." << std::endl
        << "You might be calling opengl_id() before InitOpenGL().  Or, there might have been a" << std::endl
        << "error loading texture data or binding it to OpenGL." << std::endl;
    }
    return texID_;
}

GLenum Texture2D::wrap_mode() const {
    return wrapMode_;
}

GLenum Texture2D::filter_mode() const {
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

bool Texture2D::initialized() const {
    return texID_ != 0;
}

    
Color Texture2D::Pixel(int x, int y) const {
    int index = y*4*width() + x*4;
    
    if (dataType_ == GL_UNSIGNED_BYTE) {
        unsigned char r = data_ubyte_[index+0];
        unsigned char g = data_ubyte_[index+1];
        unsigned char b = data_ubyte_[index+2];
        unsigned char a = data_ubyte_[index+3];
        return Color((float)r/255.0, (float)g/255.0, (float)b/255.0, (float)a/255.0);
    }
    else if (dataType_ == GL_FLOAT) {
        float r = data_float_[index+0];
        float g = data_float_[index+1];
        float b = data_float_[index+2];
        float a = data_float_[index+3];
        return Color(r, g, b, a);
    }
    else {
        std::cerr << "Texture2D: Unsupported texture data type " << dataType_ << "." << std::endl;
        return Color();
    }
}
    
} // end namespace

