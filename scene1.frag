vec4 map(vec3 p) {
   vec4 result = vec4(plane(p), checkerboard(p));
   result = uni(result, vec4(sphere(p - vec3(0.0, 0.5, 0.0), 0.25), vec3(0.7, 0.3, 0.1)));
   result = uni(result, vec4(box(p - vec3(0.5, 0.250001, 1.0), vec3(0.25, 0.25, 0.25)), vec3(0.4)));
   result = uni(result, vec4(subtract(box(p - vec3(0.0, 0.5, 0.0), vec3(0.25, 0.25, 0.25)), sphere(p - vec3(0.0, 0.5, 0.0), 0.325)), vec3(0.5)));

   return result;
}
