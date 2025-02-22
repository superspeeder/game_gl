#version 460 core

#define COS_PI6 0.866025403784

in vec2 f_uv;

out vec4 color_out;

uniform sampler2D uTexture;
uniform float uOffset;

void main() {
    vec2 red_point = vec2(f_uv.x + uOffset * COS_PI6, f_uv.y + uOffset * 0.5);
    vec2 green_point = vec2(f_uv.x - uOffset * COS_PI6, f_uv.y + uOffset * 0.5);
    vec2 blue_point = vec2(f_uv.x, f_uv.y - uOffset);

    vec4 red = texture(uTexture, red_point);
    vec4 green = texture(uTexture, green_point);
    vec4 blue = texture(uTexture, blue_point);

    color_out = vec4(red.r, green.g, blue.b, 1.0);
}