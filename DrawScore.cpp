#include <windows.h>
#include <GL/glew.h>

#include "Drawscore.h"
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

GLuint score_shader[NUM_BLOCK];
static const string score_vs("eye_view_vs.glsl");
static const string score_fs("char_fs.glsl");

GLuint score_texture[11] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 };
static unsigned int score_vao[NUM_BLOCK];
static unsigned int matrix_buffer;

const float score_width = 0.06;
const float score_height = 0.15;

void InitScoreShader()
{
	for (int i = 0; i < NUM_BLOCK; i++)
	{
		GLuint new_shader = InitShader(score_vs.c_str(), score_fs.c_str());
		if (new_shader == -1)
		{
			cout << "Loading Failed! score shader No." << i << endl;
		}
		else
		{
			if (score_shader[i] != -1)
			{
				glDeleteProgram(score_shader[i]);
			}
			score_shader[i] = new_shader;
		}
	}

	for (int i = 0; i < 11; i++)
	{
		stringstream filename;
		filename << "score_texture/" << i << ".bmp";
		string texture_name = filename.str();
		score_texture[i] = LoadTexture(texture_name.c_str());
	}
}

void CreateScoreVao()
{
	float startX = -.95f;
	float startY = .9f - score_height;

	for (int i = 0; i < NUM_BLOCK; i++)
	{
		//glUseProgram(score_shader[i]);

		const int pos_loc = 0;
		const int tex_coord_loc = 1;
		const int normal_loc = 2;
		vector<float> vertices;

		float offsetX = startX + float(i)*(score_width + 0.02);
		float offsetY = startY;

		for (int i = 0; i < 2; i++)
		{
			float u = float(i);
			float x = u*score_width + offsetX;

			for (int j = 0; j < 2; j++)
			{
				float v = float(j);
				float y = v*score_height + offsetY;

				//push vertex coords
				vertices.push_back(x);
				vertices.push_back(y);
				vertices.push_back(0.f);

				//push tex coords
				vertices.push_back(u);
				vertices.push_back(v);

				//push vertex coords
				vertices.push_back(0.f);
				vertices.push_back(0.f);
				vertices.push_back(1.f);
			}
		}

		const int stride = 8 * sizeof(float);
		glGenVertexArrays(1, &score_vao[i]);
		glBindVertexArray(score_vao[i]);
		unsigned int vbo;

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(pos_loc);
		glVertexAttribPointer(pos_loc, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0));

		glEnableVertexAttribArray(tex_coord_loc);
		glVertexAttribPointer(tex_coord_loc, 2, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(3 * sizeof(float)));

		glEnableVertexAttribArray(normal_loc);
		glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(5 * sizeof(float)));

		vector<unsigned int> indices;

		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(3);

		unsigned int ibo;
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		glBindVertexArray(0);
	}
}

void DisplayScore(int score)
{
	for (int i = 0; i < NUM_BLOCK; i++)
	{
		glUseProgram(score_shader[i]);

		const int tex_loc = 2;

		int digit = score / pow(10, NUM_BLOCK - 1 - i);
		int number = digit - digit / 10 * 10;
	    glBindTexture(GL_TEXTURE_2D, score_texture[number]);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUniform1i(tex_loc, 0);
		glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBindVertexArray(score_vao[i]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);;
	}
}