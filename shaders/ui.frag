#version 460 core

in vec2 vTexCoord;
in vec4 vColor;

out vec4 outColor;

uniform sampler2D uTexture;
uniform bool uUseTexture;

void main() {
    if (uUseTexture) {
        float alpha = texture(uTexture, vTexCoord).r;
        outColor = vec4(1.0, 1.0, 1.0, alpha);
    } else {
        outColor = vColor;
    }
}