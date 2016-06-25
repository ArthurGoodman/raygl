vec4 map(vec3 p) {
    vec4 z = vec4(p, 0.0);
    float md2 = 1.0;
    float mz2 = dot(z, z);
    vec4 nz;

    vec4 c = 0.4 * cos(vec4(0.5, 3.9, 1.4, 1.1) + 1.5 * vec4(1.2, 1.7, 1.3, 2.5)) - vec4(0.3, 0.0, 0.0, 0.0);

    for (int i = 0; i < 11; i++) {
        md2 *= 4.0 * mz2;

        nz.x = z.x * z.x - dot(z.yzw, z.yzw);
        nz.yzw = 2.0 * z.x * z.yzw;
        z = nz + c;

        mz2 = dot(z, z);

        if (mz2 > 4.0)
            break;
    }

    return vec4(0.25 * sqrt(mz2 / md2) * log(mz2), vec3(1));
}
