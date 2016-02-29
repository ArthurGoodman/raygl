#version 330

out vec4 fragColor;

uniform sampler2D uBuffer;

uniform vec2 uResolution;
uniform vec2 uRotation;
uniform float uScale;
uniform float uTime;
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

// Rendering.

vec4 map(vec3 p) { //return mandelbulb(p);
    vec4 result = vec4(plane(p), checkerboard(p));
    result = uni(result, vec4(sphere(p - vec3(0.0, 0.75, 0.0), 0.25), vec3(0.7, 0.3, 0.1)));
    result = uni(result, vec4(box(p - vec3(0.5, 0.250001, 1.0), vec3(0.25, 0.25, 0.25)), vec3(0.4)));
//    result = uni(result, vec4(subtract(box(p - vec3(0.0, 0.75, 0.0), vec3(0.25, 0.25, 0.25)), sphere(p - vec3(0.0, 0.75, 0.0), 0.325)), vec3(0.5)));

    return result;
}

vec4 castRay(vec3 origin, vec3 direction) {
    const float minDist = 0.0;
    float maxDist = 100.0;
    const int maxIt = 100;
    const float precis = 0.0001;

    float distance = minDist;
    vec3 color = vec3(-1.0);

    for (int i = 0; i < maxIt; i++) {
        vec4 result = map(origin + direction * distance);

        if (result.x < precis || distance > maxDist)
            break;

        distance += result.x;
        color = result.yzw;
    }

    if (distance > maxDist)
        color = vec3(-1.0);

    return vec4(distance, color);
}

float shadow(vec3 origin, vec3 direction, float minDist, float maxDist, float k) {
    const int maxIt = 64;

    float result = 1.0;
    float distance = minDist;

    for (int i = 0; i < maxIt; i++) {
        float h = map(origin + direction * distance).x;

        result = min(result, k * h / distance);
        distance += clamp(h, minDist, 0.1);

        if (h < 0.0001 || distance > maxDist)
            break;
    }

    return clamp(result, 0.0, 1.0);
}

vec3 calcNormal(vec3 p) {
    vec3 eps = vec3(0.001, 0.0, 0.0);
    vec3 normal = vec3(map(p + eps.xyy).x - map(p - eps.xyy).x,
                       map(p + eps.yxy).x - map(p - eps.yxy).x,
                       map(p + eps.yyx).x - map(p - eps.yyx).x);

    return normalize(normal);
}

float calcAmbientOcclusion(vec3 p, vec3 normal) {
    const int maxIt = 5;

    float occlusion = 0.0;
    float scale = 1.0;

    for (int i = 0; i < maxIt; i++) {
        float hr = 0.01 + 0.12 * float(i) / 4.0;
        vec3 aoPos = normal * hr + p;
        float dd = map(aoPos).x;
        occlusion += -(dd - hr) * scale;
        scale *= 0.95;
    }

    return clamp(1.0 - 3.0 * occlusion, 0.0, 1.0);
}

struct Light {
    vec3 direction;
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    vec3 sky;
    vec3 back;
    vec3 fresnel;
};

vec3 lighting(vec3 p, vec3 direction, Light light) {
    vec3 normal = calcNormal(p);
    vec3 reflection = reflect(direction, normal);

    float occlusion = calcAmbientOcclusion(p, normal);
    float ambient = clamp(0.5 + 0.5 * normal.y, 0.0, 1.0);
    float diffuse = clamp(dot(normal, light.direction), 0.0, 1.0);
    float back = clamp(dot(normal, normalize(vec3(-light.direction.x, 0.0, -light.direction.z))), 0.0, 1.0) * clamp(1.0 - p.y, 0.0, 1.0);
    float sky = smoothstep(-0.1, 0.1, reflection.y);
    float fresnel = pow(clamp(1.0 + dot(normal, direction), 0.0, 1.0), 2.0);
    float specular = pow(clamp(dot(reflection, light.direction), 0.0, 1.0), 16.0);

    diffuse *= shadow(p, light.direction, 0.02, 2.5, 8);
    sky *= shadow(p, reflection, 0.02, 2.5, 8);

    vec3 color = vec3(0.0);
    color += 1.20 * diffuse * light.diffuse;
    color += 1.20 * specular * light.specular * diffuse;
    color += 0.30 * ambient * light.ambient * occlusion;
    color += 0.40 * sky * light.sky * occlusion;
    color += 0.30 * back * light.back * occlusion;
    color += 0.40 * fresnel * light.fresnel * occlusion;

    return color;
}

vec3 render(vec3 origin, vec3 direction, Light light) {
    const vec3 background = vec3(0.8, 0.9, 1.0);

    vec4 result = castRay(origin, direction);
    float distance = result.x;
    vec3 color = result.yzw;

    if (color.x > -0.5) {
        vec3 position = origin + distance * direction;
        color *= lighting(position, direction, light);
        color = mix(color, background, 1.0 - exp(-0.00075 * distance * distance));
    } else color = background;

    return clamp(color, 0.0, 1.0);
}

mat3 camera(vec3 eye, vec3 target) {
    vec3 z = normalize(target - eye);
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 x = normalize(cross(z, up));
    vec3 y = normalize(cross(x, z));

    return mat3(x, y, z);
}

void main() {
    vec2 position = -1.0 + 2.0 * gl_FragCoord.xy / uResolution;
    position.x *= uResolution.x / uResolution.y;

    const float radius = 3.2;
    const float pi = 2.0 * acos(0.0);
    const float speed = 0.2;

    vec2 r = uRotation / uResolution;

//    vec3 eye = vec3(radius * cos(speed * uTime + 2 * pi * r.x), 1.0 + radius * r.y, radius * sin(speed * uTime + 2 * pi * r.x)) / uScale;

    vec2 sign = vec2(mod(r.x + 0.5, 2) <= 1 ? 1 : -1, mod(r.y + 0.5, 2) <= 1 ? 1 : -1);
    vec3 eye = vec3(/*sign.y * */cos(pi * r.y) * cos(pi * r.x), sign.y * sin(pi * r.y), sign.y * cos(pi * r.y) * sin(pi * r.x)) / uScale * 3;

    vec3 target = vec3(0.0, 0.0, 0.0);
    mat3 view = camera(eye, target);

    vec3 color = vec3(0.0);

    Light light;
    light.direction = normalize(view * vec3(-0.5, 0.5, -0.5));
    light.diffuse = vec3(1.0, 0.9, 0.8);
    light.specular = light.diffuse;
    light.ambient = vec3(0.5, 0.7, 1.0);
    light.sky = light.ambient;
    light.back = vec3(0.25, 0.25, 0.25);
    light.fresnel = vec3(1.0, 1.0, 1.0);

    for (int sx = 0; sx < uSamples; sx++)
        for (int sy = 0; sy < uSamples; sy++) {
            vec3 ray = view * normalize(vec3(position + vec2(sx, sy) / uResolution, 2.5) / uScale);
            color += render(eye, ray, light);
        }

    color /= uSamples * uSamples;

    const float gamma = 2.0;
    color = pow(color, vec3(1.0 / gamma));

    fragColor = vec4(color, 1.0);
}
