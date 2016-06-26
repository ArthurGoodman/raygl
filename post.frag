#version 330

out vec4 fragColor;

uniform sampler2D uBuffer;
uniform vec2 uResolution;
uniform int uFrame;

void main() {
    vec2 uv = gl_FragCoord.xy / uResolution.xy;

    vec3 col = vec3(0.0);

    col = texture2D(uBuffer, uv).xyz;

    if(uFrame > 0)
        col /= float(uFrame);

    col = pow(col, vec3(0.4545));

    // color grading and vigneting
    col = pow(col, vec3(0.8, 0.85, 0.9));
    col *= 0.5 + 0.5 * pow(16.0 * uv.x * uv.y * (1.0 - uv.x) * (1.0 - uv.y), 0.1);

    fragColor = vec4(col, 1.0);
}
