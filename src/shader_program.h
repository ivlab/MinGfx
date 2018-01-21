
#ifndef SRC_SHADERPROGRAM_H_
#define SRC_SHADERPROGRAM_H_

#include "color.h"
#include "matrix4.h"
#include "opengl_headers.h"
#include "point2.h"
#include "point3.h"
#include "texture2d.h"
#include "vector2.h"
#include "vector3.h"

#include <string>
#include <map>

namespace MinGfx {

class ShaderProgram {
public:
    /// Creates an empty ShaderProgram object.
    ShaderProgram();
    
    virtual ~ShaderProgram();
    
    
    // ---- These should be called during startup (e.g., in InitOpenGL()) ----
    
    bool AddVertexShaderFromFile(const std::string &file);
    bool AddVertexShaderFromSource(const std::string &code);
    
    bool AddFragmentShaderFromFile(const std::string &file);
    bool AddFragmentShaderFromSource(const std::string &code);
    
    bool LinkProgram();
    

    
    // ---- These should be called during rendering (e.g., in DrawUsingOpenGL()) ----

    /// Call this first to make the shader program active, then call SetUniform() to
    /// pass data from your C++ program into the shader code via the named uniform
    /// variables that appear in the code.  Then render whatever geometry you wish
    /// with your own glDrawArrays() call(s).  Finally, call StopProgram() to turn
    /// off the shader program.
    void UseProgram();
    
    // Set Uniform Variables in the Shader
    
    // MinGfx types
    void SetUniform(const std::string &name, const Point2 &p);
    void SetUniform(const std::string &name, const Vector2 &v);
    void SetUniform(const std::string &name, const Point3 &p);
    void SetUniform(const std::string &name, const Vector3 &v);
    void SetUniform(const std::string &name, const Matrix4 &m);
    void SetUniform(const std::string &name, const Color &c);
    
    // built-in types
    void SetUniform(const std::string &name, int i);
    void SetUniform(const std::string &name, unsigned int ui);
    void SetUniform(const std::string &name, float f);
    void SetUniform(const std::string &name, double d);
    
    // built-in types (arrays)
    void SetUniformArray(const std::string &name, int *i, int count);
    void SetUniformArray(const std::string &name, unsigned int *ui, int count);
    void SetUniformArray(const std::string &name, float *f, int count);
    void SetUniformArray(const std::string &name, double *d, int count);

    
    // Set Textures (Sampler Variables in the Shader)
    
    /// This version automatically selects an available texture unit, i.e., one
    /// not already used by this shader program.
    void BindTexture(const std::string &name, const Texture2D &tex);
    
    /// This version allows you to specify the texture unit to use.
    void BindTexture(const std::string &name, const Texture2D &tex, int texUnit);
    
    
    void StopProgram();
    
    bool initialized();
    
private:
    GLuint vertexShader_;
    GLuint fragmentShader_;
    GLuint program_;
    std::map<std::string, int> texBindings_;
};
    
    
} // end namespace

#endif
