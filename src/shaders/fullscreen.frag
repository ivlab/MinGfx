#version 330

in vec2 uv;
uniform sampler2D SurfaceTexture;

out vec4 fragColor;

void main() {
    fragColor = texture(SurfaceTexture, uv);
}