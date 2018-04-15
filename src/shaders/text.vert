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

layout(location = 0) in vec3 vertex;
layout(location = 3) in vec2 texcoord;

uniform mat4 mvp_matrix;
uniform float scale;
uniform vec3 offset;

out vec2 uv;

void main() {
    uv = texcoord.xy;
    gl_Position = mvp_matrix * vec4(scale*(vertex+offset),1.0);
}
