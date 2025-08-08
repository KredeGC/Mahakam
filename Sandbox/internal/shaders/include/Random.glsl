#ifndef RANDOM_INCLUDED
#define RANDOM_INCLUDED

float rand2dTo1d(vec2 value, vec2 dotDir) {
    //vec2 smallValue = cos(value); // Removing might be faster
    float random = dot(value, dotDir);
    random = fract(sin(random) * 143758.5453);
    return random;
}

vec2 rand2dTo2d(vec2 value) {
    return vec2(
        rand2dTo1d(value, vec2(12.989, 78.233)),
        rand2dTo1d(value, vec2(39.346, 11.135))
    );
}

float rand3dTo1d(vec3 value, vec3 dotDir) {
    //vec3 smallValue = cos(value); // Removing might be faster
    float random = dot(value, dotDir);
    random = fract(sin(random) * 143758.5453);
    return random;
}

vec2 rand3dTo2d(vec3 value) {
    return vec2(
        rand3dTo1d(value, vec3(12.989, 78.233, 37.719)),
        rand3dTo1d(value, vec3(39.346, 11.135, 83.155))
    );
}

#endif // RANDOM_INCLUDED