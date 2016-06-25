vec4 map(vec3 p) {
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
