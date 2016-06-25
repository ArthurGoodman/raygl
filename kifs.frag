vec4 map(vec3 p) {
    // I'm never sure whether I should take constant stuff like the following outside the function, 
    // or not. My 1990s CPU brain tells me outside, but it doesn't seem to make a difference to frame 
    // rate in this environment one way or the other, so I'll keep it where it looks tidy. If a GPU
    // architecture\compiler expert is out there, feel free to let me know.
    
    const vec3 offs = vec3(1, .75, .5); // Offset point.
    const vec2 a = sin(vec2(0, 1.57079632) + 1.57/2.);
    const mat2 m = mat2(a.y, -a.x, a);
    const vec2 a2 = sin(vec2(0, 1.57079632) + 1.57/4.);
    const mat2 m2 = mat2(a2.y, -a2.x, a2);
    
    const float s = 5.; // Scale factor.
    
    float d = 1e5; // Distance.
    
    
    p  = abs(fract(p*.5)*2. - 1.); // Standard spacial repetition.
     
    
    float amp = 1./s; // Analogous to layer amplitude.
    
   
    // With only two iterations, you could unroll this for more speed,
    // but I'm leaving it this way for anyone who wants to try more
    // iterations.
    for(int i=0; i<2; i++){
        
        // Rotating.
        p.xy = m*p.xy;
        p.yz = m2*p.yz;
        
        p = abs(p);
        
        // Folding about tetrahedral planes of symmetry... I think, or is it octahedral? 
        // I should know this stuff, but topology was many years ago for me. In fact, 
        // everything was years ago. :)
        // Branchless equivalent to: if (p.x<p.y) p.xy = p.yx;
        p.xy += step(p.x, p.y)*(p.yx - p.xy);
        p.xz += step(p.x, p.z)*(p.zx - p.xz);
        p.yz += step(p.y, p.z)*(p.zy - p.yz);
 
        // Stretching about an offset.
        p = p*s + offs*(1. - s);
        
        // Branchless equivalent to:
        // if( p.z < offs.z*(1. - s)*.5)  p.z -= offs.z*(1. - s);
        p.z -= step(p.z, offs.z*(1. - s)*.5)*offs.z*(1. - s);
        
        // Loosely speaking, construct an object, and combine it with
        // the object from the previous iteration. The object and
        // comparison are a cube and minimum, but all kinds of 
        // combinations are possible.
        p=abs(p);
        d = min(d, max(max(p.x, p.y), p.z)*amp);
        
        amp /= s; // Decrease the amplitude by the scaling factor.
    }
 
    return vec4(d - .035, vec3(1.0)); // .35 is analous to the object size.
}
