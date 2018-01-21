/**
 */


#ifndef SRC_TEXTURE2D_H_
#define SRC_TEXTURE2D_H_


#include "opengl_headers.h"

#include <string>


namespace MinGfx {

    
class Texture2D {
public:

    Texture2D(GLenum wrapMode=GL_REPEAT, GLenum filterMode=GL_LINEAR);
    virtual ~Texture2D();

    
    /// Call this from within the InitOpenGL() function since it will initialize
    /// not just the Texture2D's internal data but also an OpenGL texture to be
    /// stored on the graphics card.
    bool InitFromFile(const std::string &filename);
    
    /// Call this from within the InitOpenGL() function since it will initialize
    /// not just the Texture2D's internal data but also an OpenGL texture to be
    /// stored on the graphics card.
    /// With this version of Init, you may pass in your own pointer to color data.
    /// The data argument must point to an array of 4-channel color data stored as
    /// unsigned chars in RGBA format.  You are responsible for managing the memory
    /// for this array.  It is safe to free it as soon as this function returns.
    bool InitFromData(int width, int height, unsigned char* data);
    
    
    bool initialized() const { return texID_ != 0; }

    int width() const;
    int height() const;

    GLuint opengl_id() const;
    GLenum wrap_mode() const;
    GLenum filter_mode() const;
        
    void set_wrap_mode(GLenum wrapMode);
    void set_filter_mode(GLenum filterMode);
    
private:
    
    bool InitOpenGL();
    
    unsigned char *data_;
    int width_;
    int height_;
    bool handleMemInternally_;
    
    GLuint texID_;
    GLenum wrapMode_;
    GLenum filterMode_;
};

    
} // end namespace

#endif