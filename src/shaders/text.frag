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
uniform vec4 color;
uniform sampler2D font_atlas;

out vec4 frag_color;

void main() {
    frag_color = color * texture(font_atlas, uv);
    //    frag_color.a = frag_color.r;
}