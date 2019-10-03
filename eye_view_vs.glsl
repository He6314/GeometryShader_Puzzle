#version 430            

layout(location = 0) in vec3 pos_attrib;
layout(location = 1) in vec2 tex_coord_attrib;
layout(location = 2) in vec3 normal_attrib;

out vec2 tex_coord; 
out vec3 normal; 

void main(void)
{
	   gl_Position =  vec4(pos_attrib, 1.0);
	   tex_coord = tex_coord_attrib;
	   normal = normal_attrib;
}
