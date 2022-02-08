#ifndef LIGHT_STRUCT_INCLUDED
#define LIGHT_STRUCT_INCLUDED

#if defined(DIRECTIONAL)
    struct Light {
        vec3 direction;
        vec3 color;
        mat4 worldToLight;
        vec4 offset;
    };
#elif defined(POINT)
    struct Light {
        vec4 position; // xyz - pos, w - range
        vec4 color; // xyz - pos, w - 1.0 / (range * range)
        mat4 worldToLight;
        vec4 offset;
    };
#elif defined(SPOT)
    struct Light {
        mat4 objectToWorld;
        mat4 worldToLight;
        vec4 color; // xyz - color, w - 1.0 / (range * range)
        vec4 offset;
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