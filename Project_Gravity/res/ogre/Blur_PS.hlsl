// Blur pass

sampler BlurTex: register(s0);

static const float samples[11] =
{
	0.01222447, // 0
	0.02783468, // 1
	0.06559061, // 2
	0.12097757, // 3
	0.17466632, // 4
	0.19741265, // 5
	0.17466632, // 6
	0.12097757, // 7
	0.06559061, // 8
	0.02783468, // 9
	0.01222447  // 10
};

static const float2 pos[11] =
{
#ifdef HORIZONTAL // Horizontal blur
	-5, 0, // 0
	-4, 0, // 1
	-3, 0, // 2
	-2, 0, // 3
	-1, 0, // 4
	 0, 0, // 5
	 1, 0, // 6
	 2, 0, // 7
	 3, 0, // 8
	 4, 0, // 9
	 5, 0, // 10
#else             // Vertical blur
 	 0, -5, // 0
	 0, -4, // 1
	 0, -3, // 2
	 0, -2, // 3
	 0, -1, // 4
	 0, 0, // 5
	 0, 1, // 6
	 0, 2, // 7
	 0, 3, // 8
	 0, 4, // 9
	 0, 5, // 10
#endif
};

float4 main(float2 texCoord: TEXCOORD0) : COLOR
{
   float4 sum = 0;
   for (int i = 0; i < 11; i++)
   {
      sum += tex2D(BlurTex, texCoord + pos[i]*0.015) * samples[i];
   }
   return sum;
}
