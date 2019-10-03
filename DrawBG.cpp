#include <windows.h>
#include <GL/glew.h>

//#include "DrawBG.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <GL/freeglut.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <sstream>

#include "InitShader.h"
#include "LoadTexture.h"

using namespace std;

#define BUFFER_OFFSET(i) ((char *)NULL + (i)) 

GLuint BackGround_shader;
static const string BackGround_vs("eye_view_vs.glsl");
static const string BackGround_fs("gaussian_fs.glsl");

static unsigned int BackGround_vao;
static unsigned int matrix_buffer;

GLuint BackGround_texture = -1;
//GLuint fbo_texture_bg = -1;
//GLuint stretch_fbo = -1;

void InitBackGroundShader()
{

	GLuint new_shader = InitShader(BackGround_vs.c_str(), BackGround_fs.c_str());
	if (new_shader == -1)
	{
		cout << "Loading Failed! BackGround shader." << endl;
	}
	else
	{
		if (BackGround_shader != -1)
		{
			glDeleteProgram(BackGround_shader);
		}
		BackGround_shader = new_shader;
	}

	string texture_name = "BackGround_texture.bmp";
	BackGround_texture = LoadTexture(texture_name.c_str());

	//glGenTextures(1, &fbo_texture_bg);
	//glBindTexture(GL_TEXTURE_2D, fbo_texture_bg);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glBindTexture(GL_TEXTURE_2D, 0);

	//glGenFramebuffers(1, &stretch_fbo);
	//glBindFramebuffer(GL_FRAMEBUFFER, stretch_fbo);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture_bg, 0);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CreateBackGroundVao()
{
	float startX = -1.f;
	float startY = -1.f;

	glUseProgram(BackGround_shader);

	int pos_loc = 0;
	const int tex_coord_loc = 1;

	vector<float> vertices;

	vertices.push_back(-1.f);
	vertices.push_back(-1.f);
	vertices.push_back(0.f);
	vertices.push_back(0.f);
	vertices.push_back(0.f);

	vertices.push_back(1.f);
	vertices.push_back(-1.f);
	vertices.push_back(0.f);
	vertices.push_back(1.f);
	vertices.push_back(0.f);

	vertices.push_back(-1.f);
	vertices.push_back(1.f);
	vertices.push_back(0.f);
	vertices.push_back(0.f);
	vertices.push_back(1.f);

	vertices.push_back(1.f);
	vertices.push_back(1.f);
	vertices.push_back(0.f);
	vertices.push_back(1.f);
	vertices.push_back(1.f);

	const int stride = 5 * sizeof(float);
	glGenVertexArrays(1, &BackGround_vao);
	glBindVertexArray(BackGround_vao);
	unsigned int vbo;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(pos_loc);
	glVertexAttribPointer(pos_loc, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(tex_coord_loc);
	glVertexAttribPointer(tex_coord_loc, 2, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(3 * sizeof(float)));

	glBindVertexArray(0);
}

void DisplayBackGround()
{
	glUseProgram(BackGround_shader);

	const int tex_loc = 2;

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUniform1f(5, 1 / float(glutGet(GLUT_WINDOW_WIDTH)));
	glUniform1f(6, 1 / float(glutGet(GLUT_WINDOW_HEIGHT)));

	//glUniform1i(10, 1);
	//glBindFramebuffer(GL_FRAMEBUFFER, stretch_fbo);
	//glDrawBuffer(GL_COLOR_ATTACHMENT0);
	//glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glBindTexture(GL_TEXTURE_2D, BackGround_texture);
	//glUniform1i(tex_loc, 0);
	//glBindVertexArray(BackGround_vao);
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//glUniform1i(10, 2);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glDrawBuffer(GL_BACK);
	//glActiveTexture(GL_TEXTURE0);
	
	glBindTexture(GL_TEXTURE_2D, BackGround_texture);
	glUniform1i(tex_loc, 0);
	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(BackGround_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}