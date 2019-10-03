#include <windows.h>
#include <GL/glew.h>

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

#define NUM_BLOCK 5
#define BUFFER_OFFSET(i) ((char *)NULL + (i)) 

GLuint timer_shader[NUM_BLOCK];
static const string timer_vs("eye_view_vs.glsl");
static const string timer_fs("char_fs.glsl");

GLuint timer_texture[11] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

static unsigned int timer_vao[NUM_BLOCK];
static unsigned int matrix_buffer;

float timer_width = 0.08;
float timer_height = 0.15;

float startX = .95f - float(NUM_BLOCK)*timer_width;
float startY = .9f - timer_height;

void InitTimerShader()
{
	for (int i = 0; i < NUM_BLOCK; i++)
	{
		GLuint new_shader = InitShader(timer_vs.c_str(), timer_fs.c_str());
		if (new_shader == -1)
		{
			cout << "Loading Failed! Timer shader No." << i << endl;
		}
		else
		{
			if (timer_shader[i] != -1)
			{
				glDeleteProgram(timer_shader[i]);
			}
			timer_shader[i] = new_shader;
		}
	}	

	for (int i = 0; i < 11; i++)
	{
		stringstream filename;
		filename << "num_texture/" << i << ".bmp";
		string texture_name = filename.str();
		timer_texture[i] = LoadTexture(texture_name.c_str());
	}
}

void CreateTimerVao()
{	
	for (int i = 0; i < NUM_BLOCK; i++)
	{
		const int pos_loc = 0;
		const int tex_coord_loc = 1;

		vector<float> vertices;

		float offsetX = startX + float(i)*timer_width;
		float offsetY = startY;

		for (int i = 0; i < 2; i++)
		{
			float u = float(i);
			float x = u*timer_width + offsetX;

			for (int j = 0; j < 2; j++)
			{
				float v = float(j);
				float y = v*timer_height + offsetY;

				vertices.push_back(x);
				vertices.push_back(y);
				vertices.push_back(0.f);
				vertices.push_back(u);
				vertices.push_back(v);
			}
		}

		const int stride = 5 * sizeof(float);
		glGenVertexArrays(1, &timer_vao[i]);
		glBindVertexArray(timer_vao[i]);
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
}

void DisplayTimer(int* timeArray)
{
	for (int i = 0; i < NUM_BLOCK; i++)
	{
		glUseProgram(timer_shader[i]);

		const int tex_loc = 2;
		
		if (i != 2) //calculate the number showing on each digit of the timer
		{
			int digit = i;
			if (digit > 2)
				digit--;
			glBindTexture(GL_TEXTURE_2D, timer_texture[timeArray[digit]]);
		}
		else
			glBindTexture(GL_TEXTURE_2D, timer_texture[10]);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUniform1i(tex_loc, 0);
		glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBindVertexArray(timer_vao[i]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);;
	}
}