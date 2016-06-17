float hash(float seed) {
    return fract(sin(seed)*43758.5453 );
}

vec3 cosineDirection( in float seed, in vec3 nor) {
    // compute basis from normal
    // see http://orbit.dtu.dk/fedora/objects/orbit:113874/datastreams/file_75b66578-222e-4c7d-abdf-f7e255100209/content
    // (link provided by nimitz)
    vec3 tc = vec3( 1.0+nor.z-nor.xy*nor.xy, -nor.x*nor.y)/(1.0+nor.z);
    vec3 uu = vec3( tc.x, tc.z, -nor.x );
    vec3 vv = vec3( tc.z, tc.y, -nor.y );

    float u = hash( 78.233 + seed);
    float v = hash( 10.873 + seed);
    float a = 6.283185 * v;

    return  sqrt(u)*(cos(a)*uu + sin(a)*vv) + sqrt(1.0-u)*nor;
}

vec3 calcNormal( in vec3 pos ) {
    vec3 eps = vec3(0.0001,0.0,0.0);

    return normalize(vec3(
      map( pos+eps.xyy ).x - map( pos-eps.xyy ).x,
      map( pos+eps.yxy ).x - map( pos-eps.yxy ).x,
      map( pos+eps.yyx ).x - map( pos-eps.yyx ).x));
}

vec4 intersect( in vec3 ro, in vec3 rd ) {
    float res = -1.0;
    float tmax = 100.0;
    float t = 0.01;
    vec3 color = vec3(0.0);
    for(int i=0; i<128; i++ )
    {
        vec4 h = map(ro+rd*t);
        if( h.x<0.0001 || t>tmax ) break;
        t += h.x;
        color = h.yzw;
    }

    if( t<tmax ) res = t;

    return vec4(res, color);
}

float shadow( in vec3 ro, in vec3 rd ) {
    float res = 0.0;

    float tmax = 12.0;

    float t = 0.001;
    for(int i=0; i<80; i++ ) {
        float h = map(ro+rd*t).x;
        if( h<0.0001 || t>tmax) break;
        t += h;
    }

    if( t>tmax ) res = 1.0;

    return res;
}

//vec3 sunDir = normalize(vec3(-0.3,1.3,0.1));
vec3 sunDir = normalize(vec3(-0.5, 0.5,-0.5));
vec3 sunCol = 6.0 * vec3(1.0,0.8,0.6);
vec3 skyCol = 4.0 * vec3(0.2,0.35,0.5);

vec3 calculateColor(vec3 ro, vec3 rd, float sa ) {
    const float epsilon = 0.0001;
    
    vec3 colorMask = vec3(1.0);
    vec3 accumulatedColor = vec3(0.0);

    float fdis = 0.0;
    for( int bounce = 0; bounce<3; bounce++ ) // bounces of GI
    {
        rd = normalize(rd);

        //-----------------------
        // trace
        //-----------------------
        vec4 hit = intersect( ro, rd ); 
        float t = hit.x;
        if( t < 0.0 )
        {
            if ( bounce==0 )
                // return mix( 0.05*vec3(0.9,1.0,1.0), skyCol, smoothstep(0.1,0.25,rd.y) );
                return skyCol;

            break;
        }

        if( bounce==0 ) fdis = t;

        vec3 pos = ro + rd * t;
        vec3 nor = calcNormal( pos );
        vec3 surfaceColor = vec3(0.4)*hit.yzw;//vec3(1.2,1.1,1.0);

        //-----------------------
        // add direct lighitng
        //-----------------------
        colorMask *= surfaceColor;

        vec3 iColor = vec3(0.0);

        // light 1
        float sunDif =  max(0.0, dot(sunDir, nor));
        float sunSha = 1.0; if( sunDif > 0.00001 ) sunSha = shadow( pos + nor*epsilon, sunDir);
        iColor += sunCol * sunDif * sunSha;
        // todo - add back direct specular

        // light 2
        vec3 skyPoint = cosineDirection( sa + 7.1*float(uFrame) + 5681.123 + float(bounce)*92.13, nor);
        float skySha = shadow( pos + nor*epsilon, skyPoint);
        iColor += skyCol * skySha;

        accumulatedColor += colorMask * iColor;

        //-----------------------
        // calculate new ray
        //-----------------------
        //float isDif = 0.8;
        //if( hash(sa + 1.123 + 7.7*float(bounce)) < isDif )
        {
           rd = cosineDirection(76.2 + 73.1*float(bounce) + sa + 17.7*float(uFrame), nor);
        }
        //else
        {
        //    float glossiness = 0.2;
        //    rd = normalize(reflect(rd, nor)) + uniformVector(sa + 111.123 + 65.2*float(bounce)) * glossiness;
        }

        ro = pos;
   }

   // float ff = exp(-0.01*fdis*fdis);
   // accumulatedColor *= ff;
   // accumulatedColor += (1.0-ff)*0.05*vec3(0.9,1.0,1.0);

   accumulatedColor = mix(accumulatedColor, skyCol, 1.0 - exp(-0.00075 * fdis * fdis));

   return accumulatedColor;
}

mat3 camera(in vec3 eye, in vec3 target, in float cr) {
    vec3 cw = normalize(target - eye);
    vec3 cp = vec3(sin(cr), cos(cr), 0.0);
    vec3 cu = normalize(cross(cw, cp));
    vec3 cv = normalize(cross(cu, cw));
    return mat3(cu, cv, -cw);
}

void main() {
    const float pi = 2.0 * acos(0.0);

    float sa = hash( dot( gl_FragCoord.xy, vec2(12.9898, 78.233) ) + 1113.1*float(uFrame) );

    vec2 of = -0.5 + vec2( hash(sa+13.271), hash(sa+63.216) );
    vec2 p = (-uResolution.xy + 2.0*(gl_FragCoord.xy+of)) / uResolution.y;

    vec2 r = uRotation / uResolution;

    vec2 sign = vec2(mod(r.x + 0.5, 2) <= 1 ? 1 : -1, mod(r.y + 0.5, 2) <= 1 ? 1 : -1);
    vec3 eye = vec3(/*sign.y * */cos(pi * r.y) * cos(pi * r.x), sign.y * sin(pi * r.y), sign.y * cos(pi * r.y) * sin(pi * r.x)) / uScale * 3;

    mat3 view = camera(eye, vec3(0.0, 0.0, 0.0), 0.0);
    vec3 target = normalize(view * vec3(p, -2.5) / uScale);

    vec3 col = texture2D( uBuffer, gl_FragCoord.xy/uResolution.xy ).xyz;
    if( uFrame==0 ) col = vec3(0.0);

    col += calculateColor(eye, target, sa);

    fragColor = vec4( col, 1.0 );
}
