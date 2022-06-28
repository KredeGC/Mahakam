// Normals are packed in the RG channels, preferably using BC5
vec3 UnpackNormal(vec2 xy) {
    xy *= 2.0;
    xy -= 1.0;
    
    vec3 n;
    n.x = xy.x;
    n.y = xy.y;
    n.z = 1.0 - clamp(dot(n.xy, n.xy), 0.0, 1.0);
    return normalize(n);
}