#version 330

out vec4 fragColor;

uniform sampler2D uBuffer;
uniform vec2 uResolution;
uniform vec2 uRotation;
uniform float uScale;
uniform float uTime;
uniform int uFrame;
uniform int uSamples;

void main() {
    fragColor = vec4(vec3(gl_FragCoord.xy / uResolution, 0.0), 1.0);
}
