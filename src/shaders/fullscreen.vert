#version 330
layout(location = 0) in vec3 position;
layout(location = 3) in vec2 texcoord;

out vec2 uv;

void main() {
    uv = texcoord.xy;
    gl_Position = vec4(position,1.0);
}
