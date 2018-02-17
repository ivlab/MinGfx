#version 330

/*
 This file is part of the MinGfx Project.
 
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 
 Original Author(s) of this File:
	Dan Keefe, 2018, University of Minnesota
	
 Author(s) of Significant Updates/Modifications to the File:
	...
 */

const int MAX_LIGHTS = 10;

in vec3 N;
in vec3 v;
in vec2 uv;
in vec4 col_interp;

out vec4 fragColor;

uniform int NumLights;
uniform vec3 LightPositions[MAX_LIGHTS];
uniform vec4 LightIntensitiesAmbient[MAX_LIGHTS];
uniform vec4 LightIntensitiesDiffuse[MAX_LIGHTS];
uniform vec4 LightIntensitiesSpecular[MAX_LIGHTS];

uniform vec4 MatReflectanceAmbient;
uniform vec4 MatReflectanceDiffuse;
uniform vec4 MatReflectanceSpecular;
uniform float MatReflectanceShininess;

uniform int UseSurfaceTexture;
uniform sampler2D SurfaceTexture;

void main() {

    // initialize the fragment color to the interpolated value of per-vertex colors
    // since some meshes will have color specified per-vertex.  if there are no
    // per vertex colors, then this will default to white.
    fragColor = col_interp;


    // if there is a surface texture, then factor this into the base color of this
    // fragment as well
    if (UseSurfaceTexture != 0) {
        fragColor *= texture(SurfaceTexture, uv);
    }

    // modulate this base color by the additive intensity from all light sources
    vec3 Ia = vec3(0,0,0);
    vec3 Id = vec3(0,0,0);
    vec3 Is = vec3(0,0,0);
    
    vec3 n = normalize(N);
    
    for (int i=0; i<NumLights; i++) {
        vec3 L = normalize(LightPositions[i] - v);
        vec3 V = normalize(-v); // eye is at (0,0,0)
        vec3 R = normalize(-reflect(L,N));

        Ia += MatReflectanceAmbient.rgb * LightIntensitiesAmbient[i].rgb;
        
        if (dot(n,L) > 0.0) {
            Id += clamp(MatReflectanceDiffuse.rgb * LightIntensitiesDiffuse[i].rgb * max(dot(n, L), 0.0), 0.0, 1.0);

            Is += MatReflectanceSpecular.rgb * LightIntensitiesSpecular[i].rgb * pow(max(dot(R, V), 0.0), MatReflectanceShininess);
            Is = clamp(Is, 0.0, 1.0);
        }
    }
    fragColor.rgb *= Ia + Id + Is;
}

