vec4 map(vec3 p) {
  return vec4(mandelbulb(p).x, 1.0, 1.0, 1.0);
}
