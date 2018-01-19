/**
 */


#ifndef SRC_TEXTURE2D_H_
#define SRC_TEXTURE2D_H_


#include "opengl_headers.h"

#include <string>


namespace MinGfx {

    
class Texture2D {
public:

    Texture2D(const std::string &filename, GLenum wrapMode=GL_REPEAT, GLenum filterMode=GL_LINEAR);
    
    /// With this constructor, you may pass in your own pointer to color data.
    /// The data argument must point to an array of 4-channel color data stored as
    /// unsigned chars in RGBA format.  You are responsible for managing the memory
    /// for this array, and you must not free it until after you call InitOpenGL.
    /// At that point, the data will be copied over into an OpenGL texture, and it
    /// is fine to free the memory in your data array.
    Texture2D(int width, int height, unsigned char* data, GLenum wrapMode=GL_REPEAT, GLenum filterMode=GL_LINEAR);

    virtual ~Texture2D();
    
    
    bool initialized() { return texID_ != 0; }

    void InitOpenGL();

    
    int width();
    int height();

    GLuint opengl_id();
    GLenum wrap_mode();
    GLenum filter_mode();
        
    void set_wrap_mode(GLenum wrapMode);
    void set_filter_mode(GLenum filterMode);
    
private:
    
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