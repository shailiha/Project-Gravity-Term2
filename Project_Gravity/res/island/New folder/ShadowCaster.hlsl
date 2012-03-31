// Palm CG vertex and fragment program - For Hydrax-SkyX demo application
// Xavier Verguín González

void main_vp( // In
             float4 iPosition         : POSITION,
             float2 iUv               : TEXCOORD0,
             // Out
             out float4 oPosition     : POSITION,
             out float2 oUv           : TEXCOORD0,
             out float4 oDepth        : TEXCOORD1,
		  out float4 oDepthDir   : TEXCOORD2,
             // Uniform
             uniform float4x4 uWorld,
             uniform float4x4 uWorldViewProj)
{
   oDepth = mul(iWorldView, iPosition);
   oDepthDir = mul(iWorldViewProj, iPosition);
   oPosition  = oDepthDir;
   oUv        = iUv;
}

void main_fp( // In
             float2 iUv        : TEXCOORD0,
             float4 iDepth     : TEXCOORD1,
	        float4 iDepthDir   : TEXCOORD2,
             // Out
             out float4 oColor : COLOR,
             // Uniform
             uniform float4 iDepthRange,
             uniform float iAlphaReject,
	         uniform sampler2D uTexture : TEXUNIT0)
{
//	float depth = iDepth.x-iDepth.y;
//	oColor = float4(depth,0,0,tex2D(uTexture, iUv).w);

   if(tex2D(dTex,iUV).a < iAlphaReject)
      discard;   

   float d = (length(iDepth.xyz) - iDepthRange.x) * iDepthRange.w;

   oColour = float4(d, d * d, iDepthDir.z, iDepthDir.z*iDepthDir.z);
}


