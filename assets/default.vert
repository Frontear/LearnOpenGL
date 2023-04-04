#version 460 core

layout (location = 0) in vec2 xy;
layout (location = 1) in vec3 rgb;

uniform mat4 matrix = mat4(1.0);

out vec3 VertexColor;

void main() {
    gl_Position = matrix * vec4(xy, 1.0, 1.0);
    VertexColor = vec3(rgb);
}
