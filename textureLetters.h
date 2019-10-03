#pragma once

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

#define BUFFER_OFFSET(i) ((char *)NULL + (i)) 

static const string letter_vs("eye_view_vs.glsl");
static const string letter_fs("char_fs.glsl");

class textureLetters
{
public:
	textureLetters(int number = 0, float inputW = 0.08, float inputH = 0.15, float posX = -1.1, float posY = -1.1)
	{
		if (number > 0)
			NUM_BLOCK = number;
		else
			NUM_BLOCK = 5;

		width = inputW;
		height = inputH;

		if (posX < -1.0)
			startX = .95f - float(NUM_BLOCK)*width;
		else
			startX = posX;

		if (posY <= -1.0)
			startY = .9f - height;
		else
			startY = posY;
	}

	void Init()
	{
		for (int i = 0; i < NUM_BLOCK; i++)
		{
			GLuint new_shader = InitShader(letter_vs.c_str(), letter_fs.c_str());
			if (new_shader == -1)
			{
				cout << "Loading Failed! Timer shader No." << i << endl;
				letter_shader.push_back(-1);
			}
			else
			{
				//if (letter_shader[i] != -1)
				{
					//glDeleteProgram(letter_shader[i]);
				}
				letter_shader.push_back(new_shader);
			}
			letter_vao.push_back(-1);
		}

		for (int s = 0; s < 3; s++)
		{
			for (int i = 0; i < 41; i++)
			{
				stringstream filename;

				char letter;

				switch (i)
				{
				case 0: letter = '0'; break;
				case 1: letter = '1'; break;
				case 2: letter = '2'; break;
				case 3: letter = '3'; break;
				case 4: letter = '4'; break;
				case 5: letter = '5'; break;
				case 6: letter = '6'; break;
				case 7: letter = '7'; break;
				case 8: letter = '8'; break;
				case 9: letter = '9'; break;

				case 10: letter = 'A'; break;
				case 11: letter = 'B'; break;
				case 12: letter = 'C'; break;
				case 13: letter = 'D'; break;
				case 14: letter = 'E'; break;
				case 15: letter = 'F'; break;
				case 16: letter = 'G'; break;
				case 17: letter = 'H'; break;
				case 18: letter = 'I'; break;
				case 19: letter = 'J'; break;
				case 20: letter = 'K'; break;
				case 21: letter = 'L'; break;
				case 22: letter = 'M'; break;
				case 23: letter = 'N'; break;
				case 24: letter = 'O'; break;
				case 25: letter = 'P'; break;
				case 26: letter = 'Q'; break;
				case 27: letter = 'R'; break;
				case 28: letter = 'S'; break;
				case 29: letter = 'T'; break;
				case 30: letter = 'U'; break;
				case 31: letter = 'V'; break;
				case 32: letter = 'W'; break;
				case 33: letter = 'X'; break;
				case 34: letter = 'Y'; break;
				case 35: letter = 'Z'; break;

				case 36: letter = '!'; break;
				case 37: letter = '?'; break;

				case 38: letter = '#'; break;
				case 39: letter = '.'; break;
				case 40: letter = '%'; break;
				default:
					break;
				}

				filename << "letters" << s + 1 << "/";
				if (letter == '?')
					filename << "!!.bmp";
				else if (letter == '.')
					filename << "##.bmp";
				else
					filename << letter << ".bmp";


				string texture_name = filename.str();
				letter_texture[s][i] = LoadTexture(texture_name.c_str());
			}
		}
	}

	void CreateVao()
	{
		//contents = inputS;
		//if (inputS.length() > NUM_BLOCK)
		//{
		//	for (int i = 0; i < inputS.length() - NUM_BLOCK; i++)
		//	{
		//		GLuint new_shader = InitShader(letter_vs.c_str(), letter_fs.c_str());
		//		if (new_shader == -1)
		//		{
		//			cout << "Loading Failed! Timer shader No." << i << endl;
		//			letter_shader.push_back(-1);
		//		}
		//		else
		//		{
		//			//if (letter_shader[i] != -1)
		//			{
		//				//glDeleteProgram(letter_shader[i]);
		//			}
		//			letter_shader.push_back(new_shader);
		//		}
		//		letter_vao.push_back(-1);
		//	}
		//	NUM_BLOCK = inputS.length();
		//}
		//if (inputS.length() < NUM_BLOCK)
		//{
		//	for (int i = 0; i < NUM_BLOCK - inputS.length(); i++)
		//	{
		//		letter_shader.pop_back();
		//		letter_vao.pop_back();
		//	}
		//	NUM_BLOCK = inputS.length();
		//}

		for (int i = 0; i < NUM_BLOCK; i++)
		{
			const int pos_loc = 0;
			const int tex_coord_loc = 1;

			vector<float> vertices;

			float offsetX = startX + float(i)*width;
			float offsetY = startY;

			for (int i = 0; i < 2; i++)
			{
				float u = float(i);
				float x = u*width + offsetX;

				for (int j = 0; j < 2; j++)
				{
					float v = float(j);
					float y = v*height + offsetY;

					vertices.push_back(x);
					vertices.push_back(y);
					vertices.push_back(0.f);
					vertices.push_back(u);
					vertices.push_back(v);
				}
			}

			const int stride = 5 * sizeof(float);
			glGenVertexArrays(1, &letter_vao[i]);
			glBindVertexArray(letter_vao[i]);
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

	void Display(string inputS, int style)
	{
		contents = inputS;
		for (int i = 0; i < NUM_BLOCK; i++)
		{
			glUseProgram(letter_shader[i]);
			const int tex_loc = 2;

			for (int i = 0; i < contents.length(); i++)
			{
				stringstream filename;

				int digit;
				switch (contents[i])
				{
				case'0': digit = 0; break;
				case'1': digit = 1; break;
				case'2': digit = 2; break;
				case'3': digit = 3; break;
				case'4': digit = 4; break;
				case'5': digit = 5; break;
				case'6': digit = 6; break;
				case'7': digit = 7; break;
				case'8': digit = 8; break;
				case'9': digit = 9; break;

				case 'A':case 'a': digit = 10; break;
				case 'B':case 'b': digit = 11; break;
				case 'C':case 'c': digit = 12; break;
				case 'D':case 'd': digit = 13; break;
				case 'E':case 'e': digit = 14; break;
				case 'F':case 'f': digit = 15; break;
				case 'G':case 'g': digit = 16; break;
				case 'H':case 'h': digit = 17; break;
				case 'I':case 'i': digit = 18; break;
				case 'J':case 'j': digit = 19; break;
				case 'K':case 'k': digit = 20; break;
				case 'L':case 'l': digit = 21; break;
				case 'M':case 'm': digit = 22; break;
				case 'N':case 'n': digit = 23; break;
				case 'O':case 'o': digit = 24; break;
				case 'P':case 'p': digit = 25; break;
				case 'Q':case 'q': digit = 26; break;
				case 'R':case 'r': digit = 27; break;
				case 'S':case 's': digit = 28; break;
				case 'T':case 't': digit = 29; break;
				case 'U':case 'u': digit = 30; break;
				case 'V':case 'v': digit = 31; break;
				case 'W':case 'w': digit = 32; break;
				case 'X':case 'x': digit = 33; break;
				case 'Y':case 'y': digit = 34; break;
				case 'Z':case 'z': digit = 35; break;

				case '!': digit = 36; break;
				case '?': digit = 37; break;

				case '#': digit = 38; break;
				case '.': digit = 39; break;
				case ' ': digit = 40; break;
				default: digit = 37;
					break;
				}

				glBindTexture(GL_TEXTURE_2D, letter_texture[style - 1][digit]);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glUniform1i(tex_loc, 0);
				glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

				glDepthMask(GL_FALSE);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				glBindVertexArray(letter_vao[i]);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

				glDepthMask(GL_TRUE);
				glDisable(GL_BLEND);;
			}
		}
	}

	~textureLetters()
	{
		letter_shader.clear();
		letter_vao.clear();
	}

private:

	string contents;

	vector<GLuint> letter_shader;
	vector<unsigned int> letter_vao;

	int NUM_BLOCK = 5;

	GLuint letter_texture[3][41];

	float width;
	float height;
	float startX;
	float startY;
};