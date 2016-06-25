vec3 fsign(vec3 p) {return vec3(p.x<0.0?-1.0:1.0,p.y<0.0?-1.0:1.0,p.z<0.0?-1.0:1.0);}

vec3 paxis(vec3 p) { //idea from dila originally at https://www.shadertoy.com/view/Xlj3DK
    vec3 a=abs(p);
    return fsign(p)*max(fsign(a-max(a.yzx,a.zxy)),0.0);
}

vec3 paxis2(vec3 p){
    vec3 a=abs(p);
    return fsign(p)*max(fsign(a-min(a.yzx,a.zxy)),0.0);
}

vec4 map(in vec3 p){
    float b=1.0;
    for(int i=0;i<3;i++){
        p-=paxis(p)*b*(1.0);
        b*=0.5;
    }
    float d=length(p)-0.28;
    for(int i=0;i<4;i++){
        p-=paxis2(p)*b;
        b*=0.5;
    }
    p=abs(p);
    float d2=max(p.x,max(p.y,p.z))-b;
    return vec4(max(d2,-d), vec3(0.5));
}
