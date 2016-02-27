#version 330

out vec4 color;

void main() {
    color = vec4(gl_FragCoord.x / 1920 * 2, gl_FragCoord.y / 1080 * 2, 0, 1);
}
