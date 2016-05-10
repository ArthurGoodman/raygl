void main() {
    fragColor = vec4(vec3(gl_FragCoord.xy / uResolution, 0.0), 1.0);
}
