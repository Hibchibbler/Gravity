///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////
uniform vec2 view;
uniform vec2 position;
uniform vec2 resolution;
uniform sampler2D texture;

void main()
{
	vec4 pixelColor = texture2D(texture, gl_TexCoord[0].xy);
    vec4 solidColor = vec4(0.01,0.01,0.01,1.0);
	vec2 texCoord = gl_TexCoord[0].xy;
	//float argleR = (noise1(0.75) + 1.0) / 2.0;// a random value scaled to be between 0, 1.
	
	//
	// The further from position, the darker we make the pixel.
	// Specifically, we have a cubic fal off - stays light for a bit, then gets dark fast.
	//
	vec2 dist = distance(gl_FragCoord.xy, position.xy) / view.xy;
	float len = length(dist);
	float cubicFalloff = len*len*len;	
	vec4 pixel = texture2D(texture, texCoord.xy) * gl_Color;	
	gl_FragColor = pixel - (13.20*cubicFalloff);
}