#version 460 core
layout(location = 0) in vec2 posIn;
layout(location = 1) in vec2 uvIn;

out vec2 fUV;

void main() {
    gl_Position = vec4(posIn, 0.0, 1.0); // should error
    fUV = uvIn;
}