#ifndef STANDARD_LIGHTING_INCLUDED
#define STANDARD_LIGHTING_INCLUDED

#include "assets/shaders/include/lighting/CookTorrance.glsl"

#define PBR_DIRECT CookTorranceBRDF
#define PBR_INDIRECT IndirectAmbient

#include "assets/shaders/include/lighting/PBR.glsl"

#endif // STANDARD_LIGHTING_INCLUDED