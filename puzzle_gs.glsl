#version 430

layout(location = 0) uniform mat4 PVM;
layout(location = 1) uniform int pass;
layout(location = 10) uniform mat4 P;
layout(location = 11) uniform mat4 V;
layout(location = 12) uniform mat4 M;

layout(location = 20) uniform vec4 CameraPos;
layout(location = 21) uniform vec4 ConverPos;

layout(location = 50) uniform float level;

layout(location = 4) uniform float time;
layout(location = 5) uniform int startFlag = 0;

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec2 tex_coord_vs[];

out vec2 tex_coord_gs;
out float success;
//=======================================================
float randomFrom2D(vec2 st);
float randomFrom1D(float x);
float random();
float DiffMat4(mat4 A, mat4 B);
mat4 calculateM();

//=======================================================
mat4 M0 = mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
float Pi = 3.1415926535898;

void main()
{

    mat4 gamingM = M;
	success = 0.0;

	float sum = DiffMat4(gamingM, M0);
    if(sum<0.25)
	{    
	     gamingM = M0;
		 success = 1.0;
	}
    
	vec4 ConverPos = gamingM * CameraPos;

    float amp = (randomFrom2D(gl_in[0].gl_Position.xy) - .5f)* (1.1-level/10.0);

if(pass==1)
{
	for(int n=0; n<gl_in.length(); ++n)
	{
	    amp += (randomFrom2D(gl_in[n].gl_Position.xy)* .1f)*(0.9/level+0.1*level);

		gl_Position = gamingM * gl_in[n].gl_Position;		
		vec4 viewLine =  normalize(ConverPos - gl_Position);
		vec4 offset = viewLine * amp;
		gl_Position += offset;

		gl_Position = P*V*gl_Position;

		tex_coord_gs = tex_coord_vs[n];
		EmitVertex();
	}
	EndPrimitive();
}

else
{   
	for(int n=0; n<gl_in.length(); ++n)
	{
		gl_Position = gl_in[n].gl_Position;
		tex_coord_gs = tex_coord_vs[n];
		EmitVertex();
	}
	EndPrimitive();
}
}
//////////////////////////////////////////////////////
/////          SELF CONSTRUCT FUNCTIONS         //////
//////////////////////////////////////////////////////
float DiffMat4(mat4 A, mat4 B)
{
	mat4 diffMat = A-B;
	float sum = 0;
	for(int i=0;i<4;i++)
	for(int j=0;j<4;j++)
	    sum+=abs(diffMat[i][j]);
	return sum;
}

float randomFrom2D(vec2 st) {
    return fract(sin(dot(st, vec2(12.9898,78.233)))*43758.5453123);
}

float randomFrom1D(float x) {
    return fract(sin(dot(vec2(fract(time),x), vec2(66.67,99.81)))*47906.4607296);
}

float random() {
    return fract(sin(dot(vec2(sin(time),cos(time)), vec2(33.22,22.33)))*38324.2224717);
}

mat4 calculateM()
{
     mat4 Mx;
	 mat4 My;

	 if(startFlag==1)
	 {
	     float x = random()*Pi;
		 float y = randomFrom1D(x)*Pi;

		 Mx = mat4(1,0,0,0,
		           0,sin(x),cos(x),0,
				   0,-cos(x),sin(x),0,
				   0,0,0,1);
		 My = mat4(sin(y),0,cos(y),0,
		           0,1,0,0,
				   -cos(y),0,sin(y),0,
				   0,0,0,1);
		 return Mx*My;
	  }
	  return M;
}
