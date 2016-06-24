void main() {
    const int samples = 2;
    const int maxIt = 1000;

    vec3 col = vec3(0.0);

    for (int sx = 0; sx < samples; sx++) {
        for (int sy = 0; sy < samples; sy++) {
            vec2 p = vec2(-.5, 0) + (vec2(uMouse.x, -uMouse.y) - vec2(uResolution.x, -uResolution.y) / 2) / uResolution.y;
            vec2 c = vec2(-.5, 0) + ((gl_FragCoord.xy - uResolution.xy / 2 + vec2(float(sx), float(sy)) / float(samples) / 1.5) / uScale - vec2(uRotation.x, -uRotation.y) / uScale) / uResolution.y;

            int n = 0;
            vec2 z = vec2(0.0);

            float trap = 1e20;

            for (; n < maxIt; n++) {
                z = vec2(z.x * z.x - z.y * z.y, 2 * z.x * z.y) + c;

                trap = min(trap, length(p - z));

                if (dot(z, z) > 8)
                    break;
            }

            // if (n < maxIt)
             //     col += .5 + .5 * cos(vec3(3, 4, 11) + .05 * (n - log2(log2(dot(z, z)))));

            col += vec3(0.5) + vec3(0.5) * cos(2 * 3.1415926 * (vec3(1.0) * trap + vec3(0.0, 0.1, 0.2)));
        }
    }

    col /= float(samples * samples);

    fragColor = vec4(col, 1.0);
}
