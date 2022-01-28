#ifndef LIGHT_STRUCT_INCLUDED
#define LIGHT_STRUCT_INCLUDED

#if defined(DIRECTIONAL)
    struct Light {
        vec3 direction;
        vec3 color;
        //mat4 worldToLight;
    };
#elif defined(POINT)
    struct Light {
        vec4 position; // xyz - pos, w - light range/distance
        vec3 color;
        //mat4 worldToLight;
    };
#elif defined(SPOT)
    struct Light {
        vec4 position; // xyz - pos, w - light range/distance
        vec3 direction;
        vec3 color;
        float cutoff;
        //mat4 worldToLight;
    };
#endif


#if defined(DIRECTIONAL) || defined(POINT) || defined(SPOT)
    layout (std430, binding = 1) buffer Lights {
        #if defined(DIRECTIONAL)
            int lightAmount;
        #endif
        Light[] lights;
    };
#endif

#endif // LIGHT_STRUCT_INCLUDED