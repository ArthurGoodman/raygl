vec4 map(vec3 p) {
  return vec4(mandelbulb(p).x, 1.2, 1.1, 1.0);
}
