float maxcomp(in vec3 p) {
    return max(p.x, max(p.y, p.z));
}

float sdBox(vec3 p, vec3 b) {
    vec3 di = abs(p) - b;
    float mc = maxcomp(di);
    return min(mc, length(max(di, 0.0)));
}

vec4 map(vec3 p) {
    vec3 w = p;
    vec3 q = p;

    q.xz = mod(q.xz + 1.0, 2.0) - 1.0;

    float d = sdBox(q, vec3(1.0));
    float s = 1.0;

    for (int m = 0; m < 6; m++) {
        float h = float(m) / 6.0;

        p = q - 0.5 * sin(abs(p.y) + float(m) * 3.0 + vec3(0.0, 3.0, 1.0));

        vec3 a = mod(p * s, 2.0) - 1.0;
        s *= 3.0;
        vec3 r = abs(1.0 - 3.0 * abs(a));

        float da = max(r.x, r.y);
        float db = max(r.y, r.z);
        float dc = max(r.z, r.x);
        float c = (min(da, min(db, dc)) - 1.0) / s;

        d = max(c, d);
   }

   float d1 = length(w - vec3(0.22, 0.35, 0.4)) - 0.09;
   d = min(d, d1);

   float d2 = w.y + 0.22;
   d = min(d, d2);

   return vec4(d, 1.0, 1.0, 1.0);
}
