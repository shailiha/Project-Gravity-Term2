// Island CG vertex and fragment program - For Hydrax-SkyX demo application
// Xavier Verguín González - Xavyiy

void main_vp( // In
             float4 iPosition         : POSITION,
             float3 iNormal           : NORMAL,
             float2 iUv               : TEXCOORD0,
             float3 iTangent          : TANGENT0,
             // Out
             out float4 oPosition     : POSITION,
             out float2 oUv           : TEXCOORD0,
             out float3 oLightDir     : TEXCOORD1, 
             out float3 oEyeDir       : TEXCOORD2, 
             out float3 oHalfAngle    : TEXCOORD3,
             out float  oYPos         : TEXCOORD4,
#ifdef SHADOWS
             out float4 oShadowUV     : TEXCOORD5,
#endif
             // Uniform
             uniform float4   uLightPosition, 
             uniform float3   uEyePosition,
             uniform float4x4 uWorldViewProj,
             uniform float    uTexturesScale
#ifdef SHADOWS
            ,uniform float4x4 uShadowTextProj
#endif
											 )
{
   // Just a little hack, TerrainSceneManager doesn't builds tangent vectors
   iTangent = 1;
   
   oPosition = mul(uWorldViewProj, iPosition);
   oUv       = iUv*uTexturesScale;

   float3 lightDir = normalize(uLightPosition.xyz -  (iPosition.xyz * uLightPosition.w));
   float3 eyeDir   = uEyePosition - iPosition.xyz;

   float3 binormal   = cross(iTangent, iNormal);
   float3x3 rotation = float3x3(iTangent, binormal, iNormal);

   lightDir = normalize(mul(rotation, lightDir));
   eyeDir   = normalize(mul(rotation, eyeDir));

   oLightDir  = lightDir;
   oEyeDir    = eyeDir;
   oHalfAngle = normalize(eyeDir + lightDir);

   oYPos = iPosition.y/180; //[0,1] range
#ifdef SHADOWS
   oShadowUV = mul(uShadowTextProj, iPosition);
#endif
}

float3 expand(float3 v)
{
	return (v - 0.5) * 2;
}

float3 calculate_colour(sampler2D uNormalHeightMap, 
                        sampler2D uDiffuseMap, 
                        float2    iUv,
                        float3    iEyeDir,
                        float3    iLightDir,
                        float3    iHalfAngle,
                        float3    uLightDiffuse,
	                    float3    uLightSpecular,
                        float4    uScaleBias,
                        float     _GreenFade)
{
    // get the height using the tex coords
	float height = tex2D(uNormalHeightMap, iUv).a;

	// calculate displacement
	float displacement = (height * uScaleBias.x) + uScaleBias.y;

	// calculate the new tex coord to use for normal and diffuse
	float2 newTexCoord = ((iEyeDir.xy * displacement) + iUv).xy;

	// get the new normal and diffuse values
	float3 normal = expand(tex2D(uNormalHeightMap, newTexCoord).xyz);
	float3 diffuse = tex2D(uDiffuseMap, newTexCoord).xyz;
	
	if (_GreenFade>0.6)
	{
	    float d = (_GreenFade-0.6)/3;
	    diffuse += float3(-d,d,-d);
	}

	float3 specular = pow(saturate(dot(normal, iHalfAngle)), 10) * uLightSpecular;
	float diff = saturate(dot(normal, iLightDir));
	
	if (diff<0.5)
	{
	   diff = 0.5;
	}

	return diffuse * diff * uLightDiffuse + specular;
}

void main_fp( // In
             float2 iUv        : TEXCOORD0,
	         float3 iLightDir  : TEXCOORD1,
	         float3 iEyeDir    : TEXCOORD2,
	         float3 iHalfAngle : TEXCOORD3,
             float  iYPos      : TEXCOORD4,
#ifdef SHADOWS
             float4 iShadowUV  : TEXCOORD5,
#endif
             // Out
             out float4 oColor : COLOR,
             // Uniform
	         uniform float3 uLightDiffuse,
	         uniform float3 uLightSpecular,
	         uniform float4 uScaleBias,
	         uniform sampler2D uNormalHeightMap1,
	         uniform sampler2D uDiffuseMap1,
             uniform sampler2D uNormalHeightMap2,
	         uniform sampler2D uDiffuseMap2
#ifdef SHADOWS
	        ,uniform sampler2D uShadowMap,
	         uniform sampler2D uNoise
#endif
									 )
{
#ifdef SHADOWS
	// Shadows
	float shadow = tex2D(uShadowMap, iShadowUV.xy / iShadowUV.w + (1-2*tex2D(uNoise, iUv*10).xy)*0.002).r;
	
	if (iShadowUV.z / iShadowUV.w > shadow)
		shadow = 0; // Shadow
	else
		shadow = 1; // No shadow
		
	float shadow_opacity = uLightDiffuse.r*0.3;
	float minus_shadow_opacity = 1-shadow_opacity;
	
	// Attenuate specular lighting on shadowed areas
	uLightSpecular *= 0.25+0.75*shadow;
#endif

	float3 col1 = calculate_colour(uNormalHeightMap1, 
	                               uDiffuseMap1,
	                               iUv,
                                   iEyeDir,
                                   iLightDir,
                                   iHalfAngle,
                                   uLightDiffuse,
	                               uLightSpecular,
                                   uScaleBias,
                                   0);
                                   
    float3 col2 = calculate_colour(uNormalHeightMap2, 
	                               uDiffuseMap2,
	                               iUv,
                                   iEyeDir,
                                   iLightDir,
                                   iHalfAngle,
                                   uLightDiffuse,
	                               uLightSpecular,
                                   uScaleBias,
                                   iYPos);
	
	oColor = float4(saturate(lerp(col1,col2,iYPos)), 1);

#ifdef SHADOWS
	oColor.xyz *= minus_shadow_opacity+shadow_opacity*shadow;
#endif
}


