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

in vec2 uv;
uniform sampler2D SurfaceTexture;

out vec4 fragColor;

void main() {
    fragColor = texture(SurfaceTexture, uv);
}