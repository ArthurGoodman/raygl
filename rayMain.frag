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
    light.direction = normalize(/*view * */vec3(-0.5, 0.5, -0.5));
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
