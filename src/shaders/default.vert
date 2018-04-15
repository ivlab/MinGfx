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

layout(location = 0) in vec3 position; 
layout(location = 1) in vec3 normal; 
layout(location = 2) in vec4 color; 
layout(location = 3) in vec2 texcoord;

layout(location = 8) in mat4 instance_xform;

uniform mat4 ModelMatrix; 
uniform mat4 ViewMatrix; 
uniform mat4 ProjectionMatrix; 
uniform mat4 NormalMatrix; 

out vec3 N; 
out vec3 v; 
out vec2 uv; 
out vec4 col_interp; 

void main() { 
   v = (ViewMatrix * ModelMatrix * vec4(position, 1)).xyz; 
   N = normalize((NormalMatrix * vec4(normal, 0)).xyz); 
   uv = texcoord.xy; 
   gl_Position	= ProjectionMatrix * ViewMatrix * instance_xform * ModelMatrix * vec4(position, 1);
   col_interp = color; 
} 