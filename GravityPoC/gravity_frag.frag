///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////
uniform vec2 view;
uniform vec2 position;
uniform vec2 resolution;
uniform sampler2D texture;
uniform sampler2D losMask;

void main()
{

    vec4 solidColor = vec4(0.1,0.1,0.1,1.0);
	vec4 solidBlack = vec4(1.0,1.0,1.0,0.10);
	
	vec2 texCoord = gl_TexCoord[0].xy;
	//float argleR = (noise1(0.75) + 1.0) / 2.0;// a random value scaled to be between 0, 1.
	
	//
	// The further from position, the darker we make the pixel.
	// Specifically, we have a cubic fal off - stays light for a bit, then gets dark fast.
	//
	vec2 dist1 = distance(gl_FragCoord.xy, position.xy) / view.xy;
	float len1 = length(dist1);
	float cubicFalloff1 = len1*len1*len1;	
	vec4 pixel = texture2D(texture, texCoord.xy) * gl_Color;	
	vec4 maskColor = texture2D(losMask, gl_TexCoord[0].xy)* gl_Color;
	
	
	
	
	
	if (maskColor == vec4(1.0, 1.0, 1.0, 1.0))
	{
		gl_FragColor = pixel - (1.3*cubicFalloff1/len1);
	}
	else
	{
		gl_FragColor = pixel - (80.00*cubicFalloff1);	
	}
/* 	vec2 dist2 = distance(gl_FragCoord.xy, last_position.xy) / view.xy;
	float len2 = length(dist2);
	float cubicFalloff2 = len2*len2*len2;	
	gl_FragColor = gl_FragColor + (0.4*cubicFalloff2); */
}