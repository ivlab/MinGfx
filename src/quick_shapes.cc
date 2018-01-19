/**
 This file is part of the MinVR Open Source Project, which is developed and
 maintained collaboratively by the University of Minnesota and Brown University.
 
 Copyright (c) 2016-2017 Regents of the University of Minnesota and Brown University.
 This software is distributed under the BSD-3 Clause license, which can be found
 at: http://github.minvr.org/blob/master/LICENSE.txt
 
 Original Author(s) of this File:
	Dan Keefe, 2017, University of Minnesota
    Dan Orban, 2017, University of Minnesota (ShaderUtils routines)
	
 Author(s) of Significant Updates/Modifications to the File:
	...
 */


#include "quick_shapes.h"
#include "matrix4.h"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>


namespace MinGfx {

#define PI 3.14159265359
#define TWOPI 6.28318530718



/**********  SHADER UTILS  **********/


// Helper class for rendering with shaders
class ShaderUtils {
public:

    static void init();
    
    /** Call this before rendering some geometry to "turn on" a simple Phong shader.
	    (Phong = per-fragment lighting).  Vertices should be in vertex attrib 0,
        and normals in vertex attrib 1.
     */
    static void startSimplePhongShader(const float *modelMatrix,
                                       const float *viewMatrix,
                                       const float *projectionMatrix,
                                       const float *lightPosition,
                                       const float *lightIntensityAmbient,
                                       const float *lightIntensityDiffuse,
                                       const float *lightIntensitySpecular,
                                       const float *matReflectanceAmbient,
                                       const float *matReflectanceDiffuse,
                                       const float *matReflectanceSpecular,
                                       const float matReflectanceShininess);

    /** Call this before rendering some geometry to "turn on" a simple textured
        geometry shader.  Vertices should be in vertex attrib 0, normals in 
        vertex attrib 1, and texcoords in vertex attrib 2.  TextureID
        must already be bound in OpenGL.
     */
    static void startSimpleTextureShader(const float *modelMatrix,
                                         const float *viewMatrix,
                                         const float *projectionMatrix,
                                         const float *lightPosition,
                                         const float *lightIntensityAmbient,
                                         const float *lightIntensityDiffuse,
                                         const float *lightIntensitySpecular,
                                         const float *matReflectanceAmbient,
                                         const float *matReflectanceDiffuse,
                                         const float *matReflectanceSpecular,
                                         const float matReflectanceShininess,
                                         const int textureID);

    
    /** Call this after rendering some geometry to "turn off" the current shader program.
	    Note, this is the same call regardless of which of the simple shaders listed above
	    is being used. */
    static void stopSimpleShader();
    
    /** Frees memory for all simple shaders that have been used so far. Call this when your
	    program is shutting down. */
    static void deleteSimpleShaders();
    
    
    static GLuint compileShader(const std::string& shaderText, GLuint shaderType);
    
    static void linkShaderProgram(GLuint shaderProgram);
    
private:
    
    static GLuint s_phongVS, s_phongFS, s_phongProgram;
    static GLuint s_texVS, s_texFS, s_texProgram;
    
};


GLuint ShaderUtils::s_phongVS = 0;
GLuint ShaderUtils::s_phongFS = 0;
GLuint ShaderUtils::s_phongProgram = 0;

GLuint ShaderUtils::s_texVS = 0;
GLuint ShaderUtils::s_texFS = 0;
GLuint ShaderUtils::s_texProgram = 0;

GLuint ShaderUtils::compileShader(const std::string& shaderText, GLuint shaderType) {
    const char* source = shaderText.c_str();
    int length = shaderText.size();
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, &length);
    glCompileShader(shader);
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE) {
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(length);
        glGetShaderInfoLog(shader, length, &length, &log[0]);
        std::cerr << "ShaderUtils:  Error compiling shader program: " << std::endl;
        std::cerr << &log[0] << std::endl;
        std::cerr << shaderText << std::endl;
    }
    return shader;
}


void ShaderUtils::linkShaderProgram(GLuint shaderProgram) {
    glLinkProgram(shaderProgram);
    GLint status;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    if(status == GL_FALSE) {
        GLint length;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(length);
        glGetProgramInfoLog(shaderProgram, length, &length, &log[0]);
        std::cerr << "ShaderUtils: Error linking program: " << std::endl;
        std::cerr << &log[0] << std::endl;
    }
}


void ShaderUtils::init() {
    
    // init Phong Shader:
    std::string vertexShader =
    "#version 330 \n"
    "layout(location = 0) in vec3 position; \n"
    "layout(location = 1) in vec3 normal; \n"
    "\n"
    "uniform mat4 ModelMatrix; \n"
    "uniform mat4 ViewMatrix; \n"
    "uniform mat4 ProjectionMatrix; \n"
    "uniform mat4 NormalMatrix; \n"
    "\n"
    "out vec3 N; \n"
    "out vec3 v; \n"
    "\n"
    "void main() { \n"
    "   v = (ViewMatrix * ModelMatrix * vec4(position, 1)).xyz; \n"
    "   N = normalize((NormalMatrix * vec4(normal, 0)).xyz); \n"
    "   gl_Position	= ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(position, 1); \n"
    "} \n";
    s_phongVS = compileShader(vertexShader, GL_VERTEX_SHADER);
    
    std::string fragmentShader =
    "#version 330 \n"
    "\n"
    "in vec3 N; \n"
    "in vec3 v; \n"
    "\n"
    "out vec4 fragColor; \n"
    "\n"
    "uniform vec3 LightPosition; \n"
    "uniform vec3 LightIntensityAmbient; \n"
    "uniform vec3 LightIntensityDiffuse; \n"
    "uniform vec3 LightIntensitySpecular; \n"
    "\n"
    "uniform vec3 MatReflectanceAmbient; \n"
    "uniform vec3 MatReflectanceDiffuse; \n"
    "uniform vec3 MatReflectanceSpecular; \n"
    "uniform float MatReflectanceShininess; \n"
    "\n"
    "void main() { \n"
    "   vec3 L = normalize(LightPosition - v); \n"
    "   vec3 V = normalize(-v); // eye is at (0,0,0) \n"
    "   vec3 R = normalize(-reflect(L,N));"
    "\n"
    "   vec3 Ia = MatReflectanceAmbient * LightIntensityAmbient; \n"
    "\n"
    "   vec3 Id = clamp(MatReflectanceDiffuse * LightIntensityDiffuse * max(dot(N, L), 0.0), 0.0, 1.0); \n"
    "\n"
    "   vec3 Is = MatReflectanceSpecular * LightIntensitySpecular * pow(max(dot(R, V), 0.0), MatReflectanceShininess); \n"
    "   Is = clamp(Is, 0.0, 1.0);"
    "\n"
    "   fragColor.rgb = Ia + Id + Is; \n"
    "   fragColor.a = 1.0; \n"
    "} \n";
    s_phongFS = compileShader(fragmentShader, GL_FRAGMENT_SHADER);
    
    s_phongProgram = glCreateProgram();
    glAttachShader(s_phongProgram, s_phongVS);
    glAttachShader(s_phongProgram, s_phongFS);
    
    glBindAttribLocation(s_phongProgram, 0, "position");
    glBindAttribLocation(s_phongProgram, 1, "normal");
    
    linkShaderProgram(s_phongProgram);
    
    
    
    // init Texture Shader:
    vertexShader =
    "#version 330 \n"
    "layout(location = 0) in vec3 position; \n"
    "layout(location = 1) in vec3 normal; \n"
    "layout(location = 2) in vec2 texcoord;\n"
    "\n"
    "uniform mat4 ModelMatrix; \n"
    "uniform mat4 ViewMatrix; \n"
    "uniform mat4 ProjectionMatrix; \n"
    "uniform mat4 NormalMatrix; \n"
    "\n"
    "out vec3 N; \n"
    "out vec3 v; \n"
    "out vec2 uv; \n"
    "\n"
    "void main() { \n"
    "   v = (ViewMatrix * ModelMatrix * vec4(position, 1)).xyz; \n"
    "   N = normalize((NormalMatrix * vec4(normal, 0)).xyz); \n"
    "   uv = texcoord.xy; \n"
    "   gl_Position	= ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(position, 1); \n"
    "} \n";
    s_texVS = compileShader(vertexShader, GL_VERTEX_SHADER);
    
    fragmentShader =
    "#version 330 \n"
    "\n"
    "in vec3 N; \n"
    "in vec3 v; \n"
    "in vec2 uv; \n"
    "\n"
    "out vec4 fragColor; \n"
    "\n"
    "uniform vec3 LightPosition; \n"
    "uniform vec3 LightIntensityAmbient; \n"
    "uniform vec3 LightIntensityDiffuse; \n"
    "uniform vec3 LightIntensitySpecular; \n"
    "\n"
    "uniform vec3 MatReflectanceAmbient; \n"
    "uniform vec3 MatReflectanceDiffuse; \n"
    "uniform vec3 MatReflectanceSpecular; \n"
    "uniform float MatReflectanceShininess; \n"
    "\n"
    "uniform sampler2D SurfaceTexture; \n"
    "\n"
    "void main() { \n"
    "   vec3 L = normalize(LightPosition - v); \n"
    "   vec3 V = normalize(-v); // eye is at (0,0,0) \n"
    "   vec3 R = normalize(-reflect(L,N));"
    "\n"
    "   vec3 Ia = MatReflectanceAmbient * LightIntensityAmbient; \n"
    "\n"
    "   vec3 Id = clamp(MatReflectanceDiffuse * LightIntensityDiffuse * max(dot(N, L), 0.0), 0.0, 1.0); \n"
    "\n"
    "   vec3 Is = MatReflectanceSpecular * LightIntensitySpecular * pow(max(dot(R, V), 0.0), MatReflectanceShininess); \n"
    "   Is = clamp(Is, 0.0, 1.0);"
    "\n"
    "   fragColor = texture(SurfaceTexture, uv); \n"
    "   fragColor.rgb *= Ia + Id + Is; \n"
    "} \n";
    s_texFS = compileShader(fragmentShader, GL_FRAGMENT_SHADER);
    
    s_texProgram = glCreateProgram();
    glAttachShader(s_texProgram, s_texVS);
    glAttachShader(s_texProgram, s_texFS);
    
    glBindAttribLocation(s_texProgram, 0, "position");
    glBindAttribLocation(s_texProgram, 1, "normal");
    glBindAttribLocation(s_texProgram, 2, "texcoord");
    
    linkShaderProgram(s_texProgram);
}


void ShaderUtils::startSimplePhongShader(const float *modelMatrix,
                                         const float *viewMatrix,
                                         const float *projectionMatrix,
                                         const float *lightPosition,
                                         const float *lightIntensityAmbient,
                                         const float *lightIntensityDiffuse,
                                         const float *lightIntensitySpecular,
                                         const float *matReflectanceAmbient,
                                         const float *matReflectanceDiffuse,
                                         const float *matReflectanceSpecular,
                                         const float matReflectanceShininess) {
    // Initialize if first time through
    if (s_phongProgram == 0) {
        init();
    }
    
    // This is the one place in this QuickShapes / ShaderUtils code with a
    // dependency on an external project.  A matrix inverse routine is needed
    // for calculating the normal matrix.
    Matrix4 M(modelMatrix);
    Matrix4 V(viewMatrix);
    Matrix4 N = (M*V).inverse().transpose();
    const float *normalMatrix = N.value_ptr();
    
    // Alternatively, to implement with the glm library, use this:
    //glm::mat4 M = glm::make_mat4(modelMatrix);
    //glm::mat4 V = glm::make_mat4(viewMatrix);
    //glm::mat4 N = glm::transpose(glm::inverse(M*V));
    //float *normalMatrix = glm::value_ptr(N);
    
    glUseProgram(s_phongProgram);
    GLint loc = glGetUniformLocation(s_phongProgram, "ProjectionMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, projectionMatrix);
    loc = glGetUniformLocation(s_phongProgram, "ViewMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, viewMatrix);
    loc = glGetUniformLocation(s_phongProgram, "ModelMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, modelMatrix);
    loc = glGetUniformLocation(s_phongProgram, "NormalMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, normalMatrix);
    loc = glGetUniformLocation(s_phongProgram, "LightPosition");
    glUniform3fv(loc, 1, lightPosition);
    loc = glGetUniformLocation(s_phongProgram, "LightIntensityAmbient");
    glUniform3fv(loc, 1, lightIntensityAmbient);
    loc = glGetUniformLocation(s_phongProgram, "LightIntensityDiffuse");
    glUniform3fv(loc, 1, lightIntensityDiffuse);
    loc = glGetUniformLocation(s_phongProgram, "LightIntensitySpecular");
    glUniform3fv(loc, 1, lightIntensitySpecular);
    loc = glGetUniformLocation(s_phongProgram, "MatReflectanceAmbient");
    glUniform3fv(loc, 1, matReflectanceAmbient);
    loc = glGetUniformLocation(s_phongProgram, "MatReflectanceDiffuse");
    glUniform3fv(loc, 1, matReflectanceDiffuse);
    loc = glGetUniformLocation(s_phongProgram, "MatReflectanceSpecular");
    glUniform3fv(loc, 1, matReflectanceSpecular);
    loc = glGetUniformLocation(s_phongProgram, "MatReflectanceShininess");
    glUniform1f(loc, matReflectanceShininess);
}

void ShaderUtils::startSimpleTextureShader(const float *modelMatrix,
                                           const float *viewMatrix,
                                           const float *projectionMatrix,
                                           const float *lightPosition,
                                           const float *lightIntensityAmbient,
                                           const float *lightIntensityDiffuse,
                                           const float *lightIntensitySpecular,
                                           const float *matReflectanceAmbient,
                                           const float *matReflectanceDiffuse,
                                           const float *matReflectanceSpecular,
                                           const float matReflectanceShininess,
                                           const int textureID) {
    // Initialize if first time through
    if (s_texProgram == 0) {
        init();
    }
    
    // This is the one place in this QuickShapes / ShaderUtils code with a
    // dependency on an external project.  A matrix inverse routine is needed
    // for calculating the normal matrix.
    Matrix4 M(modelMatrix);
    Matrix4 V(viewMatrix);
    Matrix4 N = (M*V).inverse().transpose();
    const float *normalMatrix = N.value_ptr();
    
    // Alternatively, to implement with the glm library, use this:
    //glm::mat4 M = glm::make_mat4(modelMatrix);
    //glm::mat4 V = glm::make_mat4(viewMatrix);
    //glm::mat4 N = glm::transpose(glm::inverse(M*V));
    //float *normalMatrix = glm::value_ptr(N);
    
    glUseProgram(s_texProgram);
    GLint loc = glGetUniformLocation(s_texProgram, "ProjectionMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, projectionMatrix);
    loc = glGetUniformLocation(s_texProgram, "ViewMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, viewMatrix);
    loc = glGetUniformLocation(s_texProgram, "ModelMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, modelMatrix);
    loc = glGetUniformLocation(s_texProgram, "NormalMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, normalMatrix);
    loc = glGetUniformLocation(s_texProgram, "LightPosition");
    glUniform3fv(loc, 1, lightPosition);
    loc = glGetUniformLocation(s_texProgram, "LightIntensityAmbient");
    glUniform3fv(loc, 1, lightIntensityAmbient);
    loc = glGetUniformLocation(s_texProgram, "LightIntensityDiffuse");
    glUniform3fv(loc, 1, lightIntensityDiffuse);
    loc = glGetUniformLocation(s_texProgram, "LightIntensitySpecular");
    glUniform3fv(loc, 1, lightIntensitySpecular);
    loc = glGetUniformLocation(s_texProgram, "MatReflectanceAmbient");
    glUniform3fv(loc, 1, matReflectanceAmbient);
    loc = glGetUniformLocation(s_texProgram, "MatReflectanceDiffuse");
    glUniform3fv(loc, 1, matReflectanceDiffuse);
    loc = glGetUniformLocation(s_texProgram, "MatReflectanceSpecular");
    glUniform3fv(loc, 1, matReflectanceSpecular);
    loc = glGetUniformLocation(s_texProgram, "MatReflectanceShininess");
    glUniform1f(loc, matReflectanceShininess);
    
    loc = glGetUniformLocation(s_texProgram, "SurfaceTexture");
    glUniform1i(loc, 0);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
}

    
void ShaderUtils::stopSimpleShader() {
    glUseProgram(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}


void ShaderUtils::deleteSimpleShaders() {
    if (s_phongProgram != 0) {
        glDetachShader(s_phongProgram, s_phongVS);
        glDetachShader(s_phongProgram, s_phongFS);
        glDeleteShader(s_phongVS);
        glDeleteShader(s_phongFS);
        glDeleteProgram(s_phongProgram);
        s_phongProgram = 0;
        s_phongVS = 0;
        s_phongFS = 0;
    }
    
    if (s_texProgram != 0) {
        glDetachShader(s_texProgram, s_texVS);
        glDetachShader(s_texProgram, s_texFS);
        glDeleteShader(s_texVS);
        glDeleteShader(s_texFS);
        glDeleteProgram(s_texProgram);
        s_texProgram = 0;
        s_texVS = 0;
        s_texFS = 0;
    }
}

/**********  END SHADER UTILS  **********/



// Helper datastructure for building shapes algorithmically
class Vertex {
public:
    Vertex(GLfloat xx, GLfloat yy, GLfloat zz, GLfloat nnx, GLfloat nny, GLfloat nnz) :
    x(xx), y(yy), z(zz), nx(nnx), ny(nny), nz(nnz) {}
    
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat nx;
    GLfloat ny;
    GLfloat nz;
};

    
QuickShapes::QuickShapes() {
    
    s_lightPos[0] = 10.0;
    s_lightPos[1] = 10.0;
    s_lightPos[2] = 10.0;
    
    s_lightAmb[0] = 0.2;
    s_lightAmb[1] = 0.2;
    s_lightAmb[2] = 0.2;
    
    s_lightDiff[0] = 0.7;
    s_lightDiff[1] = 0.7;
    s_lightDiff[2] = 0.7;
    
    s_lightSpec[0] = 0.6;
    s_lightSpec[1] = 0.6;
    s_lightSpec[2] = 0.6;
    
    s_matSpec[0] = 0.6;
    s_matSpec[1] = 0.6;
    s_matSpec[2] = 0.6;
    
    s_matShin = 10.0;

    s_cubeVAO = 0;
    s_cubeVBO = 0;

    s_squareVAO = 0;
    s_squareVBO = 0;
    
    s_cylVAO = 0;
    s_cylVBO = 0;
    s_cylNVerts = 0;

    s_coneVAO = 0;
    s_coneVBO = 0;
    s_coneNVerts = 0;
    
    s_sphVAO = 0;
    s_sphVBO = 0;
    s_sphNVerts = 0;

    s_brushVAO = 0;
    s_brushVBO = 0;
}


QuickShapes::~QuickShapes() {
    freeGPUMemory();
}




// ------------  CUBE  ------------


void QuickShapes::initCube() {
	GLfloat vertices[]  = {
		 1.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 1.0f,  -1.0f,-1.0f, 1.0f,      // v0-v1-v2 (front)
		-1.0f,-1.0f, 1.0f,   1.0f,-1.0f, 1.0f,   1.0f, 1.0f, 1.0f,      // v2-v3-v0

		 1.0f, 1.0f, 1.0f,   1.0f,-1.0f, 1.0f,   1.0f,-1.0f,-1.0f,      // v0-v3-v4 (right)
		 1.0f,-1.0f,-1.0f,   1.0f, 1.0f,-1.0f,   1.0f, 1.0f, 1.0f,      // v4-v5-v0

		 1.0f, 1.0f, 1.0f,   1.0f, 1.0f,-1.0f,  -1.0f, 1.0f,-1.0f,      // v0-v5-v6 (top)
		-1.0f, 1.0f,-1.0f,  -1.0f, 1.0f, 1.0f,   1.0f, 1.0f, 1.0f,      // v6-v1-v0

		-1.0f, 1.0f, 1.0f,  -1.0f, 1.0f,-1.0f,  -1.0f,-1.0f,-1.0f,      // v1-v6-v7 (left)
		-1.0f,-1.0f,-1.0f,  -1.0f,-1.0f, 1.0f,  -1.0f, 1.0f, 1.0f,      // v7-v2-v1.0

		-1.0f,-1.0f,-1.0f,   1.0f,-1.0f,-1.0f,   1.0f,-1.0f, 1.0f,      // v7-v4-v3 (bottom)
		 1.0f,-1.0f, 1.0f,  -1.0f,-1.0f, 1.0f,  -1.0f,-1.0f,-1.0f,      // v3-v2-v7

		 1.0f,-1.0f,-1.0f,  -1.0f,-1.0f,-1.0f,  -1.0f, 1.0f,-1.0f,      // v4-v7-v6 (back)
		-1.0f, 1.0f,-1.0f,   1.0f, 1.0f,-1.0f,   1.0f,-1.0f,-1.0f       // v6-v5-v4
	};

	GLfloat normals[]   = { 
		 0, 0, 1,   0, 0, 1,   0, 0, 1,      // v0-v1-v2 (front)
		 0, 0, 1,   0, 0, 1,   0, 0, 1,      // v2-v3-v0

		 1, 0, 0,   1, 0, 0,   1, 0, 0,      // v0-v3-v4 (right)
		 1, 0, 0,   1, 0, 0,   1, 0, 0,      // v4-v5-v0

		 0, 1, 0,   0, 1, 0,   0, 1, 0,      // v0-v5-v6 (top)
		 0, 1, 0,   0, 1, 0,   0, 1, 0,      // v6-v1-v0

		-1, 0, 0,  -1, 0, 0,  -1, 0, 0,     // v1-v6-v7 (left)
		-1, 0, 0,  -1, 0, 0,  -1, 0, 0,     // v7-v2-v1

		 0,-1, 0,   0,-1, 0,   0,-1, 0,      // v7-v4-v3 (bottom)
		 0,-1, 0,   0,-1, 0,   0,-1, 0,      // v3-v2-v7

		 0, 0,-1,   0, 0,-1,   0, 0,-1,      // v4-v7-v6 (back)
		 0, 0,-1,   0, 0,-1,   0, 0,-1       // v6-v5-v4
	};

	glGenBuffers(1, &s_cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, s_cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)+sizeof(normals), 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(normals), normals);

	glGenVertexArrays(1, &s_cubeVAO);
	glBindVertexArray(s_cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, s_cubeVBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (char*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (char*)0 + sizeof(vertices));
    glBindVertexArray(0);
}


void QuickShapes::drawCube(const Matrix4 &modelMatrix, const Matrix4 &viewMatrix,
                           const Matrix4 &projectionMatrix, const Color &color) {
	if (s_cubeVAO == 0) {
		initCube();
	}

	ShaderUtils::startSimplePhongShader(modelMatrix.value_ptr(),
                                        viewMatrix.value_ptr(),
                                        projectionMatrix.value_ptr(),
										s_lightPos, s_lightAmb, s_lightDiff, s_lightSpec,
										color.value_ptr(), color.value_ptr(), s_matSpec, s_matShin);

	glBindVertexArray(s_cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	ShaderUtils::stopSimpleShader();
}




    
    
// ------------  SQUARE  ------------


void QuickShapes::initSquare() {
    GLfloat vertices[] = {
        1.0f, 0.0f, 1.0f,   1.0f, 0.0f,-1.0f,  -1.0f, 0.0f,-1.0f,      // v0-v5-v6 (top)
        -1.0f, 0.0f,-1.0f,  -1.0f, 0.0f, 1.0f,   1.0f, 0.0f, 1.0f      // v6-v1-v0
    };
    
    GLfloat normals[] = {
        0, 1, 0,   0, 1, 0,   0, 1, 0,
        0, 1, 0,   0, 1, 0,   0, 1, 0
    };
    
    GLfloat texcoords[] = {
        1.0f, 1.0f,   1.0f, 0.0f,   0.0f, 0.0f,
        0.0f, 0.0f,   0.0f, 1.0f,   1.0f, 1.0f
    };
    
    glGenBuffers(1, &s_squareVBO);
    glBindBuffer(GL_ARRAY_BUFFER, s_squareVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)+sizeof(normals)+sizeof(texcoords), 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(normals), normals);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices)+sizeof(normals), sizeof(texcoords), texcoords);
    
    glGenVertexArrays(1, &s_squareVAO);
    glBindVertexArray(s_squareVAO);
    glBindBuffer(GL_ARRAY_BUFFER, s_squareVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (char*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (char*)0 + sizeof(vertices));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (char*)0 + sizeof(vertices) + sizeof(normals));
    glBindVertexArray(0);
}


void QuickShapes::drawSquare(const Matrix4 &modelMatrix, const Matrix4 &viewMatrix,
                             const Matrix4 &projectionMatrix, const Color &color) {
    if (s_squareVAO == 0) {
        initSquare();
    }
    
    ShaderUtils::startSimplePhongShader(modelMatrix.value_ptr(),
                                        viewMatrix.value_ptr(),
                                        projectionMatrix.value_ptr(),
                                        s_lightPos, s_lightAmb, s_lightDiff, s_lightSpec,
                                        color.value_ptr(), color.value_ptr(), s_matSpec, s_matShin);
    
    glBindVertexArray(s_squareVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    
    ShaderUtils::stopSimpleShader();
}

void QuickShapes::drawTexturedSquare(const Matrix4 &modelMatrix, const Matrix4 &viewMatrix,
                                     const Matrix4 &projectionMatrix, const Color &color,
                                     const int textureID) {
    
    ShaderUtils::startSimpleTextureShader(modelMatrix.value_ptr(),
                                          viewMatrix.value_ptr(),
                                          projectionMatrix.value_ptr(),
                                          s_lightPos, s_lightAmb, s_lightDiff, s_lightSpec,
                                          color.value_ptr(), color.value_ptr(), s_matSpec, s_matShin, textureID);
    
    glBindVertexArray(s_squareVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    
    ShaderUtils::stopSimpleShader();
}
    
    
// ------------  CYLINDER  ------------


void QuickShapes::initCyl() {
    
    std::vector<Vertex> verts;
    
    Vertex top(0,1,0, 0,1,0);
    Vertex bot(0,-1,0, 0,-1,0);
    
    const int nslices = 20;
    for (int s=1; s<nslices+1; s++) {
        GLfloat xlast = std::cos(-TWOPI * (float)(s-1)/(float)nslices);
        GLfloat zlast = std::sin(-TWOPI * (float)(s-1)/(float)nslices);
        GLfloat xnew = std::cos(-TWOPI * (float)(s)/(float)nslices);
        GLfloat znew = std::sin(-TWOPI * (float)(s)/(float)nslices);
        
        // one triangle on the top
        verts.push_back(top);
        verts.push_back(Vertex(xlast,1,zlast, 0,1,0));
        verts.push_back(Vertex(xnew,1,znew, 0,1,0));

        // two triangles to create a rect on the side
        verts.push_back(Vertex(xlast,1,zlast, xlast,0,zlast));
        verts.push_back(Vertex(xlast,-1,zlast, xlast,0,zlast));
        verts.push_back(Vertex(xnew,1,znew, xnew,0,znew));
        
        verts.push_back(Vertex(xnew,-1,znew, xnew,0,znew));
        verts.push_back(Vertex(xnew,1,znew, xnew,0,znew));
        verts.push_back(Vertex(xlast,-1,zlast, xlast,0,zlast));
        
        // one triangle on the bottom
        verts.push_back(bot);
        verts.push_back(Vertex(xnew,-1,znew, 0,-1,0));
        verts.push_back(Vertex(xlast,-1,zlast, 0,-1,0));
    }
    
    GLfloat *vertices = new GLfloat[3*verts.size()];
    GLfloat *normals  = new GLfloat[3*verts.size()];
    
    int index = 0;
    for (int i=0; i<verts.size(); i++) {
        vertices[index]   = verts[i].x;
        vertices[index+1] = verts[i].y;
        vertices[index+2] = verts[i].z;
        normals[index]    = verts[i].nx;
        normals[index+1]  = verts[i].ny;
        normals[index+2]  = verts[i].nz;
        index += 3;
    }
  
    glGenBuffers(1, &s_cylVBO);
    glBindBuffer(GL_ARRAY_BUFFER, s_cylVBO);
    glBufferData(GL_ARRAY_BUFFER, 2*3*verts.size()*sizeof(GLfloat), 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 3*verts.size()*sizeof(GLfloat), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 3*verts.size()*sizeof(GLfloat), 3*verts.size()*sizeof(GLfloat), normals);
    
    glGenVertexArrays(1, &s_cylVAO);
    glBindVertexArray(s_cylVAO);
    glBindBuffer(GL_ARRAY_BUFFER, s_cylVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (char*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (char*)0 + 3*verts.size()*sizeof(GLfloat));
    glBindVertexArray(0);
    
    s_cylNVerts = verts.size();
}


void QuickShapes::drawCylinder(const Matrix4 &modelMatrix, const Matrix4 &viewMatrix,
                               const Matrix4 &projectionMatrix, const Color &color) {
    if (s_cylVAO == 0) {
        initCyl();
    }
    
    ShaderUtils::startSimplePhongShader(modelMatrix.value_ptr(),
                                        viewMatrix.value_ptr(),
                                        projectionMatrix.value_ptr(),
                                        s_lightPos, s_lightAmb, s_lightDiff, s_lightSpec,
                                        color.value_ptr(), color.value_ptr(), s_matSpec, s_matShin);
    
    glBindVertexArray(s_cylVAO);
    glDrawArrays(GL_TRIANGLES, 0, s_cylNVerts);
    glBindVertexArray(0);
    
    ShaderUtils::stopSimpleShader();
}






// ------------  CONE  ------------


void QuickShapes::initCone() {
    
    std::vector<Vertex> verts;
    
    Vertex top(0,1,0, 0,1,0);
    Vertex bot(0,-1,0, 0,-1,0);
    
    const int nslices = 20;
    for (int s=1; s<nslices+1; s++) {
        GLfloat xlast = std::cos(-TWOPI * (float)(s-1)/(float)nslices);
        GLfloat zlast = std::sin(-TWOPI * (float)(s-1)/(float)nslices);
        GLfloat xnew = std::cos(-TWOPI * (float)(s)/(float)nslices);
        GLfloat znew = std::sin(-TWOPI * (float)(s)/(float)nslices);
        
        // one triangle on the side
        // normals are a bit more complex than for other shapes...
        Vector3 nlast = Vector3(xlast, 2, zlast).normalize();
        Vector3 nnew = Vector3(xnew, 2, znew).normalize();
        Vector3 ntop = 0.5*(nlast + nnew);
        
        verts.push_back(Vertex(top.x, top.y, top.z, ntop[0], ntop[1], ntop[2]));
        verts.push_back(Vertex(xlast,-1,zlast, nlast[0], nlast[1], nlast[2]));
        verts.push_back(Vertex(xnew,-1,znew, nnew[0], nnew[1], nnew[2]));
        
        // one triangle on the bottom
        verts.push_back(bot);
        verts.push_back(Vertex(xnew,-1,znew, 0,-1,0));
        verts.push_back(Vertex(xlast,-1,zlast, 0,-1,0));
    }
    
    GLfloat *vertices = new GLfloat[3*verts.size()];
    GLfloat *normals  = new GLfloat[3*verts.size()];
    
    int index = 0;
    for (int i=0; i<verts.size(); i++) {
        vertices[index]   = verts[i].x;
        vertices[index+1] = verts[i].y;
        vertices[index+2] = verts[i].z;
        normals[index]    = verts[i].nx;
        normals[index+1]  = verts[i].ny;
        normals[index+2]  = verts[i].nz;
        index += 3;
    }
    
    glGenBuffers(1, &s_coneVBO);
    glBindBuffer(GL_ARRAY_BUFFER, s_coneVBO);
    glBufferData(GL_ARRAY_BUFFER, 2*3*verts.size()*sizeof(GLfloat), 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 3*verts.size()*sizeof(GLfloat), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 3*verts.size()*sizeof(GLfloat), 3*verts.size()*sizeof(GLfloat), normals);
    
    glGenVertexArrays(1, &s_coneVAO);
    glBindVertexArray(s_coneVAO);
    glBindBuffer(GL_ARRAY_BUFFER, s_coneVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (char*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (char*)0 + 3*verts.size()*sizeof(GLfloat));
    glBindVertexArray(0);
    
    s_coneNVerts = verts.size();
}


void QuickShapes::drawCone(const Matrix4 &modelMatrix, const Matrix4 &viewMatrix,
                           const Matrix4 &projectionMatrix, const Color &color) {
    if (s_coneVAO == 0) {
        initCone();
    }
    
    ShaderUtils::startSimplePhongShader(modelMatrix.value_ptr(),
                                        viewMatrix.value_ptr(),
                                        projectionMatrix.value_ptr(),
                                        s_lightPos, s_lightAmb, s_lightDiff, s_lightSpec,
                                        color.value_ptr(), color.value_ptr(), s_matSpec, s_matShin);
    
    glBindVertexArray(s_coneVAO);
    glDrawArrays(GL_TRIANGLES, 0, s_coneNVerts);
    glBindVertexArray(0);
    
    ShaderUtils::stopSimpleShader();
}



    
    
    
// ------------  SPHERE  ------------


void QuickShapes::initSph() {
    
    std::vector<Vertex> verts;
    
    Vertex top(0,1,0, 0,1,0);
    Vertex bot(0,-1,0, 0,-1,0);
    
    const int nslices = 40;
    const int nstacks = 40;
    for (int s=1; s<nslices+1; s++) {
        GLfloat xlast = std::cos(-TWOPI * (float)(s-1)/(float)nslices);
        GLfloat zlast = std::sin(-TWOPI * (float)(s-1)/(float)nslices);
        GLfloat xnew = std::cos(-TWOPI * (float)(s)/(float)nslices);
        GLfloat znew = std::sin(-TWOPI * (float)(s)/(float)nslices);
        
        float stackstep = PI/(float)nstacks;
        
        // one triangle on the top
        verts.push_back(top);
        verts.push_back(Vertex(std::sin(stackstep)*xlast,std::cos(stackstep),std::sin(stackstep)*zlast,
                               std::sin(stackstep)*xlast,std::cos(stackstep),std::sin(stackstep)*zlast));
        verts.push_back(Vertex(std::sin(stackstep)*xnew,std::cos(stackstep),std::sin(stackstep)*znew,
                               std::sin(stackstep)*xnew,std::cos(stackstep),std::sin(stackstep)*znew));
        
        for (int t=2; t<nstacks; t++) {
            GLfloat ylast = std::cos(PI*(float)(t-1)/(float)nstacks);
            GLfloat ynew = std::cos(PI*(float)(t)/(float)nstacks);
            
            GLfloat rlast = std::sin(PI * (float)(t-1)/(float)nstacks);
            GLfloat rnew = std::sin(PI * (float)(t)/(float)nstacks);
            
            // two triangles to create a rect on the side
            verts.push_back(Vertex(rlast*xlast,ylast,rlast*zlast, rlast*xlast,ylast,rlast*zlast));
            verts.push_back(Vertex(rnew*xlast,ynew,rnew*zlast, rnew*xlast,ynew,rnew*zlast));
            verts.push_back(Vertex(rnew*xnew,ynew,rnew*znew, rnew*xnew,ynew,rnew*znew));
            
            verts.push_back(Vertex(rnew*xnew,ynew,rnew*znew, rnew*xnew,ynew,rnew*znew));
            verts.push_back(Vertex(rlast*xnew,ylast,rlast*znew, rlast*xnew,ylast,rlast*znew));
            verts.push_back(Vertex(rlast*xlast,ylast,rlast*zlast, rlast*xlast,ylast,rlast*zlast));
        }
        
        // one triangle on the bottom
        verts.push_back(bot);
        verts.push_back(Vertex(std::sin(stackstep)*xlast,std::cos(PI-stackstep),std::sin(stackstep)*zlast,
                               std::sin(stackstep)*xlast,std::cos(PI-stackstep),std::sin(stackstep)*zlast));
        verts.push_back(Vertex(std::sin(stackstep)*xnew,std::cos(PI-stackstep),std::sin(stackstep)*znew,
                               std::sin(stackstep)*xnew,std::cos(PI-stackstep),std::sin(stackstep)*znew));
    }
    
    GLfloat *vertices = new GLfloat[3*verts.size()];
    GLfloat *normals  = new GLfloat[3*verts.size()];
    
    int index = 0;
    for (int i=0; i<verts.size(); i++) {
        vertices[index]   = verts[i].x;
        vertices[index+1] = verts[i].y;
        vertices[index+2] = verts[i].z;
        normals[index]    = verts[i].nx;
        normals[index+1]  = verts[i].ny;
        normals[index+2]  = verts[i].nz;
        index += 3;
    }
    
    glGenBuffers(1, &s_sphVBO);
    glBindBuffer(GL_ARRAY_BUFFER, s_sphVBO);
    glBufferData(GL_ARRAY_BUFFER, 2*3*verts.size()*sizeof(GLfloat), 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 3*verts.size()*sizeof(GLfloat), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 3*verts.size()*sizeof(GLfloat), 3*verts.size()*sizeof(GLfloat), normals);
    
    glGenVertexArrays(1, &s_sphVAO);
    glBindVertexArray(s_sphVAO);
    glBindBuffer(GL_ARRAY_BUFFER, s_sphVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (char*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (char*)0 + 3*verts.size()*sizeof(GLfloat));
    glBindVertexArray(0);
    
    s_sphNVerts = verts.size();
}


void QuickShapes::drawSphere(const Matrix4 &modelMatrix, const Matrix4 &viewMatrix,
                             const Matrix4 &projectionMatrix, const Color &color) {
    if (s_sphVAO == 0) {
        initSph();
    }
    
    ShaderUtils::startSimplePhongShader(modelMatrix.value_ptr(),
                                        viewMatrix.value_ptr(),
                                        projectionMatrix.value_ptr(),
                                        s_lightPos, s_lightAmb, s_lightDiff, s_lightSpec,
                                        color.value_ptr(), color.value_ptr(), s_matSpec, s_matShin);
    
    glBindVertexArray(s_sphVAO);
    glDrawArrays(GL_TRIANGLES, 0, s_sphNVerts);
    glBindVertexArray(0);
    
    ShaderUtils::stopSimpleShader();
}






// ------------  BRUSH  ------------


void QuickShapes::initBrush() {
    
    // Raw vertices -- points that make up the brush geometry
    const GLfloat v[19][3] = {
        { 0.5,   0.0,   0.0},  // 0
        {-0.5,   0.0,   0.0},  // 1
        
        { 0.5,   0.1,   0.25}, // 2
        {-0.5,   0.1,   0.25}, // 3
        { 0.5,   0.1,   0.75}, // 4
        {-0.5,   0.1,   0.75}, // 5
        { 0.1,   0.06,  1.0},  // 6
        {-0.1,   0.06,  1.0},  // 7
        { 0.15,  0.1,   1.75}, // 8
        {-0.15,  0.1,   1.75}, // 9
        
        { 0.0,   0.0,   1.85}, // 10
        
        { 0.5,  -0.1,   0.25}, // 11
        {-0.5,  -0.1,   0.25}, // 12
        { 0.5,  -0.1,   0.75}, // 13
        {-0.5,  -0.1,   0.75}, // 14
        { 0.1,  -0.06,  1.0},  // 15
        {-0.1,  -0.06,  1.0},  // 16
        { 0.15, -0.1,   1.75}, // 17
        {-0.15, -0.1,   1.75}  // 18
    };
    
    
    // Vertices arranged into triangles
    const GLfloat verts[34][3][3] = {
        // top
        {{v[0][0], v[0][1], v[0][2]},  {v[1][0], v[1][1], v[1][2]},  {v[2][0], v[2][1], v[2][2]}},
        {{v[1][0], v[1][1], v[1][2]},  {v[3][0], v[3][1], v[3][2]},  {v[2][0], v[2][1], v[2][2]}},
        
        {{v[2][0], v[2][1], v[2][2]},  {v[3][0], v[3][1], v[3][2]},  {v[4][0], v[4][1], v[4][2]}},
        {{v[3][0], v[3][1], v[3][2]},  {v[5][0], v[5][1], v[5][2]},  {v[4][0], v[4][1], v[4][2]}},
        
        {{v[4][0], v[4][1], v[4][2]},  {v[5][0], v[5][1], v[5][2]},  {v[6][0], v[6][1], v[6][2]}},
        {{v[5][0], v[5][1], v[5][2]},  {v[7][0], v[7][1], v[7][2]},  {v[6][0], v[6][1], v[6][2]}},
        
        {{v[6][0], v[6][1], v[6][2]},  {v[7][0], v[7][1], v[7][2]},  {v[8][0], v[8][1], v[8][2]}},
        {{v[7][0], v[7][1], v[7][2]},  {v[9][0], v[9][1], v[9][2]},  {v[8][0], v[8][1], v[8][2]}},
        
        {{v[8][0], v[8][1], v[8][2]},  {v[9][0], v[9][1], v[9][2]},  {v[10][0], v[10][1], v[10][2]}},
        
        // bottom
        {{v[0][0], v[0][1], v[0][2]},  {v[12][0], v[12][1], v[12][2]},  {v[1][0], v[1][1], v[1][2]}},
        {{v[11][0], v[11][1], v[11][2]},  {v[12][0], v[12][1], v[12][2]},  {v[0][0], v[0][1], v[0][2]}},
        
        {{v[11][0], v[11][1], v[11][2]},  {v[14][0], v[14][1], v[14][2]},  {v[12][0], v[12][1], v[12][2]}},
        {{v[13][0], v[13][1], v[13][2]},  {v[14][0], v[14][1], v[14][2]},  {v[11][0], v[11][1], v[11][2]}},
        
        {{v[13][0], v[13][1], v[13][2]},  {v[16][0], v[16][1], v[16][2]},  {v[14][0], v[14][1], v[14][2]}},
        {{v[15][0], v[15][1], v[15][2]},  {v[16][0], v[16][1], v[16][2]},  {v[13][0], v[13][1], v[13][2]}},
        
        {{v[15][0], v[15][1], v[15][2]},  {v[18][0], v[18][1], v[18][2]},  {v[16][0], v[16][1], v[16][2]}},
        {{v[17][0], v[17][1], v[17][2]},  {v[18][0], v[18][1], v[18][2]},  {v[15][0], v[15][1], v[15][2]}},
        
        {{v[18][0], v[18][1], v[18][2]},  {v[17][0], v[17][1], v[17][2]},  {v[10][0], v[10][1], v[10][2]}},
        
        // one side
        {{v[11][0], v[11][1], v[11][2]},  {v[0][0], v[0][1], v[0][2]},  {v[2][0], v[2][1], v[2][2]}},
        
        {{v[11][0], v[11][1], v[11][2]},  {v[2][0], v[2][1], v[2][2]},  {v[4][0], v[4][1], v[4][2]}},
        {{v[4][0], v[4][1], v[4][2]},  {v[13][0], v[13][1], v[13][2]},  {v[11][0], v[11][1], v[11][2]}},
        
        {{v[13][0], v[13][1], v[13][2]},  {v[4][0], v[4][1], v[4][2]},  {v[6][0], v[6][1], v[6][2]}},
        {{v[6][0], v[6][1], v[6][2]},  {v[15][0], v[15][1], v[15][2]},  {v[13][0], v[13][1], v[13][2]}},
        
        {{v[15][0], v[15][1], v[15][2]},  {v[6][0], v[6][1], v[6][2]},  {v[8][0], v[8][1], v[8][2]}},
        {{v[8][0], v[8][1], v[8][2]},  {v[17][0], v[17][1], v[17][2]},  {v[15][0], v[15][1], v[15][2]}},
        
        {{v[17][0], v[17][1], v[17][2]},  {v[8][0], v[8][1], v[8][2]},  {v[10][0], v[10][1], v[10][2]}},
        
        // other side
        {{v[3][0], v[3][1], v[3][2]},  {v[1][0], v[1][1], v[1][2]},  {v[12][0], v[12][1], v[12][2]}},
        
        {{v[3][0], v[3][1], v[3][2]},  {v[12][0], v[12][1], v[12][2]},  {v[14][0], v[14][1], v[14][2]}},
        {{v[14][0], v[14][1], v[14][2]},  {v[5][0], v[5][1], v[5][2]},  {v[3][0], v[3][1], v[3][2]}},
        
        {{v[5][0], v[5][1], v[5][2]},  {v[14][0], v[14][1], v[14][2]},  {v[16][0], v[16][1], v[16][2]}},
        {{v[16][0], v[16][1], v[16][2]},  {v[7][0], v[7][1], v[7][2]},  {v[5][0], v[5][1], v[5][2]}},
        
        {{v[7][0], v[7][1], v[7][2]},  {v[16][0], v[16][1], v[16][2]},  {v[18][0], v[18][1], v[18][2]}},
        {{v[18][0], v[18][1], v[18][2]},  {v[9][0], v[9][1], v[9][2]},  {v[7][0], v[7][1], v[7][2]}},
        
        {{v[9][0], v[9][1], v[9][2]},  {v[18][0], v[18][1], v[18][2]},  {v[10][0], v[10][1], v[10][2]}}
        
    };
    

    // Normals defined so as to make each face of the brush a flat surface
    const GLfloat norms[34][3][3] = {
        // top
        {{0.0, 0.93, -0.37}, {0.0, 0.93, -0.37}, {0.0, 0.93, -0.37}},
        {{0.0, 0.93, -0.37}, {0.0, 0.93, -0.37}, {0.0, 0.93, -0.37}},
        
        {{0.0, 1.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 1.0, 0.0}},
        {{0.0, 1.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 1.0, 0.0}},

        {{0.0, 0.988, 0.158}, {0.0, 0.988, 0.158}, {0.0, 0.988, 0.158}},
        {{0.0, 0.988, 0.158}, {0.0, 0.988, 0.158}, {0.0, 0.988, 0.158}},

        {{0.0, 0.999, -0.0533}, {0.0, 0.999, -0.0533}, {0.0, 0.999, -0.0533}},
        {{0.0, 0.999, -0.0533}, {0.0, 0.999, -0.0533}, {0.0, 0.999, -0.0533}},

        {{0.0, 0.709, 0.709}, {0.0, 0.709, 0.709}, {0.0, 0.709, 0.709}},
        
        // bottom
        {{0.0, -0.93, -0.37}, {0.0, -0.93, -0.37}, {0.0, -0.93, -0.37}},
        {{0.0, -0.93, -0.37}, {0.0, -0.93, -0.37}, {0.0, -0.93, -0.37}},

        {{0.0, -1.0, 0.0}, {0.0, -1.0, 0.0}, {0.0, -1.0, 0.0}},
        {{0.0, -1.0, 0.0}, {0.0, -1.0, 0.0}, {0.0, -1.0, 0.0}},

        {{0.0, -0.988, 0.158}, {0.0, -0.988, 0.158}, {0.0, -0.988, 0.158}},
        {{0.0, -0.988, 0.158}, {0.0, -0.988, 0.158}, {0.0, -0.988, 0.158}},

        {{0.0, -0.999, -0.0533}, {0.0, -0.999, -0.0533}, {0.0, -0.999, -0.0533}},
        {{0.0, -0.999, -0.0533}, {0.0, -0.999, -0.0533}, {0.0, -0.999, -0.0533}},

        {{0.0, -0.709, 0.709}, {0.0, -0.709, 0.709}, {0.0, -0.709, 0.709}},
        
        // one side
        {{1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}},
        
        {{1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}},
        {{1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}},

        {{0.848, 0.0, 0.530}, {0.848, 0.0, 0.530}, {0.848, 0.0, 0.530}},
        {{0.848, 0.0, 0.530}, {0.848, 0.0, 0.530}, {0.848, 0.0, 0.530}},

        {{1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}},
        {{1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}},

        {{0.709, 0.0, 0.709}, {0.709, 0.0, 0.709}, {0.709, 0.0, 0.709}},
        
        // other side
        {{-1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0}},

        {{-1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0}},
        {{-1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0}},

        {{-0.848, 0.0, 0.530}, {-0.848, 0.0, 0.530}, {-0.848, 0.0, 0.530}},
        {{-0.848, 0.0, 0.530}, {-0.848, 0.0, 0.530}, {-0.848, 0.0, 0.530}},

        {{-1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0}},
        {{-1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0}},

        {{-0.709, 0.0, 0.709}, {-0.709, 0.0, 0.709}, {-0.709, 0.0, 0.709}}
    };
    
    
    glGenVertexArrays(1, &s_brushVAO);
    glBindVertexArray(s_brushVAO);
    
    glGenBuffers(1, &s_brushVBO);
    glBindBuffer(GL_ARRAY_BUFFER, s_brushVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts)+sizeof(norms), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(verts), sizeof(norms), norms);
 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (char*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (char*)0 + sizeof(verts));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}


void QuickShapes::drawBrush(const Matrix4 &modelMatrix, const Matrix4 &viewMatrix,
                            const Matrix4 &projectionMatrix, const Color &color) {
    if (s_brushVAO == 0) {
        initBrush();
    }
    
    ShaderUtils::startSimplePhongShader(modelMatrix.value_ptr(),
                                        viewMatrix.value_ptr(),
                                        projectionMatrix.value_ptr(),
                                        s_lightPos, s_lightAmb, s_lightDiff, s_lightSpec,
                                        color.value_ptr(), color.value_ptr(), s_matSpec, s_matShin);
    
    glBindVertexArray(s_brushVAO);
    glDrawArrays(GL_TRIANGLES, 0, 102);
    glBindVertexArray(0);
    
    ShaderUtils::stopSimpleShader();
}



// ----------------
    
    
void QuickShapes::drawLineSegment(const Matrix4 &modelMatrix,
                                  const Matrix4 &viewMatrix,
                                  const Matrix4 &projectionMatrix,
                                  const Color &color,
                                  const Point3 &p1,
                                  const Point3 &p2,
                                  float radius)
{
    Matrix4 S = Matrix4::scale(Vector3(radius, 0.5*(p2-p1).length(), radius));
    Vector3 y = (p2-p1).normalize();
    Vector3 z = Vector3(1,0,0).cross(y).normalize();
    if (z == Vector3(0,0,0)) {
        z = Vector3(0,0,1).cross(y).normalize();
    }
    Vector3 x = y.cross(z);
    Matrix4 R = Matrix4::fromRowMajorElements(
        x[0], y[0], z[0], 0,
        x[1], y[1], z[1], 0,
        x[2], y[2], z[2], 0,
           0,    0,    0, 1
    );
    Matrix4 T = Matrix4::translation(0.5 * Vector3(p1[0]+p2[0], p1[1]+p2[1], p1[2]+p2[2]));
    
    Matrix4 M = T * R * S;
    
    drawCylinder(modelMatrix * M, viewMatrix, projectionMatrix, color);    
}

    

void QuickShapes::drawLines(const Matrix4 &modelMatrix,
                            const Matrix4 &viewMatrix,
                            const Matrix4 &projectionMatrix,
                            const Color &color,
                            const std::vector<Point3> &points,
                            LinesType ltype,
                            float radius)
{
    if (ltype == LinesType::LINES) {
        for (int i=0; i<points.size(); i+=2) {
            drawLineSegment(modelMatrix, viewMatrix, projectionMatrix, color, points[i], points[i+1], radius);
        }
    }
    else {
        for (int i=0; i<points.size()-1; i++) {
            drawLineSegment(modelMatrix, viewMatrix, projectionMatrix, color, points[i], points[i+1], radius);
        }
        if (ltype == LinesType::LINE_LOOP) {
            drawLineSegment(modelMatrix, viewMatrix, projectionMatrix, color, points[points.size()-1], points[0], radius);
        }
    }
}

void QuickShapes::drawFullscreenTexture(const Color &color, const int textureID) {
    glDepthMask(GL_FALSE);
    drawTexturedSquare(Matrix4().value_ptr(), Matrix4::rotationX(1.28).value_ptr(), Matrix4().value_ptr(), color, textureID);
    glDepthMask(GL_TRUE);
}
    
void QuickShapes::freeGPUMemory() {
    ShaderUtils::deleteSimpleShaders();
    
    if (s_cubeVAO != 0) {
        glDeleteBuffers(1, &s_cubeVBO);
        s_cubeVBO = 0;
        glDeleteVertexArrays(1, &s_cubeVAO);
        s_cubeVAO = 0;
    }
    
    if (s_squareVAO != 0) {
        glDeleteBuffers(1, &s_squareVBO);
        s_squareVBO = 0;
        glDeleteVertexArrays(1, &s_squareVAO);
        s_squareVAO = 0;
    }
    
    if (s_cylVAO != 0) {
        glDeleteBuffers(1, &s_cylVBO);
        s_cylVBO = 0;
        glDeleteVertexArrays(1, &s_cylVAO);
        s_cylVAO = 0;
    }
    
    if (s_coneVAO != 0) {
        glDeleteBuffers(1, &s_coneVBO);
        s_coneVBO = 0;
        glDeleteVertexArrays(1, &s_coneVAO);
        s_coneVAO = 0;
    }
    
    if (s_sphVAO != 0) {
        glDeleteBuffers(1, &s_sphVBO);
        s_sphVBO = 0;
        glDeleteVertexArrays(1, &s_sphVAO);
        s_sphVAO = 0;
    }
    
    if (s_brushVAO != 0) {
        glDeleteBuffers(1, &s_brushVBO);
        s_brushVBO = 0;
        glDeleteVertexArrays(1, &s_brushVAO);
        s_brushVAO = 0;
    }
}

} // end namespace
