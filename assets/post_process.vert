#version 460 core

layout(location=0) in vec2 pos_in;
layout(location=1) in vec2 uv_in;

out vec2 f_uv;

void main() {
    gl_Position = vec4(pos_in, 0.0, 1.0);
    f_uv = uv_in;
}