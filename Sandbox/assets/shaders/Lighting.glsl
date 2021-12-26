struct Light {
    vec3 direction;
    vec3 color;
};


layout (std140) uniform Lights {
    Light light;
    //vec3 u_LightPosition;
    //vec3 u_LightColor;
};

const float PI = 3.14159265359;
const float Epsilon = 0.00001;
const vec3 Fdielectric = vec3(0.04);

// float DistributionGGX(vec3 N, vec3 H, float roughness)
// {
//     float a = roughness*roughness;
//     float a2 = a*a;
//     float NdotH = max(dot(N, H), 0.0);
//     float NdotH2 = NdotH*NdotH;

//     float nom   = a2;
//     float denom = (NdotH2 * (a2 - 1.0) + 1.0);
//     denom = PI * denom * denom;

//     return nom / denom;
// }

// float GeometrySchlickGGX(float NdotV, float roughness)
// {
//     float r = (roughness + 1.0);
//     float k = (r*r) / 8.0;

//     float nom   = NdotV;
//     float denom = NdotV * (1.0 - k) + k;

//     return nom / denom;
// }

// float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
// {
//     float NdotV = max(dot(N, V), 0.0);
//     float NdotL = max(dot(N, L), 0.0);
//     float ggx2 = GeometrySchlickGGX(NdotV, roughness);
//     float ggx1 = GeometrySchlickGGX(NdotL, roughness);

//     return ggx1 * ggx2;
// }

// vec3 fresnelSchlick(float cosTheta, vec3 F0)
// {
//     return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
// }



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