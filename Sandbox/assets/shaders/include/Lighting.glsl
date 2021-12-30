struct Light {
    vec3 direction;
    vec3 color;
};


layout (std140) uniform Lights {
    Light light;
};

const float PI = 3.14159265359;
const float Epsilon = 0.00001;
const vec3 Fdielectric = vec3(0.04);

uniform sampler2D u_IrradianceMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 sampleSphericalMap(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2.
float ndfGGX(float cosLh, float roughness)
{
	float alpha   = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (PI * denom * denom);
}

// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
	return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

// Shlick's approximation of the Fresnel factor.
vec3 fresnelSchlick(vec3 F0, float cosTheta)
{
	return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
} 

// Calculate attenuated light falloff
float calculateLightFalloff(float dist, float outerRadius) {
	float normalizedDist = dist / outerRadius;
	return clamp(1.0 / (1.0 + 25.0 * normalizedDist * normalizedDist) * clamp((1.0 - normalizedDist) * 5.0, 0.0, 1.0), 0.0, 1.0);
}



vec3 BRDF_Direct(Light light, vec3 albedo, float metallic, float roughness, float ao, vec3 viewDir, vec3 worldPos, vec3 worldNormal) {
	// Angle between surface normal and outgoing light direction.
	float cosLo = max(0.0, dot(worldNormal, viewDir));
		
	// Specular reflection vector.
	vec3 Lr = 2.0 * cosLo * worldNormal - viewDir;

	// Fresnel reflectance at normal incidence (for metals use albedo color).
	vec3 F0 = mix(Fdielectric, albedo, metallic);

	// Direct lighting calculation for analytical lights.
    //float dist = length(v_WorldPos - light.direction);
	vec3 directLighting = vec3(0);
    vec3 Li = -normalize(light.direction);
    vec3 Lradiance = light.color;

    // Half-vector between Li and Lo.
    vec3 Lh = normalize(Li + viewDir);

    // Calculate angles between surface normal and various light vectors.
    float cosLi = max(0.0, dot(worldNormal, Li));
    float cosLh = max(0.0, dot(worldNormal, Lh));

    // Calculate Fresnel term for direct lighting. 
    vec3 F  = fresnelSchlick(F0, max(0.0, dot(Lh, viewDir)));
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


    vec2 uv = sampleSphericalMap(worldNormal);

    vec3 kS = fresnelSchlickRoughness(max(dot(worldNormal, viewDir), 0.0), F0, roughness); 
    vec3 kD = 1.0 - kS;
    vec3 irradiance = texture(u_IrradianceMap, uv, roughness * 4.0).rgb;
    vec3 diffuse    = irradiance * albedo;
    vec3 ambient    = (kD * diffuse) * ao;


	return directLighting + ambient;
}