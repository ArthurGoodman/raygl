#version 330

out vec4 color;

uniform vec2 resolution;
uniform float time;

void main() {
    color = vec4(gl_FragCoord.xy / resolution, 0, 1);
}
