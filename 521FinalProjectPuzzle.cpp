#include <windows.h>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>

#include <iostream>
#include <fstream> 
#include <sstream>
#include <iomanip>  

#include "LoadTexture.h"

#include "InitShader.h"
#include "imgui_impl_glut.h"
#include "VideoMux.h"
#include "DebugCallback.h"

#include "DrawPlane.h"
#include "DrawTimer.h"
#include "DrawScore.h"
#include "DrawBG.h"

#include "textureLetters.h"

#include <mmsystem.h>

//https://www.nintendo.co.jp/titles/50010000013992

//names of the shader files to load
static const std::string vertex_shader("puzzle_vs.glsl");
static const std::string geometry_shader("puzzle_gs.glsl");
static const std::string fragment_shader("puzzle_fs.glsl");
GLuint shader_program = -1;

//Full screen quad in a VAO
GLuint quad_vao = -1;

//Texture which we will render into

GLuint texture_id = -1;
std::string texture_name = "patterns/1/1.bmp";

GLuint fbo_texture = -1;
GLuint pick_texture = -1;
GLuint fbo_texture_width = 1280;
GLuint fbo_texture_height = 720;

//The frame buffer object provides render-to-texture functionality 
GLuint fbo;
GLuint dpth_rb;

int STAGE = 1;

float time_sec = 0.0f;
float angleX = 0.0f;
float angleY = 0.0f;
float angleZ = 0.0f;

//bool recording = false;
int mouseX = 0;
int mouseY = 0;

float score = 0;
float TempScore = 0;
float ScoreStep = 0;

int startTime = 0;
int showTime[4] = { 0,0,0,0 };
bool holdFlag = true;

int level = 1;
int pattern = 1;

ifstream readHS;
ofstream writeHS;
string hsStrings[6];
int hsValue[6];

void initOpenGl();
void reload_shader();

textureLetters titleText(8, 0.2, 0.3, -0.8, -0.0);
textureLetters titleNotice(11, 0.05, 0.11, -0.275, -0.5);

textureLetters levelText(7, 0.2, 0.3, -0.7, -0.0);
textureLetters thankyouText(10, 0.15, 0.25, -0.75, -0.0);

textureLetters hsTitle(10, 0.16, 0.3, -0.8, 0.65);
textureLetters hsText[6] = { textureLetters(7, 0.14, 0.2, -0.49, 0.35),
								 textureLetters(7, 0.14, 0.2, -0.49, 0.15),
								 textureLetters(7, 0.14, 0.2, -0.49, -0.05),
								 textureLetters(7, 0.14, 0.2, -0.49, -0.25),
								 textureLetters(7, 0.14, 0.2, -0.49, -0.45),
								 textureLetters(7, 0.14, 0.2, -0.49, -0.65) };
textureLetters hsNotice(11, 0.05, 0.075, -0.275, -0.85);

int string2value(char* input)
{
	char what[5];
	for (int i = 2; i < 7; i++)
	{
		what[i-2] = input[i];
	}
	return atoi(what);
}

string value2string(int number, int value)
{
	stringstream output;
	output << number << ".";
	for (int i = 4; i >= 0; i--)
	{
		int dm = pow(10, i + 1);
		int digit = value - (value / dm *dm);
		digit /= pow(10, i);
		output << digit;
	}
	return output.str();
}

void updateHS()
{
	int newList[7];
	int tmp;
	for (int i = 0; i < 6; i++)
		newList[i] = hsValue[i];
	newList[6] = score;
	for(int i=0;i<7;i++)
		for(int j=i;j<7;j++)
			if (newList[j] > newList[i])
			{
				tmp = newList[i];
				newList[i] = newList[j];
				newList[j] = tmp;
			}
	for (int i = 0; i < 6; i++)
		hsValue[i] = newList[i];
}

void randAngles()
{
	srand(int(glutGet(GLUT_ELAPSED_TIME)));
	angleX = (rand() / float(RAND_MAX) - 0.5) * 3.1415926;
	angleY = (rand() / float(RAND_MAX) - 0.5) * 3.1415926;
	float a = 0;
}

void reloadPattern(float level, float pattern)
{
	std::stringstream filename;
	filename << "patterns/" << level << "/" << pattern << ".bmp";
	texture_name = filename.str();

	texture_id = LoadTexture(texture_name.c_str());
	glGenTextures(1, &fbo_texture);
	glBindTexture(GL_TEXTURE_2D, fbo_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fbo_texture_width, fbo_texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	randAngles();
	startTime = glutGet(GLUT_ELAPSED_TIME);
}

//Draw the user interface using ImGui
void draw_gui()
{

	ImGui_ImplGlut_NewFrame();
	ImGui::SetWindowPos(ImVec2(20, 520));
	ImGui::SetWindowSize(ImVec2(150, 165));
	//   const int filename_len = 256;
	//   static char video_filename[filename_len] = "capture.mp4";

	ImGui::Image((void*)texture_id, ImVec2(128.0f, 128.0f), ImVec2(0.0, 1.0), ImVec2(1.0, 0.0));
	ImGui::Render();
}

// glut display callback function.
// This function gets called every time the scene gets redisplayed 
void display()
{
	const int w = glutGet(GLUT_WINDOW_WIDTH);
	const int h = glutGet(GLUT_WINDOW_HEIGHT);
	const float aspect_ratio = float(w) / float(h);

	DisplayBackGround();

	if (STAGE == 1)
	{
		titleText.Display("welcome!", 2);
		titleNotice.Display("press enter", 3);
	}

	else if (STAGE == 2)
	{
		static int delayCounter = 0;
		holdFlag = true;
		stringstream levelNow;
		levelNow << "LEVEL " << level;
		if (delayCounter < 300)
		{
			delayCounter++;
			if (level == 6)
				thankyouText.Display("Thank you!", 2);
			else
				levelText.Display(levelNow.str(),2);
		}
		else
		{
			if (level == 6)
			{
				STAGE = 4;
			}
			else
			{
				STAGE = 3;
			}
			delayCounter = 0;
			holdFlag = false;
			startTime = glutGet(GLUT_ELAPSED_TIME);
		}
	}

	else if (STAGE == 3)
	{
		glUseProgram(shader_program);

		//Set uop some uniform variables
		glm::mat4 Mx = glm::rotate(angleX, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 My = glm::rotate(angleY, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 Mz = glm::rotate(angleZ, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 M = Mx*My*Mz;

		glm::vec3 CameraPos(0.0f, 0.f, 1.5f);
		glm::mat4 V = glm::lookAt(CameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 P = glm::perspective(3.141592f / 4.0f, aspect_ratio, 0.1f, 100.0f);

		glUniformMatrix4fv(10, 1, false, glm::value_ptr(P));
		glUniformMatrix4fv(11, 1, false, glm::value_ptr(V));
		glUniformMatrix4fv(12, 1, false, glm::value_ptr(M));
		glUniform1d(5, 1);//startFlag
		glUniform1f(50, float(level));

		//M for converge
		glm::vec4 CameraPosVec4 = glm::vec4(CameraPos, 1.0f);
		float  CameraPos4fv[4], ConvergePos4fv[4];
		for (int i = 0; i < 4; i++)
		{
			CameraPos4fv[i] = CameraPosVec4[i];
		}
		glUniform4fv(20, 1, CameraPos4fv);

		//we are using layout qualifiers in the shader

		const int PVM_loc = 0;
		const int pass_loc = 1;
		const int tex_loc = 2;
		const int time_loc = 4;
		const int chosen_loc = 5;

		glm::mat4 PVM = P*V*M;
		glUniformMatrix4fv(PVM_loc, 1, false, glm::value_ptr(PVM));

		glUniform1f(time_loc, time_sec);

		glUniform1i(pass_loc, 1);

		glBindTexture(GL_TEXTURE_2D, texture_id);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUniform1i(tex_loc, 0);
		glViewport(0, 0, w, h);

		DrawSurfaceVao();

		glUniform1i(pass_loc, 2);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glViewport(0, 0, w, h);
		DrawSurfaceVao();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (!holdFlag)
		{
			for (int i = 0; i < 4; i++)
			{
				int timeNow = glutGet(GLUT_ELAPSED_TIME) - startTime;
				int timeDigit = timeNow / pow(10, 4 - i);//4-digit:second time, 6-digit, minute; 8-digit, hours
				showTime[i] = timeDigit - timeDigit / 10 * 10;
			}
		}

		DisplayTimer(showTime);
		DisplayScore(score);

		draw_gui();

		GLfloat buffer[4];
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glReadPixels(640, 360, 1, 1, GL_RGBA, GL_FLOAT, buffer);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//If you win
		static int delayCounter = 0;
		if (buffer[0] == 1)
		{
			if (!holdFlag)
			{
				//PlaySound("dingdong.wav", NULL, SND_NOSTOP | SND_ASYNC | SND_FILENAME);
				mciSendString(TEXT("open dingdong.wav alias Succeed"), NULL, 0, NULL);
				mciSendString(TEXT("play Succeed "), NULL, 0, NULL);

				float usedTime = showTime[0] * 1000 + showTime[1] * 100 + showTime[2] * 10 + showTime[4] * 1;
				float scoreThisRound = (2000 - level * 200 - usedTime) / 10 * ((level + 1) / 2);
				if (scoreThisRound < 0)
					scoreThisRound = 0;
				TempScore = score + scoreThisRound;
				TempScore += 10;

				ScoreStep = TempScore - score;
			}

			holdFlag = true;

			if (delayCounter < 100)
			{
				if (delayCounter < 98)
					score += ScoreStep / 98;
				if (delayCounter == 98)
					score = TempScore;
				delayCounter++;
			}

			else
			{
				pattern++;
				if (pattern == 6)
				{
					pattern = 1;
					level++;
					SetLevel(level);
					if (level == 6)
					{
						readHS.open("high_score.txt");
						for (int n = 0; n < 6; n++)
						{
							char curLine[8];
							readHS.getline(curLine, 8);
							hsValue[n] = string2value(curLine);
						}
						readHS.close();
						updateHS();
					}
					STAGE = 2;
				}

				reloadPattern(level, pattern);

				delayCounter = 0;
				mciSendString("close Succeed", NULL, 0, NULL);
				holdFlag = false;
				startTime = glutGet(GLUT_ELAPSED_TIME);
			}
		}
	}

	else if (STAGE == 4)
	{
		holdFlag = true;

		hsTitle.Display("HIGH SCORE",1);

		bool hsFlag = false;
		static bool playFlag = false;

		for (int i = 0; i < 6; i++)
		{
			hsStrings[i] = value2string(i + 1, hsValue[i]);

			if (score == hsValue[i] && !hsFlag)
			{
				hsText[i].Display(hsStrings[i], 1);
				hsFlag = true;
			}
			else
				hsText[i].Display(hsStrings[i],3);
		}

		if (hsFlag && !playFlag)
		{
			PlaySound("V.wav", NULL, SND_ASYNC | SND_FILENAME);
			playFlag = true;
		}
		hsNotice.Display("press enter",3);
	}

	/*if (recording == true)
	{
		glFinish();

		glReadBuffer(GL_BACK);
		read_frame_to_encode(&rgb, &pixels, w, h);
		encode_frame(rgb);
	}*/

	glutSwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// glut idle callback.
//This function gets called between frames
void idle()
{
	glutPostRedisplay();

	const int time_ms = glutGet(GLUT_ELAPSED_TIME);
	time_sec = 0.001f*time_ms;
}

void reload_shader()
{
	//
	GLuint new_shader = InitShader(vertex_shader.c_str(), geometry_shader.c_str(), fragment_shader.c_str());

	if (new_shader == -1) // loading failed
	{
		glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
	}
	else
	{
		glClearColor(0.7f, 0.8f, 0.8f, 1.0f);

		if (shader_program != -1)
		{
			glDeleteProgram(shader_program);
		}
		shader_program = new_shader;
	}

	InitBackGroundShader();
	InitTimerShader();
	InitScoreShader();

	titleText.Init();
	titleNotice.Init();
	levelText.Init();
	thankyouText.Init();

	hsTitle.Init();
	for (int i = 0; i < 6; i++)
		hsText[i].Init();
	hsNotice.Init();
}

// Display info about the OpenGL implementation provided by the graphics driver.
// Your version should be > 4.0 for CGT 521 
void printGlInfo()
{
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void initOpenGl()
{
	//Initialize glew so that new OpenGL function names can be used
	glewInit();

	RegisterCallback();

	glEnable(GL_DEPTH_TEST);

	randAngles();
	startTime = glutGet(GLUT_ELAPSED_TIME);;
	reload_shader();

	//Load a mesh and a texture
	reloadPattern(level,pattern); 
	
	//Helper function: Uses FreeImage library
	CreateBackGroundVao();
	CreateSurfaceVao();
	CreateTimerVao();
	CreateScoreVao();

	titleText.CreateVao();
	titleNotice.CreateVao();
	levelText.CreateVao();
	thankyouText.CreateVao();

	hsTitle.CreateVao();
	for (int i = 0; i < 6; i++)
		hsText[i].CreateVao();	
	hsNotice.CreateVao();
}

void reset()
{
	score = 0;

	level = 1;
	pattern = 1;

	SetLevel(level);
	initOpenGl();
	PlaySound("bgm.wav", NULL, SND_LOOP | SND_ASYNC | SND_FILENAME);
}
// glut callbacks need to send keyboard and mouse events to imgui
void keyboard(unsigned char key, int x, int y)
{
	ImGui_ImplGlut_KeyCallback(key);

	std::cout << "key : " << key << ", x: " << x << ", y: " << y << std::endl;

	if (!holdFlag)
	{
		switch (key)
		{

		case 'r':
		case 'R':
			reloadPattern(level, pattern); reload_shader();
			break;
			 
		case '1':  level = 1; pattern = 1; SetLevel(level); reloadPattern(level, pattern); STAGE = 2; break;
		case '2':  level = 2; pattern = 1; SetLevel(level); reloadPattern(level, pattern); STAGE = 2; break;
		case '3':  level = 3; pattern = 1; SetLevel(level); reloadPattern(level, pattern); STAGE = 2; break;
		case '4':  level = 4; pattern = 1; SetLevel(level); reloadPattern(level, pattern); STAGE = 2; break;
		case '5':  level = 5; pattern = 1; SetLevel(level); reloadPattern(level, pattern); STAGE = 2; break;

		case 'w':
		case 'W':  angleX -= 0.08; break;
		case 's':
		case 'S':  angleX += 0.08; break;
		case 'a':
		case 'A':  angleY -= 0.08; break;
		case 'd':
		case 'D':  angleY += 0.08; break;

		case '+':  startTime = glutGet(GLUT_ELAPSED_TIME); randAngles(); PlusLevel(); reload_shader(); initOpenGl(); break;
		case '-':  startTime = glutGet(GLUT_ELAPSED_TIME); randAngles(); MinusLevel(); reload_shader(); initOpenGl(); break;
		}
	}
	else if (key == 13)
	{
		if (STAGE == 1)
		{
			STAGE = 2;
			holdFlag = false;
		}
		if (STAGE == 4)
		{
			writeHS.open("high_score.txt");
			for (int i = 0; i < 6; i++)
			{
				char curLine[8];
				for (int j = 0; j < 7; j++)
				{
					curLine[j] = hsStrings[i][j];
				}
				curLine[7] = '\n';
				writeHS.write(curLine,8);
			}
			writeHS.close();

			reset();
			STAGE = 1;
		}
	}
}

void keyboard_up(unsigned char key, int x, int y)
{
	ImGui_ImplGlut_KeyUpCallback(key);
}

void special_up(int key, int x, int y)
{
	ImGui_ImplGlut_SpecialUpCallback(key);
}

void passive(int x, int y)
{
	ImGui_ImplGlut_PassiveMouseMotionCallback(x, y);
}

void special(int key, int x, int y)
{
	ImGui_ImplGlut_SpecialCallback(key);
}

void motion(int x, int y)
{
	ImGui_ImplGlut_MouseMotionCallback(x, y);
}

void mouse(int button, int state, int x, int y)
{
	ImGui_ImplGlut_MouseButtonCallback(button, state);

	mouseX = x;
	mouseY = y;
}


int main(int argc, char **argv)
{
	//Configure initial window state using freeglut

#if _DEBUG
	glutInitContextFlags(GLUT_DEBUG);
#endif
	glutInitContextVersion(4, 3);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(5, 5);
	glutInitWindowSize(1280, 720);
	int win = glutCreateWindow("3D Puzzle");

	printGlInfo();

	PlaySound("bgm.wav", NULL, SND_LOOP | SND_ASYNC | SND_FILENAME);

	//Register callback functions with glut. 
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutKeyboardUpFunc(keyboard_up);
	glutSpecialUpFunc(special_up);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(motion);

	glutIdleFunc(idle);

	initOpenGl();
	ImGui_ImplGlut_Init(); // initialize the imgui system

	//Enter the glut event loop.
	glutMainLoop();

	//mciSendString("close bgm", NULL, 0, NULL);

	glutDestroyWindow(win);
	return 0;
}


