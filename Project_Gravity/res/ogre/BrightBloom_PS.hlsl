// Bright pass

float4 main(float2 texCoord: TEXCOORD0,
			uniform sampler RT: register(s0)
		) : COLOR 
{		
	float4 tex = tex2D(RT, texCoord);
	float minval = 1.1; // Only >1 values
	
	return pow(saturate(tex-minval),0.0001)*10;
}
