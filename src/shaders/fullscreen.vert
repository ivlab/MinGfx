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
layout(location = 3) in vec2 texcoord;

out vec2 uv;

void main() {
    uv = texcoord.xy;
    gl_Position = vec4(position,1.0);
}
