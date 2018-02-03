#version 330 
layout(location = 0) in vec3 position; 
layout(location = 1) in vec3 normal; 
layout(location = 2) in vec4 color; 
layout(location = 3) in vec2 texcoord;

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
   gl_Position	= ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(position, 1); 
   col_interp = color; 
} 