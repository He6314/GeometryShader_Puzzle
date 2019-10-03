#include <windows.h>
#include <GL/glew.h>

#include "DrawPlane.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

static unsigned int vao;
static unsigned int matrix_buffer;

int ni = 10; 
int nj = 10;

#define BUFFER_OFFSET(i) ((char *)NULL + (i)) 

void PlusLevel()
{
	ni += 5;
	nj += 5;
}
void MinusLevel()
{
	ni -= 5;
	nj -= 5;
}

void SetLevel(int level)
{
	ni = 5 + level * 5;
	nj = 5 + level * 5;
	CreateSurfaceVao();
}

void CreateSurfaceVao()
{

   const int pos_loc = 0;
   const int tex_coord_loc = 1;
   const int normal_loc = 2;

   std::vector<float> vertices;
   /*
   for (int i = 0; i<ni; i++)
   {
      float u = i / float(ni - 1);
      float x = (2.0f*u - 1.0f) * 0.5f;

      for (int j = 0; j<nj; j++)
      {
         float v = j / float(nj - 1);
         float y = (2.0f*v - 1.0f)*0.5f;

         glm::vec3 p = surface(x, y);

         //push vertex coords
         vertices.push_back(p.x);
         vertices.push_back(p.y);
         vertices.push_back(p.z);

         //push tex coords
         vertices.push_back(u);
         vertices.push_back(v);

         glm::vec3 n = normal(x, y);
         //push vertex coords
         vertices.push_back(n.x);
         vertices.push_back(n.y);
         vertices.push_back(n.z);

      }
   }
   */

   for (int i = 0; i<ni; i++)
   {
	   float u = i / float(ni - 1);
	   float x = (u -  0.5f)*1.f;

	   for (int j = 0; j<nj; j++)
	   {
		   float v = j / float(nj - 1);
		   float y = (v - 0.5f)*1.f;

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
   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);
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

   std::vector<unsigned int> indices;

   for (int i = 0; i < ni -1; i++)
   {
	   for (int j = 0; j < nj-1; j++)
	   {
		   indices.push_back(i*nj + j);
		   indices.push_back(i*nj + j + nj);
		   indices.push_back(i*nj + j + 1);

		   indices.push_back(i*nj + j + nj);
		   indices.push_back(i*nj + j + 1 + nj);
		   indices.push_back(i*nj + j + 1);
	   }
   }

   unsigned int ibo;
   glGenBuffers(1, &ibo);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

   glBindVertexArray(0);

}

void DrawSurfaceVao()
{
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, (ni-1)*(nj-1)*6, GL_UNSIGNED_INT, 0);

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}