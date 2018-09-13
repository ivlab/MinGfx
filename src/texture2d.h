/*
 This file is part of the MinGfx Project.
 
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 
 Original Author(s) of this File:
	Dan Keefe, 2018, University of Minnesota
	
 Author(s) of Significant Updates/Modifications to the File:
	...
 */

#ifndef SRC_TEXTURE2D_H_
#define SRC_TEXTURE2D_H_


#include "opengl_headers.h"
#include "color.h"

#include <string>


namespace mingfx {

/** A wrapper around a 2D texture that supports loading images from files or
 setting texture color data directly.  Example:
 ~~~
 Texture2D tex1;
 Texture2D tex2(GL_CLAMP_TO_EDGE);
 
 void MyGraphicsApp::InitOpenGL() {
     std::vector<std::string> search_path;
     search_path.push_back(".");
     search_path.push_back("./data");
     search_path.push_back("./shaders");
     tex1.InitFromFile(Platform::FindFile("earth-2k.png", search_path));
     tex2.InitFromFile(Platform::FindFile("toon-ramp.png", search_path));
 }
 ~~~
 */
class Texture2D {
public:

    /// Creates an empty texture.  Optional parameters can be provided to set
    /// the texture wrap mode and filter mode.
    Texture2D(GLenum wrapMode=GL_REPEAT, GLenum filterMode=GL_LINEAR);
    virtual ~Texture2D();

    
    /// Call this from within the InitOpenGL() function since it will initialize
    /// not just the Texture2D's internal data but also an OpenGL texture to be
    /// stored on the graphics card.  Internally, this uses the stbi library to
    /// load images.  It supports png, jpg, bmp, and other file formats.
    bool InitFromFile(const std::string &filename);
    
    /// Call this from within the InitOpenGL() function since it will initialize
    /// not just the Texture2D's internal data but also an OpenGL texture to be
    /// stored on the graphics card.
    /// With this version of Init, you may pass in your own pointer to color data.
    /// The data argument must point to an array of 4-channel color data stored as
    /// unsigned chars in RGBA format.  You are responsible for managing the memory
    /// for this array.  If you will never call Pixel(), then it is safe to free
    /// data as soon as this function returns.  Otherwise, you need to make sure
    /// data does not change in memory until you destroy the Texture2D object.
    bool InitFromBytes(int width, int height, const unsigned char * data);
    
    /// Call this from within the InitOpenGL() function since it will initialize
    /// not just the Texture2D's internal data but also an OpenGL texture to be
    /// stored on the graphics card.
    /// With this version of Init, you may pass in your own pointer to color data.
    /// The data argument must point to an array of 4-channel color data stored as
    /// floats in RGBA format.  You are responsible for managing the memory
    /// for this array.  If you will never call Pixel(), then it is safe to free
    /// data as soon as this function returns.  Otherwise, you need to make sure
    /// data does not change in memory until you destroy the Texture2D object.
    bool InitFromFloats(int width, int height, const float * data);

    
    /// This function may be called to re-read the texture data from an array
    /// formated the same as in InitFromBytes.  The width and height of the
    /// texture must remain the same.
    bool UpdateFromBytes(const unsigned char * data);

    /// This function may be called to re-read the texture data from an array
    /// formated the same as in InitFromFloats.  The width and height of the
    /// texture must remain the same.
    bool UpdateFromFloats(const float * data);

    
    /// Returns true if the texture data has been successfully transferred to OpenGL.
    bool initialized() const;
    
    /// Returns the width in pixels of the texture.
    int width() const;
    
    /// Returns the height in pixels of the texture.
    int height() const;

    /// Returns the unsigned int used as the texture handle by OpenGL
    GLuint opengl_id() const;

    /// Returns an enumerated constant for the OpenGL wrap mode used by the texture.
    GLenum wrap_mode() const;

    /// Returns an enumerated constant for the OpenGL filter mode used by the texture.
    GLenum filter_mode() const;
    
    /// Uses the OpenGL texture wrap mode arguments
    void set_wrap_mode(GLenum wrapMode);

    /// Uses the OpenGL texture filter mode arguments
    void set_filter_mode(GLenum filterMode);
    
    /// Returns the color at the specified pixel.  The top left corner of the
    /// image is (0,0) and the bottom right is (width()-1, height()-1).
    Color Pixel(int x, int y) const;
    
private:
    
    bool InitOpenGL();
    
    GLenum dataType_; // GL_UNSIGNED_BYTE or GL_FLOAT
    const unsigned char * data_ubyte_;
    const float * data_float_;
    
    int width_;
    int height_;
    bool handleMemInternally_;
    
    GLuint texID_;
    GLenum wrapMode_;
    GLenum filterMode_;
};

    
} // end namespace

#endif