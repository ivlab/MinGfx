/**
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All rights reserved.
 
 Original Author(s) of this File:
	Dan Keefe, 2017, University of Minnesota
	
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



namespace MinGfx {
    
    class DefaultShader {
    public:
        
        /// If changed, this needs to also be changed in the glsl shader code
        static const unsigned int MAX_LIGHTS = 10;
        
        
        /// Small data structure to hold properties of the material to be lit
        class MaterialProperties {
        public:
            Color ambientReflectance;
            Color diffuseReflectance;
            Color specularReflectance;
            float shinniness;
            Texture2D surfaceTexture;
            // eventually, this might include a normal map, etc.
            
            
            // defaults
            MaterialProperties() :
                ambientReflectance(0.25, 0.25, 0.25),
                diffuseReflectance(0.6, 0.6, 0.6),
                specularReflectance(0.4, 0.4, 0.4),
                shinniness(20.0) {}
        };
        
        /// Small data structure to hold per-light properties
        class LightProperties {
        public:
            Point3 position;
            Color ambientIntensity;
            Color diffuseIntensity;
            Color specularIntensity;
            
            // defaults
            LightProperties() :
                position(10,10,10),
                ambientIntensity(0.25, 0.25, 0.25),
                diffuseIntensity(0.6, 0.6, 0.6),
                specularIntensity(0.6, 0.6, 0.6) {}
        };
        
        
        
        /// Optionally adds a single white light to the scene at (10,10,10)
        DefaultShader(bool addDefaultLight=true);

        virtual ~DefaultShader();
    
        
        
        void add_light(LightProperties light);
        
        void set_light(int i, LightProperties light);

        int num_lights();
        
        LightProperties light(int i);
        
        
        
        
        void Init();
    
        void Draw(const Matrix4 &model, const Matrix4 &view, const Matrix4 &projection,
                  Mesh *mesh, const MaterialProperties &material);
        
        
        
        void UseProgram(const Matrix4 &model, const Matrix4 &view, const Matrix4 &projection,
                        const MaterialProperties &material);
        
        void StopProgram();
        
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