/*
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 See corresponding header file for details.
 */

#include "shader_program.h"

#include "opengl_headers.h"

#include <vector>
#include <fstream>


namespace mingfx {


ShaderProgram::ShaderProgram() : vertexShader_(0), fragmentShader_(0), program_(0) {
}

ShaderProgram::~ShaderProgram() {
}
    
bool ShaderProgram::initialized() {
    return (program_ != 0);
}

bool ShaderProgram::AddVertexShaderFromSource(const std::string &vertexSource) {
    // https://www.khronos.org/opengl/wiki/Shader_Compilation
    
    // Create an empty vertex shader handle
    vertexShader_ = glCreateShader(GL_VERTEX_SHADER);
    
    // Send the vertex shader source code to GL
    // Note that std::string's .c_str is NULL character terminated.
    const GLchar *source = (const GLchar *)vertexSource.c_str();
    glShaderSource(vertexShader_, 1, &source, 0);
    
    // Compile the vertex shader
    glCompileShader(vertexShader_);
    
    GLint isCompiled = 0;
    glGetShaderiv(vertexShader_, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(vertexShader_, GL_INFO_LOG_LENGTH, &maxLength);
        
        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(vertexShader_, maxLength, &maxLength, &infoLog[0]);
        
        // We don't need the shader anymore.
        glDeleteShader(vertexShader_);
        
        std::cerr << "ShaderProgram: Error compiling vertex shader program: " << std::endl;
        std::cerr << &infoLog[0] << std::endl;
        std::cerr << vertexSource << std::endl;
        return false;
    }
    return true;
}
    
bool ShaderProgram::AddVertexShaderFromFile(const std::string &file) {
    std::cout << "Loading vertex shader from file: " << file << std::endl;
    std::string source;
    std::string line;
    std::ifstream myfile (file);
    if (myfile.is_open()) {
        while (std::getline(myfile, line)) {
            source += line + "\n";
        }
        myfile.close();
        return AddVertexShaderFromSource(source);
    }
    else {
        std::cerr << "ShaderProgram: Cannot open file " << file << std::endl;
        return false;
    }
}

    
    
bool ShaderProgram::AddFragmentShaderFromSource(const std::string &fragmentSource) {
    // https://www.khronos.org/opengl/wiki/Shader_Compilation
    
    // Create an empty fragment shader handle
    fragmentShader_ = glCreateShader(GL_FRAGMENT_SHADER);
    
    // Send the fragment shader source code to GL
    // Note that std::string's .c_str is NULL character terminated.
    const GLchar *source = (const GLchar *)fragmentSource.c_str();
    glShaderSource(fragmentShader_, 1, &source, 0);
    
    // Compile the fragment shader
    glCompileShader(fragmentShader_);
    
    GLint isCompiled = 0;
    glGetShaderiv(fragmentShader_, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(fragmentShader_, GL_INFO_LOG_LENGTH, &maxLength);
        
        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(fragmentShader_, maxLength, &maxLength, &infoLog[0]);
        
        // We don't need the shader anymore.
        glDeleteShader(fragmentShader_);
        
        std::cerr << "ShaderProgram: Error compiling fragment shader program: " << std::endl;
        std::cerr << &infoLog[0] << std::endl;
        std::cerr << fragmentSource << std::endl;
        return false;
    }
    return true;
}


bool ShaderProgram::AddFragmentShaderFromFile(const std::string &file) {
    std::cout << "Loading fragment shader from file: " << file << std::endl;
    std::string source;
    std::string line;
    std::ifstream myfile (file);
    if (myfile.is_open()) {
        while (std::getline(myfile, line)) {
            source += line + "\n";
        }
        myfile.close();
        return AddFragmentShaderFromSource(source);
    }
    else {
        std::cerr << "ShaderProgram: Cannot open file " << file << std::endl;
        return false;
    }
}

    
bool ShaderProgram::LinkProgram() {
    if (!vertexShader_) {
        std::cerr << "ShaderProgram: Error linking program.  A vertex shader must be added and successfully compiled before the program can be linked." << std::endl;
        return false;
    }
    if (!fragmentShader_) {
        std::cerr << "ShaderProgram: Error linking program.  A fragment shader must be added and successfully compiled before the program can be linked." << std::endl;
        return false;
    }
    
    // Vertex and fragment shaders are successfully compiled.
    // Now time to link them together into a program.
    // Get a program object.
    program_ = glCreateProgram();
    
    // Attach our shaders to our program
    glAttachShader(program_, vertexShader_);
    glAttachShader(program_, fragmentShader_);
    
    // Link our program
    glLinkProgram(program_);
    
    // Note the different functions here: glGetProgram* instead of glGetShader*.
    GLint isLinked = 0;
    glGetProgramiv(program_, GL_LINK_STATUS, (int *)&isLinked);
    if (isLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &maxLength);
        
        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program_, maxLength, &maxLength, &infoLog[0]);
        
        // We don't need the program anymore.
        glDeleteProgram(program_);
        // Don't leak shaders either.
        glDeleteShader(vertexShader_);
        glDeleteShader(fragmentShader_);
        
        std::cerr << "ShaderProgram: Error linking program: " << std::endl;
        std::cerr << &infoLog[0] << std::endl;
        return false;
    }
    
    // Always detach shaders after a successful link.
    glDetachShader(program_, vertexShader_);
    glDetachShader(program_, fragmentShader_);
    
    return true;
}
    

void ShaderProgram::UseProgram() {
    if (!initialized()) {
        std::cerr << "ShaderProgram: Warning cannot UseProgram() until it shaders have been added and linked.  Calling LinkProgram() for you now." << std::endl;
        LinkProgram();
    }
    glUseProgram(program_);
}

    
void ShaderProgram::StopProgram() {
    glUseProgram(0);
}
    
    
// MinGfx math types
void ShaderProgram::SetUniform(const std::string &name, const Point2 &p) {
    UseProgram();
    GLint loc = glGetUniformLocation(program_, name.c_str());
    glUniform2f(loc, p[0], p[1]);
}

void ShaderProgram::SetUniform(const std::string &name, const Vector2 &v) {
    UseProgram();
    GLint loc = glGetUniformLocation(program_, name.c_str());
    glUniform2f(loc, v[0], v[1]);
}

void ShaderProgram::SetUniform(const std::string &name, const Point3 &p) {
    UseProgram();
    GLint loc = glGetUniformLocation(program_, name.c_str());
    glUniform3f(loc, p[0], p[1], p[2]);
}

void ShaderProgram::SetUniform(const std::string &name, const Vector3 &v) {
    UseProgram();
    GLint loc = glGetUniformLocation(program_, name.c_str());
    glUniform3f(loc, v[0], v[1], v[2]);
}

void ShaderProgram::SetUniform(const std::string &name, const Matrix4 &m) {
    UseProgram();
    GLint loc = glGetUniformLocation(program_, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, m.value_ptr());
}

void ShaderProgram::SetUniform(const std::string &name, const Color &c) {
    UseProgram();
    GLint loc = glGetUniformLocation(program_, name.c_str());
    glUniform4f(loc, c[0], c[1], c[2], c[3]);
}
    

// built-in types
void ShaderProgram::SetUniform(const std::string &name, int i) {
    UseProgram();
    GLint loc = glGetUniformLocation(program_, name.c_str());
    glUniform1i(loc, i);
}

void ShaderProgram::SetUniform(const std::string &name, unsigned int ui) {
    UseProgram();
    GLint loc = glGetUniformLocation(program_, name.c_str());
    glUniform1ui(loc, ui);
}

void ShaderProgram::SetUniform(const std::string &name, float f) {
    UseProgram();
    GLint loc = glGetUniformLocation(program_, name.c_str());
    glUniform1f(loc, f);
}



// built-in types - arrays
void ShaderProgram::SetUniformArray1(const std::string &name, int *i, int count) {
    UseProgram();
    GLint loc = glGetUniformLocation(program_, name.c_str());
    glUniform1iv(loc, count, i);
}

void ShaderProgram::SetUniformArray1(const std::string &name, unsigned int *ui, int count) {
    UseProgram();
    GLint loc = glGetUniformLocation(program_, name.c_str());
    glUniform1uiv(loc, count, ui);
}

void ShaderProgram::SetUniformArray1(const std::string &name, float *f, int count) {
    UseProgram();
    GLint loc = glGetUniformLocation(program_, name.c_str());
    glUniform1fv(loc, count, f);
}


void ShaderProgram::SetUniformArray2(const std::string &name, int *i, int count) {
    UseProgram();
    GLint loc = glGetUniformLocation(program_, name.c_str());
    glUniform2iv(loc, count, i);
}

void ShaderProgram::SetUniformArray2(const std::string &name, unsigned int *ui, int count) {
    UseProgram();
    GLint loc = glGetUniformLocation(program_, name.c_str());
    glUniform2uiv(loc, count, ui);
}

void ShaderProgram::SetUniformArray2(const std::string &name, float *f, int count) {
    UseProgram();
    GLint loc = glGetUniformLocation(program_, name.c_str());
    glUniform2fv(loc, count, f);
}


void ShaderProgram::SetUniformArray3(const std::string &name, int *i, int count) {
    UseProgram();
    GLint loc = glGetUniformLocation(program_, name.c_str());
    glUniform3iv(loc, count, i);
}

void ShaderProgram::SetUniformArray3(const std::string &name, unsigned int *ui, int count) {
    UseProgram();
    GLint loc = glGetUniformLocation(program_, name.c_str());
    glUniform3uiv(loc, count, ui);
}

void ShaderProgram::SetUniformArray3(const std::string &name, float *f, int count) {
    UseProgram();
    GLint loc = glGetUniformLocation(program_, name.c_str());
    glUniform3fv(loc, count, f);
}


void ShaderProgram::SetUniformArray4(const std::string &name, int *i, int count) {
    UseProgram();
    GLint loc = glGetUniformLocation(program_, name.c_str());
    glUniform4iv(loc, count, i);
}

void ShaderProgram::SetUniformArray4(const std::string &name, unsigned int *ui, int count) {
    UseProgram();
    GLint loc = glGetUniformLocation(program_, name.c_str());
    glUniform4uiv(loc, count, ui);
}

void ShaderProgram::SetUniformArray4(const std::string &name, float *f, int count) {
    UseProgram();
    GLint loc = glGetUniformLocation(program_, name.c_str());
    glUniform4fv(loc, count, f);
}



void ShaderProgram::BindTexture(const std::string &name, const Texture2D &tex) {
    UseProgram();

    int texUnit = 0;
    
    std::map<std::string,int>::const_iterator it = texBindings_.find(name);
    if (it != texBindings_.end()) {
        // Found: This sampler was already bound to a tex unit, so use the same one
        texUnit = texBindings_[name];
    }
    else {
        // Not found: This sampler was not already bound to a tex unit, so loop through the
        // past/current bindings and pick the next available texUnit
        std::map<std::string,int>::const_iterator it2;
        for (it2 = texBindings_.begin(); it2 != texBindings_.end(); ++it2) {
            if (texUnit <= it2->second) {
                texUnit = it2->second + 1;
            }
        }
        // save it for future reference
        texBindings_[name] = texUnit;
    }
    
    // associate the named shader program sampler variable with the selected texture unit
    GLint loc = glGetUniformLocation(program_, name.c_str());
    glUniform1i(loc, texUnit);
    // bind the opengl texture handle to the same texture unit
    glActiveTexture(GL_TEXTURE0 + texUnit);
    glBindTexture(GL_TEXTURE_2D, tex.opengl_id());
}

void ShaderProgram::BindTexture(const std::string &name, const Texture2D &tex, int texUnit) {
    UseProgram();

    texBindings_[name] = texUnit;
    
    // associate the named shader program sampler variable with the selected texture unit
    GLint loc = glGetUniformLocation(program_, name.c_str());
    glUniform1i(loc, texUnit);
    // bind the opengl texture handle to the same texture unit
    glActiveTexture(GL_TEXTURE0 + texUnit);
    glBindTexture(GL_TEXTURE_2D, tex.opengl_id());
}


    
} // end namespace


