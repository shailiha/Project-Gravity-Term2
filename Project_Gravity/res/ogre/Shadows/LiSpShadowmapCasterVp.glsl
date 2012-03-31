uniform mat4 worldViewProj;
uniform vec4 texelOffsets;

varying vec2 depth;

void main()
{
	gl_Position = ftransform();

	// fix pixel / texel alignment
	gl_Position.xy += texelOffsets.zw * gl_Position.w;
	// linear depth storage
	// offset / scale range output
	depth.x = gl_Position.z;
	depth.y = gl_Position.w;
}

