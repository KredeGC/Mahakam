#ifndef STANDARD_LIGHTING_INCLUDED
#define STANDARD_LIGHTING_INCLUDED

// Lighting is divided into multiple steps
#include "internal/shaders/include/lighting/CookTorrance.glsl"
#include "internal/shaders/include/lighting/Shadows.glsl"

// Define the various functions for the lighting setup
#define PBR_DIRECT CookTorranceBRDF // A function used for direct lighting calculations, on a per-light basis
#define PBR_INDIRECT IndirectAmbient // A function used for indirect lighting calculations, only once per pixel
#define PBR_SHADOW CalculateShadowAttenuation // A function used to calculate filtered shadow attenutation
#define PBR_SHADOW_RAW CalculateShadowAttenuationRaw // A function used to calculate unfiltered shadow attenutation

// Include the PBR function itself, which uses the functions defined above
#include "internal/shaders/include/lighting/PBR.glsl"

#endif // STANDARD_LIGHTING_INCLUDED