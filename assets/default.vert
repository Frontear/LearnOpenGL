#version 460 core

layout (location = 0) in vec2 xy;
layout (location = 1) in vec3 rgb;

out vec3 v_color;

void main() {
    gl_Position = vec4(xy, 1.0, 1.0);
    v_color = vec3(rgb);
}
