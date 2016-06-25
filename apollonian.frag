vec4 map(vec3 p) {
    float time = 0;
    float s = 1.1 + 0.5 * smoothstep(-0.3, 0.3, cos(0.1 * time));

    float scale = 1.0;

    vec4 trap = vec4(1000.0);
    
    for (int i = 0; i < 8; i++) {
        p = -1.0 + 2.0 * fract(0.5 * p + 0.5);

        float r2 = dot(p, p);
        
        trap = min(trap, vec4(abs(p), r2));
        
        float k = max(s / r2, 0.1);
        p *= k;
        scale *= k;
    }

    vec3 rgb = vec3(1.0);
    rgb = mix(rgb, vec3(1.0, 0.80, 0.2), clamp(6.0 * trap.y, 0.0, 1.0) );
    rgb = mix(rgb, vec3(1.0, 0.55, 0.0), pow(clamp(1.0 - 2.0 * trap.z, 0.0, 1.0), 8.0));
    
    return vec4(0.25 * abs(p.y) / scale, rgb);
}
