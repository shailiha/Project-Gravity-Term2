// Palm CG vertex and fragment program - For Hydrax-SkyX demo application
// Xavier Verguín González

void main_vp( // In
             float4 iPosition         : POSITION,
             float2 iUv               : TEXCOORD0,
             // Out
             out float4 oPosition     : POSITION,
             out float2 oDepth        : TEXCOORD0,
             out float2 oUv           : TEXCOORD1,
             // Uniform
             uniform float4x4 uWorld,
             uniform float4x4 uWorldViewProj)
{
   oPosition  = mul(uWorldViewProj, iPosition);
   oDepth     = oPosition.zw;
   oUv        = iUv;
}

void main_fp( // In
             float2 iDepth     : TEXCOORD0,
             float2 iUv        : TEXCOORD1,
             // Out
             out float4 oColor : COLOR,
             // Uniform
             uniform float3 uCameraPosition,
             uniform float  uFarClipDistance,
	         uniform sampler2D uTexture)
{
	float depth = iDepth.x/iDepth.y;
	oColor = float4(depth,0,0,tex2D(uTexture, iUv).w);
}


