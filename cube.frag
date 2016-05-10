vec4 map(vec3 p) {
   vec4 result = vec4(plane(p), checkerboard(p));
   result = uni(result, vec4(box(p - vec3(0.5, 0.250001, 1.0), vec3(0.25, 0.25, 0.25)), vec3(0.4)));

   return result;
}
