#type vertex
#version 330
#include "assets/shaders/Matrix.glsl"

layout(location = 0) in vec3 i_Pos;
layout(location = 1) in vec2 i_UV;
layout(location = 2) in vec3 i_Normal;

out vec3 v_WorldPos;
out vec3 v_WorldNormal;
out vec2 v_UV;

void main() {
    gl_Position = MATRIX_MVP * vec4(i_Pos, 1.0);
    v_WorldPos = (MATRIX_M * vec4(i_Pos, 1.0)).xyz;
    //v_WorldNormal = (MATRIX_M * vec4(i_Normal, 0.0)).xyz;
    v_WorldNormal = (vec4(i_Normal, 0.0) * inverse(MATRIX_M)).xyz; // Correct for non-uniform scaled objects
    v_UV = i_UV;
}



#type fragment
#version 330
#include "assets/shaders/Matrix.glsl"
#include "assets/shaders/Lighting.glsl"

layout(location = 0) out vec4 o_Color;

in vec3 v_WorldPos;
in vec3 v_WorldNormal;
in vec2 v_UV;

uniform sampler2D u_Albedo;
uniform float u_Metallic;
uniform float u_Roughness;
uniform float u_AO;

void main() {
    // Surface values
    vec3 albedo = texture(u_Albedo, v_UV).rgb;
    float metallic = 0.0;
    float roughness = 0.5;
    float ao = 1.0;


    
    // Outgoing light direction (vector from world-space fragment position to the "eye").
	vec3 Lo = normalize(u_CameraPos - v_WorldPos);

	// Get current fragment's normal and transform to world space.
	vec3 N = normalize(v_WorldNormal);
	
	// Angle between surface normal and outgoing light direction.
	float cosLo = max(0.0, dot(N, Lo));
		
	// Specular reflection vector.
	vec3 Lr = 2.0 * cosLo * N - Lo;

	// Fresnel reflectance at normal incidence (for metals use albedo color).
	vec3 F0 = mix(Fdielectric, albedo, metallic);

	// Direct lighting calculation for analytical lights.
	vec3 directLighting = vec3(0);
    vec3 Li = -light.direction;
    vec3 Lradiance = light.color;

    // Half-vector between Li and Lo.
    vec3 Lh = normalize(Li + Lo);

    // Calculate angles between surface normal and various light vectors.
    float cosLi = max(0.0, dot(N, Li));
    float cosLh = max(0.0, dot(N, Lh));

    // Calculate Fresnel term for direct lighting. 
    vec3 F  = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
    // Calculate normal distribution for specular BRDF.
    float D = ndfGGX(cosLh, roughness);
    // Calculate geometric attenuation for specular BRDF.
    float G = gaSchlickGGX(cosLi, cosLo, roughness);

    // Diffuse scattering happens due to light being refracted multiple times by a dielectric medium.
    // Metals on the other hand either reflect or absorb energy, so diffuse contribution is always zero.
    // To be energy conserving we must scale diffuse BRDF contribution based on Fresnel factor & metalness.
    vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metallic);

    // Lambert diffuse BRDF.
    // We don't scale by 1/PI for lighting & material units to be more convenient.
    // See: https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
    vec3 diffuseBRDF = kd * albedo;

    // Cook-Torrance specular microfacet BRDF.
    vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);

    // Total contribution for this light.
    directLighting += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;



    vec3 color = directLighting + vec3(0.01) * albedo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // Gamma correction
    color = pow(color, vec3(1.0 / 2.2)); 

    o_Color = vec4(color, 1.0);
}