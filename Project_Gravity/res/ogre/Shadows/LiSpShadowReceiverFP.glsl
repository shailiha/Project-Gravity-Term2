uniform float inverseShadowmapSize;
uniform float fixedDepthBias;
uniform float gradientClamp;
uniform float gradientScaleBias;

uniform sampler2D shadowMap;

void main()
{
	vec4 shadowUV = gl_TexCoord[0];
	// point on shadowmap
	shadowUV = shadowUV / shadowUV.w;
	float centerdepth = texture2D(shadowMap, shadowUV.xy).x;
    
    // gradient calculation
  	float pixeloffset = inverseShadowmapSize;
    vec4 depths = vec4(
    	texture2D(shadowMap, shadowUV.xy + vec2(-pixeloffset, 0)).x,
    	texture2D(shadowMap, shadowUV.xy + vec2(+pixeloffset, 0)).x,
    	texture2D(shadowMap, shadowUV.xy + vec2(0, -pixeloffset)).x,
    	texture2D(shadowMap, shadowUV.xy + vec2(0, +pixeloffset)).x);

	vec2 differences = abs( depths.yw - depths.xz );
	float gradient = min(gradientClamp, max(differences.x, differences.y));
	float gradientFactor = gradient * gradientScaleBias;

	// visibility function
	float depthAdjust = gradientFactor + (fixedDepthBias * centerdepth);
	float finalCenterDepth = centerdepth + depthAdjust;

	// shadowUV.z contains lightspace position of current object

	gl_FragColor = (finalCenterDepth > shadowUV.z) ? gl_Color : vec4(0,0,0,1);
}

