#version 460 core

layout (location = 0) in vec2 xy;
layout (location = 1) in vec3 rgb;

uniform mat4 matrix = mat4(1.0);

out vec3 v_color;

void main() {
    gl_Position = matrix * vec4(xy, 1.0, 1.0);
    v_color = vec3(rgb);
}
