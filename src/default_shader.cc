/*
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 See corresponding header file for details.
 */

#include "default_shader.h"

#include "platform.h"


namespace mingfx {
    
    DefaultShader::DefaultShader(bool addDefaultLight) {
        if (addDefaultLight) {
            AddLight(LightProperties());
        }
    }
    
    DefaultShader::~DefaultShader() {
        
    }
    
    void DefaultShader::AddLight(LightProperties light) {
        lights_.push_back(light);
        update_light_arrays();
    }
    
    void DefaultShader::SetLight(int i, LightProperties light) {
        lights_[i] = light;
        update_light_arrays();
    }
    
    void DefaultShader::update_light_arrays() {
        DefaultShader::LightProperties defaultlight;
        
        for (int i=0; i<MAX_LIGHTS; i++) {
            DefaultShader::LightProperties *light;
            if (i < lights_.size()) {
                light = &lights_[i];
            }
            else {
                light = &defaultlight;
            }
            
            lightPositions_[3*i + 0] = light->position[0];
            lightPositions_[3*i + 1] = light->position[1];
            lightPositions_[3*i + 2] = light->position[2];
                
            lightIas_[4*i + 0] = light->ambient_intensity[0];
            lightIas_[4*i + 1] = light->ambient_intensity[1];
            lightIas_[4*i + 2] = light->ambient_intensity[2];
            lightIas_[4*i + 3] = light->ambient_intensity[3];
            
            lightIds_[4*i + 0] = light->diffuse_intensity[0];
            lightIds_[4*i + 1] = light->diffuse_intensity[1];
            lightIds_[4*i + 2] = light->diffuse_intensity[2];
            lightIds_[4*i + 3] = light->diffuse_intensity[3];
            
            lightIss_[4*i + 0] = light->specular_intensity[0];
            lightIss_[4*i + 1] = light->specular_intensity[1];
            lightIss_[4*i + 2] = light->specular_intensity[2];
            lightIss_[4*i + 3] = light->specular_intensity[3];
        }
    }
    
    
    int DefaultShader::num_lights() {
        return lights_.size();
    }
    
    DefaultShader::LightProperties DefaultShader::light(int i) {
        return lights_[i];
    }

    
    void DefaultShader::Init() {
        phongShader_.AddVertexShaderFromFile(Platform::FindMinGfxShaderFile("default.vert"));
        phongShader_.AddFragmentShaderFromFile(Platform::FindMinGfxShaderFile("default.frag"));
        phongShader_.LinkProgram();
    }
    
    
    
    void DefaultShader::Draw(const Matrix4 &model, const Matrix4 &view, const Matrix4 &projection,
                             Mesh *mesh, const MaterialProperties &material)
    {
        UseProgram(model, view, projection, material);
        
        // Draw the mesh using the shader program
        mesh->Draw();
        
        StopProgram();
    }
    
    
    void DefaultShader::UseProgram(const Matrix4 &model, const Matrix4 &view, const Matrix4 &projection,
                                   const MaterialProperties &material)
    {
        if (!phongShader_.initialized()) {
            Init();
        }
        
        Matrix4 normalMatrix = (view*model).Inverse().Transpose();
        
        for (int i=0; i<lights_.size(); i++) {
            Point3 light_in_eye_space = view * lights_[i].position;
            lightPositions_[3*i + 0] = light_in_eye_space[0];
            lightPositions_[3*i + 1] = light_in_eye_space[1];
            lightPositions_[3*i + 2] = light_in_eye_space[2];
        }
        
        // Activate the shader program
        phongShader_.UseProgram();
        
        // Pass uniforms and textures from C++ to the GPU Shader Program
        phongShader_.SetUniform("ModelMatrix", model);
        phongShader_.SetUniform("ViewMatrix", view);
        phongShader_.SetUniform("ProjectionMatrix", projection);
        phongShader_.SetUniform("NormalMatrix", normalMatrix);
        
        phongShader_.SetUniform("NumLights", (int)lights_.size());
        phongShader_.SetUniformArray3("LightPositions", lightPositions_, MAX_LIGHTS);
        phongShader_.SetUniformArray4("LightIntensitiesAmbient", lightIas_, MAX_LIGHTS);
        phongShader_.SetUniformArray4("LightIntensitiesDiffuse", lightIds_, MAX_LIGHTS);
        phongShader_.SetUniformArray4("LightIntensitiesSpecular", lightIss_, MAX_LIGHTS);
        
        phongShader_.SetUniform("MatReflectanceAmbient", material.ambient_reflectance);
        phongShader_.SetUniform("MatReflectanceDiffuse", material.diffuse_reflectance);
        phongShader_.SetUniform("MatReflectanceSpecular", material.specular_reflectance);
        phongShader_.SetUniform("MatReflectanceShininess", material.shinniness);
        phongShader_.SetUniform("UseSurfaceTexture", material.surface_texture.initialized());
        if (material.surface_texture.initialized()) {
            phongShader_.BindTexture("SurfaceTexture", material.surface_texture);
        }
    }
    
    
    void DefaultShader::StopProgram() {
        // Deactivate the shader program
        phongShader_.StopProgram();
    }
    
    
    
} // end namespace

