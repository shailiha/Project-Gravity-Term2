// Blend pass

float4 main(float2 texCoord: TEXCOORD0,
			uniform sampler RT: register(s0),
			uniform sampler BlurTex: register(s1),
			uniform float uBloomStrength
		) : COLOR 
{
	float4 Original = tex2D(RT,   texCoord);
	float4 Blur  = tex2D(BlurTex, texCoord);

	return Original + uBloomStrength*Blur;
}









