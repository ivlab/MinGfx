/*
 This file is part of the MinGfx Project.
 
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 
 Original Author(s) of this File:
	Dan Keefe, 2018, University of Minnesota
	
 Author(s) of Significant Updates/Modifications to the File:
	...
 */

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

namespace mingfx {

/** A wrapper around GLSL shader programs.  This class supports loading vertex
 and fragment shaders from files or strings, compiling them, and then linking
 them into a shader program.  Uniform variables within the shader programs can 
 be set in order to pass parameters from C++ code into the shader program.
 Textures can also be bound to the shader.  Example usage:
 ~~~
 
 ShaderProgram shader_prog;
 
 void MyGraphicsApp::InitOpenGL() {
    shader_prog.AddVertexShaderFromFile(Platform::findFile("my_shader.vert", searchPath));
    shader_prog.AddFragmentShaderFromFile(Platform::findFile("my_shader.frag", searchPath));
    shader_prog.LinkProgram();
 }
 
 void MyGraphicsApp::DrawUsingOpenGL() {
     // Activate the shader program
     shader_prog.UseProgram();
     
     // Pass uniforms and textures from C++ to the GPU Shader Program
     shader_prog.SetUniform("ModelMatrix", modelMat);
     shader_prog.SetUniform("ViewMatrix", viewMat);
     shader_prog.SetUniform("ProjectionMatrix", projMat);
     shader_prog.SetUniform("LightPosition", Point3(2,2,2));
     shader_prog.BindTexture("SurfaceTexture", my_tex);
 
     // Draw whatever geometry you want now
     mesh1.Draw();
     mesh2.Draw();
 
     // Deactivate the shader program
     shader_prog.StopProgram();
 }
 ~~~
 */
class ShaderProgram {
public:
    /// Creates an empty ShaderProgram object.
    ShaderProgram();
    
    virtual ~ShaderProgram();
    
    
    // ---- These should be called during startup (e.g., in InitOpenGL()) ----
    
    /// Call during initialization but after the OpenGL context has been created
    /// (e.g., inside InitOpenGL()).  This loads the shader from  the file and
    /// compiles it.  An error will be printed to stderr if there are any
    /// compilation errors.
    bool AddVertexShaderFromFile(const std::string &file);

    /// This loads and compiles a shader from a string. An error will be printed
    /// to stderr if there are any compilation errors.
    bool AddVertexShaderFromSource(const std::string &code);
    
    /// Call during initialization but after the OpenGL context has been created
    /// (e.g., inside InitOpenGL()).  This loads the shader from  the file and
    /// compiles it.  An error will be printed to stderr if there are any
    /// compilation errors.
    bool AddFragmentShaderFromFile(const std::string &file);
    
    /// This loads and compiles a shader from a string. An error will be printed
    /// to stderr if there are any compilation errors.
    bool AddFragmentShaderFromSource(const std::string &code);
    
    /// Call this after adding vertex and fragment shaders in order to link them
    /// together to create the full shader program.  An error will be printed to
    /// stderr if there are any linking errors.
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
    
    /// Passes the x,y values of point p to the shader program and stores the
    /// result in the shader variable named name, which should be of type vec2.
    void SetUniform(const std::string &name, const Point2 &p);

    /// Passes the x,y values of vector v to the shader program and stores the
    /// result in the shader variable named name, which should be of type vec2.
    void SetUniform(const std::string &name, const Vector2 &v);

    /// Passes the x,y,z,1 values of point p to the shader program and stores the
    /// result in the shader variable named name, which should be of type vec4.
    void SetUniform(const std::string &name, const Point3 &p);

    /// Passes the x,y,z,0 values of vector v to the shader program and stores the
    /// result in the shader variable named name, which should be of type vec4.
    void SetUniform(const std::string &name, const Vector3 &v);

    /// Passes the column-major 16 float values of matrix m to the shader program
    /// and stores the result in the shader variable named name, which should be of type mat4.
    void SetUniform(const std::string &name, const Matrix4 &m);
    
    /// Passes the r,g,b,a values of color c to the shader program and stores the
    /// result in the shader variable named name, which should be of type vec4.
    void SetUniform(const std::string &name, const Color &c);
    
    
    // built-in types
    
    /// Passes the int to the shader program and stores the result in the shader
    /// variable named name, which should be of type int.
    void SetUniform(const std::string &name, int i);

    /// Passes the unsigned int to the shader program and stores the result in the shader
    /// variable named name, which should be of type uint.
    void SetUniform(const std::string &name, unsigned int ui);
    
    /// Passes the float to the shader program and stores the result in the shader
    /// variable named name, which should be of type float.
    void SetUniform(const std::string &name, float f);

    
    // built-in types (arrays)
    
    /// Passes an array of count ints to the shader program and stores the result
    /// in the shader variable named name, which should be of type int name[count].
    void SetUniformArray1(const std::string &name, int *i, int count);

    /// Passes an array of count unsigned ints to the shader program and stores the result
    /// in the shader variable named name, which should be of type uint name[count].
    void SetUniformArray1(const std::string &name, unsigned int *ui, int count);

    /// Passes an array of count floats to the shader program and stores the result
    /// in the shader variable named name, which should be of type float name[count].
    void SetUniformArray1(const std::string &name, float *f, int count);

    
    /// Passes an array of count 2D int arrays to the shader program and stores the result
    /// in the shader variable named name, which should be of type ivec2 name[count].
    void SetUniformArray2(const std::string &name, int *i, int count);

    /// Passes an array of count 2D unsigned int arrays to the shader program and stores the result
    /// in the shader variable named name, which should be of type uivec2 name[count].
    void SetUniformArray2(const std::string &name, unsigned int *ui, int count);

    /// Passes an array of count 2D float arrays to the shader program and stores the result
    /// in the shader variable named name, which should be of type vec2 name[count].
    void SetUniformArray2(const std::string &name, float *f, int count);

    
    /// Passes an array of count 3D int arrays to the shader program and stores the result
    /// in the shader variable named name, which should be of type ivec3 name[count].
    void SetUniformArray3(const std::string &name, int *i, int count);
    
    /// Passes an array of count 3D unsigned int arrays to the shader program and stores the result
    /// in the shader variable named name, which should be of type uivec3 name[count].
    void SetUniformArray3(const std::string &name, unsigned int *ui, int count);

    /// Passes an array of count 3D float arrays to the shader program and stores the result
    /// in the shader variable named name, which should be of type vec3 name[count].
    void SetUniformArray3(const std::string &name, float *f, int count);

    
    /// Passes an array of count 4D int arrays to the shader program and stores the result
    /// in the shader variable named name, which should be of type ivec4 name[count].
    void SetUniformArray4(const std::string &name, int *i, int count);
    
    /// Passes an array of count 4D unsigned int arrays to the shader program and stores the result
    /// in the shader variable named name, which should be of type uivec4 name[count].
    void SetUniformArray4(const std::string &name, unsigned int *ui, int count);
    
    /// Passes an array of count 4D float arrays to the shader program and stores the result
    /// in the shader variable named name, which should be of type vec4 name[count].
    void SetUniformArray4(const std::string &name, float *f, int count);

    
    // Set Textures (Sampler Variables in the Shader)
    
    /// Binds a Texture2D to a sampler2D in the shader program.
    /// This version automatically selects an available texture unit, i.e., one
    /// not already used by this shader program.
    void BindTexture(const std::string &name, const Texture2D &tex);
    
    /// Binds a Texture2D to a sampler2D in the shader program.
    /// This version allows you to specify the texture unit to use.
    void BindTexture(const std::string &name, const Texture2D &tex, int texUnit);
    
    
    /// Call this after rendering geometry to deactivate the shader.
    void StopProgram();

    /// Returns true if the shader program has been successfully compiled and linked.
    bool initialized();
    
private:
    GLuint vertexShader_;
    GLuint fragmentShader_;
    GLuint program_;
    std::map<std::string, int> texBindings_;
};
    
    
} // end namespace

#endif
