#version 430

layout(location = 1) uniform int pass;
layout(location = 2) uniform sampler2D color_tex;
layout(location = 4) uniform float time;

layout(location = 3) uniform int edgeDetection;

layout(location = 0) out vec4 fragcolor;  
         
in vec2 tex_coord_gs;
in float success;

void main(void)
{   
  if(pass==1)
	{	
		  vec4 pixRGB =  texture(color_tex, tex_coord_gs);
		  float brightness = pixRGB.r*0.3 + pixRGB.g*0.6 + pixRGB.b*0.1;
		  float diffRGB = abs(pixRGB.r- pixRGB.b)+abs( pixRGB.r- pixRGB.g)+abs( pixRGB.b- pixRGB.g);
		  fragcolor = texture(color_tex, tex_coord_gs);
		  if(brightness>0.75f && diffRGB<0.4) 
				fragcolor.a = 0.0f;
	}
    else if(pass==2)
    {
        fragcolor = vec4(success);
    }
 }