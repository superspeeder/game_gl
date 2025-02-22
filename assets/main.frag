#version 460 core
in vec2 fUV;

out vec4 colorOut;

uniform sampler2D uTexture;

void main() {
    colorOut = texture(uTexture, fUV);
}