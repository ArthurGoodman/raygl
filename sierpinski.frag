vec4 map(vec3 p) {
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

    vec3 color = 0.5 + 0.5 * cos(6.2831 * a / s + vec3(0.0, 1.0, 2.0));
    return vec4((sqrt(dm) - 1.0) / r, color);
}
