void main() {
    const int samples = 2;
    const int maxIt = 1000;

    vec3 col = vec3(0.0);

    for (int sx = 0; sx < samples; sx++) {
        for (int sy = 0; sy < samples; sy++) {
            vec2 c = vec2(-.5, 0) + ((gl_FragCoord.xy - uResolution.xy / 2 + vec2(float(sx), float(sy)) / float(samples) / 1.5) / uScale - vec2(uRotation.x, -uRotation.y) / uScale) / uResolution.y;

            float q = (c.x - .25) * (c.x - .25) + c.y * c.y;
            if (q * (q + (c.x - .25)) < .25 * c.y * c.y)
                continue;

            int n = 0;
            vec2 z = vec2(0.0);

            for (; n < maxIt; n++) {
                z = vec2(z.x * z.x - z.y * z.y, 2 * z.x * z.y) + c;

                if (dot(z, z) > 8)
                    break;
            }

            if (n < maxIt)
                col += .5 + .5 * cos(vec3(3, 4, 11) + .05 * (n - log2(log2(dot(z, z)))));
        }
    }

    col /= float(samples * samples);

    fragColor = vec4(col, 1.0);
}
