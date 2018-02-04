/*
 This file is part of the MinGfx Project.
 
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 
 Original Author(s) of this File:
	Dan Keefe, 2018, University of Minnesota
	
 Author(s) of Significant Updates/Modifications to the File:
	...
 */

#ifndef SRC_DEFAULT_SHADER_H_
#define SRC_DEFAULT_SHADER_H_

#include "color.h"
#include "point3.h"
#include "shader_program.h"
#include "texture2d.h"
#include "vector3.h"
#include "matrix4.h"
#include "mesh.h"



namespace mingfx {

/** A simple GLSL shader for textured per-fragment Phong shading with multiple
 light sources.  This can be used to draw 3D models stored in a mingfx::Mesh
 data structure or you can use it with your own geometry data structures.
 Lighting properties are stored within the class itself since these are considered
 part of the shading model.  Material properties are considered properties of the
 meshes or other materials you wish to draw so these are stored outside of the
 class and passed into the Draw() or UseProgram() functions.
 
 An example of using DefaultShader to render a mesh:
 ~~~
 DefaultShader phong_shader;
 Mesh teapot;
 DefaultShader::MaterialProperties teapot_material;
 
 void Init() {
    // initialize the shader
    DefaultShader::LightProperties red_light;
    red_light.position = Point3(-10, 5, 5);
    red_light.diffuseIntensity = Color(1,0,0);
    phong_shader.AddLight(red_light);
 
    // initialize the mesh
    teapot.LoadFromOBJ(Platform::FindMinGfxDataFile("teapot.obj"));
 }
 
 void DrawUsingOpenGL() {
    Matrix4 M;
    Matrix4 V = Matrix4::LookAt(Point3(0,0,3), Point3(0,0,0), Vector3(0,1,0));
    Matrix4 P = Matrix4::Perspective(60.0, aspect_ratio(), 0.1, 10.0);
    phong_shader.Draw(M, V, P, teapot, teapot_material);
 }
 ~~~
 */
class DefaultShader {
public:
    
    /// If changed, this needs to also be changed in the glsl shader code
    static const unsigned int MAX_LIGHTS = 10;
    
    
    /// Small data structure to hold properties of the material to be lit
    class MaterialProperties {
    public:
        Color ambient_reflectance;
        Color diffuse_reflectance;
        Color specular_reflectance;
        float shinniness;
        Texture2D surface_texture;
        // eventually, this might include a normal map, etc.
        
        // defaults
        MaterialProperties() :
            ambient_reflectance(0.25, 0.25, 0.25),
            diffuse_reflectance(0.6, 0.6, 0.6),
            specular_reflectance(0.4, 0.4, 0.4),
            shinniness(20.0) {}
    };
    
    /// Small data structure to hold per-light properties
    class LightProperties {
    public:
        Point3 position;
        Color ambient_intensity;
        Color diffuse_intensity;
        Color specular_intensity;
        
        // defaults
        LightProperties() :
            position(10,10,10),
            ambient_intensity(0.25, 0.25, 0.25),
            diffuse_intensity(0.6, 0.6, 0.6),
            specular_intensity(0.6, 0.6, 0.6) {}
    };
    
    /// The constructor defaults to adding a single white light to the scene at
    /// (10,10,10).  Change this by passing it 'false'.  The constructor does
    /// not load and compile the shader right away.  This is done inside Init().
    DefaultShader(bool add_default_light=true);

    virtual ~DefaultShader();

    /// Multiple lights are supported, this adds one to the end of the list.
    /// Up to MAX_LIGHTS can be added.
    void AddLight(LightProperties light);
    
    /// Changes the properties for a light that was already added.
    void SetLight(int i, LightProperties light);


    /// This loads vertex and fragment shaders from files, compiles them, and
    /// links them.  So, it must be called from within an active OpenGL context,
    /// for example, from within GraphicsApp::Init() or GraphicsApp::DrawUsingOpenGL().
    /// If you call Draw() before calling Init(), then Init() will be called as
    /// the first step within Draw().  So, if you do not mind a slowdown on the
    /// very first frame of your program, it is fine to skip calling Init().
    void Init();

    /// This starts the shader and sets its uniform variables based upon the
    /// current set of lights, the material properties passed in, and the
    /// model, view, and projection matrices.  Then, it calls mesh->Draw().
    /// After drawing, it disables the shader.
    void Draw(const Matrix4 &model, const Matrix4 &view, const Matrix4 &projection,
              Mesh *mesh, const MaterialProperties &material);
    
    
    /// Only needed if you do not want to draw a Mesh.
    /// This does all of the same setup for drawing that the Draw() function does
    /// and then it returns so that you may draw your own geometry however you want.
    /// After doing your draw must call StopProgram() to turn off the shader.
    void UseProgram(const Matrix4 &model, const Matrix4 &view, const Matrix4 &projection,
                    const MaterialProperties &material);

    /// Only needed if you do not want to draw a Mesh.  Call this after UseProgram()
    /// and after drawing your geometry to turn off the shader.
    void StopProgram();
    
    
    int num_lights();
    
    LightProperties light(int i);
    
    
private:
    
    std::vector<LightProperties> lights_;

    // cached raw float arrays store data to send directly to the gpu
    // GLSL requires fixed size arrays for these
    float lightPositions_[3*MAX_LIGHTS];
    float lightIas_[4*MAX_LIGHTS];
    float lightIds_[4*MAX_LIGHTS];
    float lightIss_[4*MAX_LIGHTS];
    void update_light_arrays();
    
    ShaderProgram phongShader_;
};
    
} // end namespace

#endif