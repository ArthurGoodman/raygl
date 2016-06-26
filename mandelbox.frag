void sphereFold(inout vec3 z, inout float dz) {
    const float fixedRadius2 = 1.9;
    const float minRadius2 = 0.1;

    float r2 = dot(z, z);
    if (r2 < minRadius2) {
        float temp = (fixedRadius2 / minRadius2);
        z *= temp;
        dz *= temp;
    } else if(r2 < fixedRadius2) {
        float temp = (fixedRadius2 / r2);
        z *= temp;
        dz *= temp;
    }
}

void boxFold(inout vec3 z, inout float dz) {
    const float foldingLimit = 1.0;
    z = clamp(z, -foldingLimit, foldingLimit) * 2.0 - z;
}

vec4 map(vec3 z) {
    const int maxIt = 15;

    vec3 offset = z;
    float dr = 1.0;

    for(int n = 0; n < maxIt; ++n) {
        boxFold(z, dr);
        sphereFold(z, dr);

        z = uParam * z + offset;
        dr = dr * abs(uParam) + 1.0;
    }

    float r = length(z);
    return vec4(r / abs(dr), vec3(0.5));
}
