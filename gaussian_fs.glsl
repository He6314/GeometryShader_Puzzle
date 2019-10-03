#version 430

layout(location = 2) uniform sampler2D color_tex;
//layout(location = 10) uniform int pass=1;

layout(location = 5) uniform float pixWidth = 0.0;
layout(location = 6) uniform float pixHeight = 0.0;

in vec2 tex_coord;
layout(location = 0) out vec4 fragcolor;  

float gaussian5[5][5] = 
               { {0.0392, 0.0398, 0.0400, 0.0398, 0.0392},
                 {0.0398, 0.0404, 0.0406, 0.0404, 0.0398},
				 {0.0400, 0.0406, 0.0408, 0.0406, 0.0400},
				 {0.0398, 0.0404, 0.0406, 0.0404, 0.0398},
				 {0.0392, 0.0398, 0.0400, 0.0398, 0.0392}};
float gaussian7[7][7] = {
{0.0000006,	0.0000229,	0.0001911,	0.0003877,	0.0001911,	0.0000229,	0.0000006},
{0.0000229,	0.0007863,	0.0065596,	0.0133037,	0.0065596,	0.0007863,	0.0000229},
{0.0001911,	0.0065596,	0.0547215,	0.1109816,	0.0547215,	0.0065596,	0.0001911},
{0.0003877,	0.0133037,	0.1109816,	0.2250855,	0.1109816,	0.0133037,	0.0003877},
{0.0001911,	0.0065596,	0.0547215,	0.1109816,	0.0547215,	0.0065596,	0.0001911},
{0.0000229,	0.0007863,	0.0065596,	0.0133037,	0.0065596,	0.0007863,	0.0000229},
{0.0000006,	0.0000229,	0.0001911,	0.0003877,	0.0001911,	0.0000229,	0.0000006},
};

void main(void)
{
    //if(pass==1)
	//{
	//    fragcolor = texture(color_tex, tex_coord);
	//} 

	//else if(pass==2)
	//{
		 vec4 pixRGB =  vec4(0);

		for(int i = 0; i<5; i++)
	    {	for(int j = 0; j<5; j++)
			{
			      vec4 component = texture(color_tex, tex_coord-vec2(2*pixWidth,2*pixHeight)+vec2(pixWidth*float(i), pixWidth*float(j)));//texelFetch(color_tex, ivec2(gl_FragCoord)-ivec2(2)+ivec2(i,j), 0);
				  pixRGB += gaussian5[i][j] * component;
			}
		}
		fragcolor =  pixRGB;

		float brightness = pixRGB.r*0.3 + pixRGB.g*0.6 + pixRGB.b*0.1;
		float diffRGB = abs(pixRGB.r- pixRGB.b)+abs( pixRGB.r- pixRGB.g)+abs( pixRGB.b- pixRGB.g);
		if(brightness>0.75f && diffRGB<0.4) 
			fragcolor.a = .0f;
	//}
}