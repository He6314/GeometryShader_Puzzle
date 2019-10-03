#version 430

layout(location = 2) uniform sampler2D color_tex;
        
in vec2 tex_coord;
layout(location = 0) out vec4 fragcolor;  
void main(void)
{   
    vec4 pixRGB =  texture(color_tex, tex_coord);
	float brightness = pixRGB.r*0.3 + pixRGB.g*0.6 + pixRGB.b*0.1;
	float diffRGB = abs(pixRGB.r- pixRGB.b)+abs( pixRGB.r- pixRGB.g)+abs( pixRGB.b- pixRGB.g);
	fragcolor = texture(color_tex, tex_coord);

	if(brightness>0.75f && diffRGB<0.4) 
		fragcolor.a = .0f;
}