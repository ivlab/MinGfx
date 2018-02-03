
#include "default_shader.h"

#include "platform.h"


namespace MinGfx {
    
    DefaultShader::DefaultShader(bool addDefaultLight) {
        if (addDefaultLight) {
            add_light(LightProperties());
        }
    }
    
    DefaultShader::~DefaultShader() {
        
    }
    
    void DefaultShader::add_light(LightProperties light) {
        lights_.push_back(light);
        update_light_arrays();
    }
    
    void DefaultShader::set_light(int i, LightProperties light) {
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
                
            lightIas_[4*i + 0] = light->ambientIntensity[0];
            lightIas_[4*i + 1] = light->ambientIntensity[1];
            lightIas_[4*i + 2] = light->ambientIntensity[2];
            lightIas_[4*i + 3] = light->ambientIntensity[3];
            
            lightIds_[4*i + 0] = light->diffuseIntensity[0];
            lightIds_[4*i + 1] = light->diffuseIntensity[1];
            lightIds_[4*i + 2] = light->diffuseIntensity[2];
            lightIds_[4*i + 3] = light->diffuseIntensity[3];
            
            lightIss_[4*i + 0] = light->specularIntensity[0];
            lightIss_[4*i + 1] = light->specularIntensity[1];
            lightIss_[4*i + 2] = light->specularIntensity[2];
            lightIss_[4*i + 3] = light->specularIntensity[3];
        }
    }
    
    
    int DefaultShader::num_lights() {
        return lights_.size();
    }
    
    DefaultShader::LightProperties DefaultShader::light(int i) {
        return lights_[i];
    }

    
    void DefaultShader::Init() {
        phongShader_.AddVertexShaderFromFile(Platform::findMinGfxShaderFile("default.vert"));
        phongShader_.AddFragmentShaderFromFile(Platform::findMinGfxShaderFile("default.frag"));
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
        
        Matrix4 normalMatrix = (model*view).inverse().transpose();
        
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
        
        phongShader_.SetUniform("MatReflectanceAmbient", material.ambientReflectance);
        phongShader_.SetUniform("MatReflectanceDiffuse", material.diffuseReflectance);
        phongShader_.SetUniform("MatReflectanceSpecular", material.specularReflectance);
        phongShader_.SetUniform("MatReflectanceShininess", material.shinniness);
        phongShader_.SetUniform("UseSurfaceTexture", material.surfaceTexture.initialized());
        if (material.surfaceTexture.initialized()) {
            phongShader_.BindTexture("SurfaceTexture", material.surfaceTexture);
        }
    }
    
    
    void DefaultShader::StopProgram() {
        // Deactivate the shader program
        phongShader_.StopProgram();
    }
    
    
    
} // end namespace

