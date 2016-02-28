#version 330

out vec4 color;

uniform vec2 resolution;
uniform float time;

void main() {
    //heavy computing
    int n = 50000 + int(time);
    float f = n;
    for(int i = 0; i < n; i++) {
        f -= 1;
    }

    color = vec4(gl_FragCoord.xy / resolution, 0, 1);
}
