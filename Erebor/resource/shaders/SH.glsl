// https://github.com/kayru/Probulator

#define myT vec3
#define myL 2

#define SphericalHarmonicsTL(T, L) T[(L + 1)*(L + 1)]
#define SphericalHarmonics SphericalHarmonicsTL(myT, myL)
#define shSize(L) ((L + 1)*(L + 1))


const float PI  = 3.1415926535897932384626433832795;
const float PIH = 1.5707963267948966192313216916398;
const float sqrtPI = 1.7724538509055160272981674833411; //sqrt(PI)

SphericalHarmonics shZero() {
	SphericalHarmonics result;
	for (int i = 0; i < shSize(myL); ++i)
	{
		result[i] = myT(0.0);
	}
    return result;
}


void shScale(inout SphericalHarmonics sh, myT scale) {
	for (int i = 0; i < shSize(myL); ++i)
	{
		sh[i] *= scale;
	}
}

void shAddWeighted(inout SphericalHarmonics accumulatorSh, in SphericalHarmonics sh, myT weight)
{
	for (int i = 0; i < shSize(myL); ++i)
	{
		accumulatorSh[i] += sh[i] * weight;
	}
}

myT shDot(in SphericalHarmonics shA, in SphericalHarmonics shB)
{
	myT result = myT(0.0);
	for (int i = 0; i < shSize(myL); ++i)
	{
		result += shA[i] * shB[i];
	}
	return result;
}

SphericalHarmonics shEvaluate(vec3 p)
{
	// From Peter-Pike Sloan's Stupid SH Tricks
	// http://www.ppsloan.org/publications/StupidSH36.pdf
	// https://github.com/dariomanesku/cmft/blob/master/src/cmft/cubemapfilter.cpp#L130

	SphericalHarmonics result;

	float x = -p.x;
	float y = -p.y;
	float z = p.z;

	float x2 = x*x;
	float y2 = y*y;
	float z2 = z*z;

	float z3 = z2*z;

	float x4 = x2*x2;
	float y4 = y2*y2;
	float z4 = z2*z2;

	int i = 0;

	result[i++] =  myT( 1.0f/(2.0f*sqrtPI) );

	#if (myL >= 1)
		result[i++] = myT(-sqrt(3.0f/(4.0f*PI))*y );
		result[i++] = myT( sqrt(3.0f/(4.0f*PI))*z );
		result[i++] = myT(-sqrt(3.0f/(4.0f*PI))*x );        	
    #endif

	#if (myL >= 2)
		result[i++] = myT( sqrt(15.0f/(4.0f*PI))*y*x );
		result[i++] = myT(-sqrt(15.0f/(4.0f*PI))*y*z );
		result[i++] = myT( sqrt(5.0f/(16.0f*PI))*(3.0f*z2-1.0f) );
		result[i++] = myT(-sqrt(15.0f/(4.0f*PI))*x*z );
		result[i++] = myT( sqrt(15.0f/(16.0f*PI))*(x2-y2) );			
    #endif

	#if (myL >= 3)
		result[i++] = myT(-sqrt( 70.0f/(64.0f*PI))*y*(3.0f*x2-y2) );
		result[i++] = myT( sqrt(105.0f/ (4.0f*PI))*y*x*z );
		result[i++] = myT(-sqrt( 21.0f/(16.0f*PI))*y*(-1.0f+5.0f*z2) );
		result[i++] = myT( sqrt(  7.0f/(16.0f*PI))*(5.0f*z3-3.0f*z) );
		result[i++] = myT(-sqrt( 42.0f/(64.0f*PI))*x*(-1.0f+5.0f*z2) );
		result[i++] = myT( sqrt(105.0f/(16.0f*PI))*(x2-y2)*z );
		result[i++] = myT(-sqrt( 70.0f/(64.0f*PI))*x*(x2-3.0f*y2) );			
    #endif

	#if (myL >= 4)
		result[i++] = myT( 3.0f*sqrt(35.0f/(16.0f*PI))*x*y*(x2-y2) );
		result[i++] = myT(-3.0f*sqrt(70.0f/(64.0f*PI))*y*z*(3.0f*x2-y2) );
		result[i++] = myT( 3.0f*sqrt( 5.0f/(16.0f*PI))*y*x*(-1.0f+7.0f*z2) );
		result[i++] = myT(-3.0f*sqrt(10.0f/(64.0f*PI))*y*z*(-3.0f+7.0f*z2) );
		result[i++] = myT( (105.0f*z4-90.0f*z2+9.0f)/(16.0f*sqrtPI) );
		result[i++] = myT(-3.0f*sqrt(10.0f/(64.0f*PI))*x*z*(-3.0f+7.0f*z2) );
		result[i++] = myT( 3.0f*sqrt( 5.0f/(64.0f*PI))*(x2-y2)*(-1.0f+7.0f*z2) );
		result[i++] = myT(-3.0f*sqrt(70.0f/(64.0f*PI))*x*z*(x2-3.0f*y2) );
		result[i++] = myT( 3.0f*sqrt(35.0f/(4.0f*(64.0f*PI)))*(x4-6.0f*y2*x2+y4) );
    #endif

	return result;
}

myT shEvaluateDiffuse(SphericalHarmonics sh, vec3 direction) {

	SphericalHarmonics directionSh = shEvaluate(direction);
	// https://cseweb.ucsd.edu/~ravir/papers/envmap/envmap.pdf equation 8

	const float A[5] = float[5](
		 1.0,
		 2.0 / 3.0,
		 1.0 / 4.0,
		 0.0,
		-1.0f / 24.0
	);

	int i = 0;

	myT result = sh[i] * directionSh[i] * A[0]; ++i;

	#if (myL >= 1)
		result += sh[i] * directionSh[i] * A[1]; ++i;
		result += sh[i] * directionSh[i] * A[1]; ++i;
		result += sh[i] * directionSh[i] * A[1]; ++i;
	#endif

	#if (myL >= 2)
		result += sh[i] * directionSh[i] * A[2]; ++i;
		result += sh[i] * directionSh[i] * A[2]; ++i;
		result += sh[i] * directionSh[i] * A[2]; ++i;
		result += sh[i] * directionSh[i] * A[2]; ++i;
		result += sh[i] * directionSh[i] * A[2]; ++i;
	#endif

	// L3 and other odd bands > 1 have 0 factor

	#if (myL >= 4)
		i = 16;

		result += sh[i] * directionSh[i] * A[4]; ++i;
		result += sh[i] * directionSh[i] * A[4]; ++i;
		result += sh[i] * directionSh[i] * A[4]; ++i;
		result += sh[i] * directionSh[i] * A[4]; ++i;
		result += sh[i] * directionSh[i] * A[4]; ++i;
		result += sh[i] * directionSh[i] * A[4]; ++i;
		result += sh[i] * directionSh[i] * A[4]; ++i;
		result += sh[i] * directionSh[i] * A[4]; ++i;
		result += sh[i] * directionSh[i] * A[4]; ++i;
	#endif

	return result;   
}


///////////////////////////////////////////////////////////////////////////////////////

#define reflectTex iChannel0
///////////////////////////////////////////////////////////////////////////////////////

const float goldenAngle = 2.3999632297286533222315555066336; // PI * (3.0 - sqrt(5.0));
const vec3 LUMA = vec3(0.2126, 0.7152, 0.0722);

vec3 SpherePoints_GoldenAngle(float i, float numSamples) {
    float theta = i * goldenAngle;
    float z = (1.0 - 1.0 / numSamples) * (1.0 - 2.0 * i / (numSamples - 1.0));
    float radius = sqrt(1.0 - z * z);
    return vec3(radius * vec2(cos(theta), sin(theta)), z);
}

vec3 sampleReflectionMap(vec3 p, float b) {
    vec3 col = textureLod(reflectTex, p, b).rgb;
    
    // fake HDR
    //col *= 1.0 + 1.0 * smoothstep(0.5, 1.0, dot(LUMA, col));
    
    return col;
}

#define ENV_SMPL_NUM 256
SphericalHarmonics CubeMapToRadianceSH() {
    // Initialise sh to 0
    SphericalHarmonics shRadiance = shZero();

    vec2 ts = vec2(textureSize(reflectTex, 0));
    float maxMipMap = log2(max(ts.x, ts.y));

    float lodBias = maxMipMap - 5.0;
    

    for (int i=0; i < ENV_SMPL_NUM; ++i) {
        vec3 direction = SpherePoints_GoldenAngle(float(i), float(ENV_SMPL_NUM));
        vec3 radiance = sampleReflectionMap(direction, lodBias);
        shAddWeighted(shRadiance, shEvaluate(direction), radiance);
    }

    // integrating over a sphere so each sample has a weight of 4*PI/samplecount (uniform solid angle, for each sample)
    float shFactor = 4.0 * PI / float(ENV_SMPL_NUM);
    shScale(shRadiance, vec3(shFactor));

    return shRadiance;
}

vec3 GetRadianceFromSH(SphericalHarmonics shRadiance, vec3 direction) {
    SphericalHarmonics shDirection = shEvaluate(direction);

    vec3 sampleSh = max(vec3(0.0), shDot(shRadiance, shDirection));    
    return sampleSh;
}

vec3 GetIrradianceFromSH(SphericalHarmonics shRadiance, vec3 direction) {
    SphericalHarmonics shDirection = shEvaluate(direction);

    vec3 sampleIrradianceSh = max(vec3(0.0), shEvaluateDiffuse(shRadiance, direction));
    return sampleIrradianceSh;
}

//////////////////////////////////////////////////////////////
#define NORM2SNORM(value) (value * 2.0 - 1.0)
#define SNORM2NORM(value) (value * 0.5 + 0.5)

vec3 EquirectToDirection(vec2 uv) {
    uv = NORM2SNORM(uv);
    uv.x *= PI;  // phi
    uv.y *= PIH; // theta
        
    /* Calculate a direction from spherical coords:
	** R = 1
    ** x = R sin(phi) cos(theta)
	** y = R sin(phi) sin(theta)
	** z = R cos(phi)
	*/
    return vec3(cos(uv.x)*cos(uv.y)
              , sin(uv.y)
              , sin(uv.x)*cos(uv.y));
}

#define ENV_SMPL_NUM2 1024
vec3 DiffuseImportanceSampling(in vec3 N, float power, bool lumaAsWeight) {
    vec3 iblDiffuse = vec3(0.0);

    float sum = 0.0;

    vec2 ts = vec2(textureSize(reflectTex, 0));
    float maxMipMap = log2(max(ts.x, ts.y));

    float lodBias = maxMipMap - 5.0;

    for (int i=0; i < ENV_SMPL_NUM2; ++i) {
        vec3 sp = SpherePoints_GoldenAngle(float(i), float(ENV_SMPL_NUM2));

        vec3 iblD = sampleReflectionMap(sp, lodBias); 

        float w = SNORM2NORM(dot(sp, N ));

        w = pow(w, power); //pow exponent is responsible for sharpness of the resulting convolution
        
        if (lumaAsWeight)
        	w *= dot(LUMA, iblD);

        iblDiffuse  += iblD * w;

        sum += w;
    }

    iblDiffuse  /= sum;
    return iblDiffuse;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = fragCoord/iResolution.xy;

    vec3 direction = EquirectToDirection(uv);
    
    SphericalHarmonics shRadiance = CubeMapToRadianceSH();
    
    vec3 col;
    
    int m = int(iTime) % 7;
    if (m == 0)
    	col = sampleReflectionMap(direction, 0.0);
    else if (m == 1)
    	col = GetRadianceFromSH(shRadiance, direction);
    else if (m == 2)
        col = GetIrradianceFromSH(shRadiance, direction);
	else if (m == 3)         
        col = DiffuseImportanceSampling(direction, 2.0, false);
	else if (m == 4)
        col = DiffuseImportanceSampling(direction, 2.0, true);
	else if (m == 5)
        col = DiffuseImportanceSampling(direction, 64.0, false);        
    else if (m == 6)
        col = textureLod(iChannel1, direction, 0.0).rgb;


    // Output to screen
    fragColor = vec4(col,1.0);
}