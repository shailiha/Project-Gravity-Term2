void main_vp( // In
             float4 iPosition         : POSITION,
             float3 iNormal           : NORMAL,
             float2 iUv               : TEXCOORD0,
             // Out
             out float4 oPosition     : SV_POSITION,
             out float3 oUv           : TEXCOORD0,
             out float3 oLightDir     : TEXCOORD1, 
             out float3 oHalfAngle    : TEXCOORD2,
		  out float4 oLightPosition	: TEXCOORD3,
             out float3  oNormal         : TEXCOORD4,

             // Uniform
             uniform float4   uLightPosition, 
             uniform float3   uEyePosition,
             uniform float4x4 uWorldViewProj,
             uniform float4x4 uTexWorldViewProj
											 )
{
   oPosition = mul(uWorldViewProj, iPosition);
   oUv.xy       = iUv;
   oUv.z		= oPosition.z;

   oLightDir = normalize(uLightPosition.xyz -  (iPosition.xyz * uLightPosition.w).xyz);
   float3 eyeDir   = normalize(uEyePosition - iPosition.xyz);
   oHalfAngle = normalize(eyeDir + oLightDir);

   oLightPosition = mul(uTexWorldViewProj, iPosition);
   oNormal = iNormal;
}

float shadowPCF(sampler2D shadowMap, float4 shadowMapPos, float2 offset)
{
	shadowMapPos = shadowMapPos / shadowMapPos.w;
	float2 uv = shadowMapPos.xy;
	float3 o = float3(offset, -offset.x) * 0.3f;

	// Note: We using 2x2 PCF. Good enough and is alot faster.
	float c =	(shadowMapPos.z <= tex2D(shadowMap, uv.xy - o.xy).r)>0 ? 1 : 0; // top left
	c +=		(shadowMapPos.z <= tex2D(shadowMap, uv.xy + o.xy).r)>0 ? 1 : 0; // bottom right
	c +=		(shadowMapPos.z <= tex2D(shadowMap, uv.xy + o.zy).r)>0 ? 1 : 0; // bottom left
	c +=		(shadowMapPos.z <= tex2D(shadowMap, uv.xy - o.zy).r)>0 ? 1 : 0; // top right
	//float c =	(shadowMapPos.z <= tex2Dlod(shadowMap, uv.xyyy - o.xyyy).r) ? 1 : 0; // top left
	//c +=		(shadowMapPos.z <= tex2Dlod(shadowMap, uv.xyyy + o.xyyy).r) ? 1 : 0; // bottom right
	//c +=		(shadowMapPos.z <= tex2Dlod(shadowMap, uv.xyyy + o.zyyy).r) ? 1 : 0; // bottom left
	//c +=		(shadowMapPos.z <= tex2Dlod(shadowMap, uv.xyyy - o.zyyy).r) ? 1 : 0; // top right
	return c / 4;
}

void main_fp( // In
		  float4 oPosition  : SV_POSITION,
             float3 iUv        : TEXCOORD0,
	         float3 iLightDir  : TEXCOORD1,
	         float3 iHalfAngle : TEXCOORD2,
             float4  iLightPosition : TEXCOORD3,
             float3 normal  : TEXCOORD4,
             // Out
             out float4 oColor : COLOR,

             // Uniform
		   uniform float4 invShadowMapSize0,
	         uniform sampler2D uLightDiffuse : register(s1),
	         uniform sampler2D uLightSpecular : register(s2),
	         uniform sampler2D uNormalHeightMap : register (s3),
	         uniform sampler2D uShadowMap : register (s0),
	         uniform float4 lightDiffuse,
		   uniform float4 lightSpecular,
		   uniform float4 ambient
									 )
{

	float shadowing = 1.0f;
	float4 splitColour;

	splitColour = float4(0.1, 0.1, 0, 1); //POSSIBLY CHANGE
	shadowing = shadowPCF(uShadowMap, iLightPosition, invShadowMapSize0.xy);

	float3 lightVec = normalize(iLightDir);
	float3 halfAngle = normalize(iHalfAngle);
	float4 diffuseColour = tex2D(uLightDiffuse, iUv.xy);
	float4 specularColour = tex2D(uLightSpecular, iUv.xy);
	float shininess = specularColour.w;
	specularColour.w = 1;
	float4 lighting = lit(dot(normal, lightVec), dot(normal, halfAngle), shininess * 128) * shadowing;
	
	oColor = (diffuseColour * clamp(ambient + lightDiffuse * lighting.y, 0, 1)) + (lightSpecular * specularColour * lighting.z);
	oColor.w = diffuseColour.w;
}



