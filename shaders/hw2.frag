#version 330 core

in float normalizedHeight;

void main() {
    gl_FragColor = vec4(normalizedHeight, 0.0, 1.0, 1.0);
} 