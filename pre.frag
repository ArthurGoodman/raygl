#version 330

out vec4 fragColor;

uniform sampler2D uBuffer;
uniform vec2 uResolution;
uniform vec2 uRotation;
uniform float uScale;
uniform float uTime;
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

vec4 menger(vec3 p) {
    float d = box(p, vec3(1.0));

    float s = 1.0;
    for (int m = 0; m < 3; m++) {
        vec3 a = mod(p * s, 2.0) - 1.0;

        s *= 3.0;

        vec3 r = abs(1.0 - 3.0 * abs(a));

        float da = max(r.x, r.y);
        float db = max(r.y, r.z);
        float dc = max(r.z, r.x);
        float c = (min(da, min(db, dc)) - 1.0) / s;

        d = max(d, c);
    }

    return vec4(d, vec3(0.5));
}

vec4 mandelbulb(vec3 p) {
    p.xyz = p.xzy;

    vec3 z = p;
    vec3 dz = vec3(0.0);
    float power = 8.0;
    float r, theta, phi;
    float dr = 1.0;

    float trap = 1.0;

    for(int i = 0; i < 7; i++) {
        r = length(z);

        if(r > 2.0)
            continue;

        theta = atan(z.y / z.x);
        phi = asin(z.z / r);

        dr = pow(r, power - 1.0) * dr * power + 1.0;

        r = pow(r, power);
        theta = theta * power;
        phi = phi * power;

        z = r * vec3(cos(theta) * cos(phi), sin(theta) * cos(phi), sin(phi)) + p;

        trap = min(trap, r);
    }

    trap = pow(clamp(trap, 0.0, 1.0), 0.55);
    vec3 trapColor = 0.5 + 0.5 * sin(3.0 + 4.2 * trap + vec3(0.0, 0.5, 1.0));
    return vec4(0.5 * log(r) * r / dr, trapColor);
}

//const float foldingLimit = 1.0;

//uniform float uMBScale;
//vec3 mtl = vec3(1.0, 1.3, 1.23) * 0.8;

//void sphereFold(inout vec3 z, inout float dz) {
//    const float fixedRadius2 = 1.9;
//    const float minRadius2 = 0.1;

//    float r2 = dot(z, z);
//    if (r2 < minRadius2) {
//        float temp = (fixedRadius2 / minRadius2);
//        z *= temp;
//        dz *= temp;
//    } else if(r2 < fixedRadius2) {
//        float temp = (fixedRadius2 / r2);
//        z *= temp;
//        dz *= temp;
//    }
//}

//void boxFold(inout vec3 z, inout float dz) {
//    z = clamp(z, -foldingLimit, foldingLimit) * 2.0 - z;
//}

//vec4 mandelbox(vec3 z) {
//    const int maxIt = 15;

//    vec3 offset = z;
//    float dr = 1.0;

//    for(int n = 0; n < maxIt; ++n) {
//        boxFold(z, dr);
//        sphereFold(z, dr);

//        z = uMBScale * z + offset;
//        dr = dr * abs(uMBScale) + 1.0;
//    }

//    float r = length(z);
//    return vec4(r / abs(dr), vec3(0.5));
//}

vec4 sierpinski(vec3 p) {
    const vec3 va = vec3(0.0, 0.57735 + 0.5, 0.0);
    const vec3 vb = vec3(0.0, -1.0 + 0.5, 1.15470);
    const vec3 vc = vec3(1.0, -1.0 + 0.5, -0.57735);
    const vec3 vd = vec3(-1.0, -1.0 + 0.5, -0.57735);

    float a = 0.0;
    float s = 1.0;
    float r = 1.0;
    float dm;
    vec3 v;

    for (int i = 0; i < 8; i++) {
        float d = dot(p - va, p - va);
        float t = 0.0;

        v = va;
        dm = d;

        d = dot(p - vb, p - vb);
        if (d < dm) {
            v = vb;
            dm = d;
            t = 1.0;
        }

        d = dot(p - vc, p - vc);
        if (d < dm) {
            v = vc;
            dm = d;
            t = 2.0;
        }

        d = dot(p - vd, p - vd);
        if (d < dm) {
            v = vd;
            dm = d;
            t = 3.0;
        }

        p = v + 2.0 * (p - v);
        r *= 2.0;
        a = t + 4.0 * a;
        s *= 4.0;
    }

    vec3 color = vec3(0.5);//0.5 + 0.5 * cos(6.2831 * a / s + vec3(0.0, 1.0, 2.0));
    return vec4((sqrt(dm) - 1.0) / r, color);
}

// Patterns.

vec3 checkerboard(vec3 p) {
    float f = mod(floor(5.0 * p.z) + floor(5.0 * p.x), 2.0);
    return 0.4 + 0.1 * f * vec3(1.0);
}
