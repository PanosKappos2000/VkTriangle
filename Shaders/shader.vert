#version 450

vec2 positions[3] = vec2[]
(
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

vec3 color[3] = vec3[]
(
    vec3(1.0f, 0.0f, 0.0f),
    vec3(0.0f, 1.0f, 0.0f),
    vec3(0.0f, 0.0f, 1.0f)
);

layout (location = 0) out vec3 fragColor;

void main() 
{
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragColor = color[gl_VertexIndex];
}