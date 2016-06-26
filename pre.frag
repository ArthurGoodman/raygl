#version 330

out vec4 fragColor;

uniform sampler2D uBuffer;
uniform vec2 uResolution;
uniform vec2 uRotation;
uniform vec2 uMouse;
uniform float uScale;
uniform float uTime;
uniform float uParam;
uniform int uFrame;
uniform int uSamples;

// Operations.

float subtract(float d1, float d2) {
    return max(-d2, d1);
}

vec4 uni(vec4 d1, vec4 d2) {
    return (d1.x < d2.x) ? d1 : d2;
}

vec3 repeat(vec3 p, vec3 c) {
    return mod(p, c) - 0.5 * c;
}

vec3 twist(vec3 p, float factor) {
    float c = cos(factor * p.y + factor);
    float s = sin(factor * p.y + factor);
    mat2 m = mat2(c, -s, s, c);
    return vec3(m * p.xz, p.y);
}

// Objects.

float plane(vec3 p) {
    return abs(p.y);
}

float sphere(vec3 p, float radius) {
    return length(p) - radius;
}

float box(vec3 p, vec3 sides) {
    vec3 d = abs(p) - sides;
    return min(max(d.x, max(d.y, d.z)), 0.0) + length(max(d, 0.0));
}

// Patterns.

vec3 checkerboard(vec3 p) {
    float f = mod(floor(5.0 * p.z) + floor(5.0 * p.x), 2.0);
    return 0.4 + 0.1 * f * vec3(1.0);
}
