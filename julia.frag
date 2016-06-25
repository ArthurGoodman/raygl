void main() {
    const int maxIt = 1000;

    vec3 col = vec3(0.0);

    for (int sx = 0; sx < uSamples; sx++) {
        for (int sy = 0; sy < uSamples; sy++) {
            vec2 p = vec2(-.5, 0) + vec2(uMouse.x, -uMouse.y) / uResolution.y;
            vec2 c = vec2(-.5, 0) + ((gl_FragCoord.xy - uResolution.xy / 2 + vec2(float(sx), float(sy)) / float(uSamples) / 1.5) / uScale - vec2(uRotation.x, -uRotation.y)) / uResolution.y;

            int n = 0;
            vec2 z = c;

            for (; n < maxIt; n++) {
                z = vec2(z.x * z.x - z.y * z.y, 2 * z.x * z.y) + p;

                if (dot(z, z) > 8)
                    break;
            }

            if (n < maxIt)
                col += .5 + .5 * cos(vec3(3, 4, 11) + .05 * (n - log2(log2(dot(z, z)))));
        }
    }

    col /= float(uSamples * uSamples);

    fragColor = vec4(col, 1.0);
}
