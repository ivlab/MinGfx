#version 330

in vec3 N;
in vec3 v;
in vec4 col_interp;

out vec4 fragColor;

uniform vec3 LightPosition;
uniform vec4 LightIntensityAmbient;
uniform vec4 LightIntensityDiffuse;
uniform vec4 LightIntensitySpecular;

uniform vec4 MatReflectanceAmbient;
uniform vec4 MatReflectanceDiffuse;
uniform vec4 MatReflectanceSpecular;
uniform float MatReflectanceShininess;

void main() {
    vec3 L = normalize(LightPosition - v);
    vec3 V = normalize(-v); // eye is at (0,0,0)
    vec3 R = normalize(-reflect(L,N));
    
    vec3 Ia = MatReflectanceAmbient.rgb * LightIntensityAmbient.rgb;
    
    vec3 Id = clamp(MatReflectanceDiffuse.rgb * LightIntensityDiffuse.rgb * max(dot(N, L), 0.0), 0.0, 1.0);
    
    vec3 Is = MatReflectanceSpecular.rgb * LightIntensitySpecular.rgb * pow(max(dot(R, V), 0.0), MatReflectanceShininess);
    Is = clamp(Is, 0.0, 1.0);
    
    fragColor = col_interp;
    fragColor.rgb *= Ia + Id + Is;
}