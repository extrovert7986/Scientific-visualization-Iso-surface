#include<GL\glew.h>
#include<GL\freeglut.h>
#include<glm\glm.hpp>
#include<stdio.h>
#include<math.h>
#include"ShaderProgram.h"
#include"ReadFile.h"
#include"MarchingCube.h"
#include"MarchingTetrahedra.h"

#define ISOVALUE 1400

const char* fileName = "cthead.inf";
rawInf_t rawinf;
float* data = nullptr;
float* gra = nullptr;		//gradient x*y*z*3
Triangle_t* triList = nullptr;
int count = 0;
double zoomFactor = 1.0;
float eyePos[3] = { 200.0,50.0,0.0 }; float focus[3] = { 0,0,0 }; float vupVec[3] = { 0,1,0 };
float u[3];
float w[3];

GLuint vao;
GLuint program;
int triCnt;
void setLight();

void init() {
	//setting program
	GLuint shader[5] = {0};
	shader[0] = set_shader(GL_VERTEX_SHADER, "vertex.glsl");
	shader[1] = set_shader(GL_FRAGMENT_SHADER, "fragment.glsl");
	
	program = set_program(shader);
	glUseProgram(program);

	
	glEnable(GL_DEPTH_TEST);
	//processing the raw data
	rawinf = read_vol(fileName);
	data = read_raw(rawinf);
	gra = compute_gradient(data, rawinf);
	triCnt = marching_cube_genTri(&triList, data, ISOVALUE, gra, rawinf);
	//triCnt = marching_tetra_genTri(&triList, data, ISOVALUE, gra, rawinf);
	
	//populate data into vbo & vao
	float* points = (float*)calloc(triCnt * 3 * 3, sizeof(float));
	float* normals = (float*)calloc(triCnt * 3 * 3, sizeof(float));
	Triangle_t* tmp = triList;
	for (int i = 0; i < triCnt; i++) {
		for (int j = 0; j < 3; j++) {
			int idx = (i * 3 + j) * 3;
			points[idx] = tmp->p[j].x;
			points[idx + 1] = tmp->p[j].y;
			points[idx + 2] = tmp->p[j].z;
			normals[idx] = tmp->p[j].gra.x;
			normals[idx + 1] = tmp->p[j].gra.y;
			normals[idx + 2] = tmp->p[j].gra.z;
		}
		tmp = tmp->next;
	}
	GLuint vbo[2];
	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, triCnt * 3 * 3 * sizeof(float), points, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, triCnt * 3 * 3 * sizeof(float), normals, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexBuffer(0, vbo[0], 0, sizeof(GLfloat) * 3);
	glBindVertexBuffer(1, vbo[1], 0, sizeof(GLfloat) * 3);
	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(0, 0);
	glVertexAttribFormat(1, 3, GL_FLOAT, GL_TRUE, 0);
	glVertexAttribBinding(1, 1);
	
	setLight();

}

void display() {
	float mtx[16];

	glClearColor(0.3, 0.3, 0.3, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-150 * zoomFactor, 150 * zoomFactor, -150 * zoomFactor, 200 * zoomFactor, 1, 600);
	glGetFloatv(GL_PROJECTION_MATRIX, mtx);
	glUniformMatrix4fv(glGetUniformLocation(program, "ProjectionMatrix"), 1, false, mtx);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyePos[0], eyePos[1], eyePos[2], focus[0], focus[1], focus[2], vupVec[0], vupVec[1], vupVec[2]);
	glGetFloatv(GL_MODELVIEW_MATRIX, mtx);
	glUniformMatrix4fv(glGetUniformLocation(program, "ModelViewMatrix"), 1, false, mtx);

	//glEnable(GL_LIGHTING);
	/*glBegin(GL_TRIANGLES);
	for (Triangle_t* tmp = triList; tmp; tmp = tmp->next) {
		glNormal3f(tmp->p[0].gra.x, tmp->p[0].gra.y, tmp->p[0].gra.z);
		glVertex3f(tmp->p[0].x, tmp->p[0].y, tmp->p[0].z);
		glNormal3f(tmp->p[1].gra.x, tmp->p[1].gra.y, tmp->p[1].gra.z);
		glVertex3f(tmp->p[1].x, tmp->p[1].y, tmp->p[1].z);
		glNormal3f(tmp->p[2].gra.x, tmp->p[2].gra.y, tmp->p[2].gra.z);
		glVertex3f(tmp->p[2].x, tmp->p[2].y, tmp->p[2].z);
	}
	glEnd();*/

	/*glDisable(GL_LIGHTING);
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINES);
	for (Triangle_t* tmp = triList; tmp->next; tmp = tmp->next) {
		glNormal3f(tmp->p[0].gra.x, tmp->p[0].gra.y, tmp->p[0].gra.z);
		glVertex3f(tmp->p[0].x, tmp->p[0].y, tmp->p[0].z);
		glNormal3f(tmp->p[1].gra.x, tmp->p[1].gra.y, tmp->p[1].gra.z);
		glVertex3f(tmp->p[1].x, tmp->p[1].y, tmp->p[1].z);
		glNormal3f(tmp->p[2].gra.x, tmp->p[2].gra.y, tmp->p[2].gra.z);
		glVertex3f(tmp->p[2].x, tmp->p[2].y, tmp->p[2].z);
	}
	glEnd();
	*/

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, triCnt*3);
	glutSwapBuffers();
}

void reshape(int x, int y) {
	glViewport(0, 0, x, y);
	
}

void mouse(int button,int state,int x,int y) {

}

void keyboard(unsigned char key, int x, int y) {
	//compute w vector
	w[0] = focus[0] - eyePos[0];
	w[1] = focus[1] - eyePos[1];
	w[2] = focus[2] - eyePos[2];
	double mag = sqrt(w[0] * w[0] + w[1] * w[1] + w[2] * w[2]);
	w[0] /= mag; w[1] /= mag; w[2] /= mag;
	//compute u vector

	u[0] = w[1] * vupVec[2] - vupVec[1] * w[2];
	u[1] = w[2] * vupVec[0] - vupVec[2] * w[0];
	u[2] = w[0] * vupVec[1] - vupVec[0] * w[1];
	mag = sqrt(u[0] * u[0] + u[1] * u[1] + u[2] * u[2]);
	u[0] /= mag; u[1] /= mag; u[2] /= mag;
	//compute vup vector
	vupVec[0] = u[1] * w[2] - w[1] * u[2];
	vupVec[1] = u[2] * w[0] - w[2] * u[0];
	vupVec[2] = u[0] * w[1] - w[0] * u[1];
	mag = sqrt(vupVec[0] * vupVec[0] + vupVec[1] * vupVec[1] + vupVec[2] * vupVec[2]);
	vupVec[0] /= mag; vupVec[1] /= mag; vupVec[2] /= mag;

	if (key == 'w' || key == 'W') {
		GLfloat mat[16];
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(-focus[0], -focus[1], -focus[2]);
		glRotatef(0.5, u[0], u[1], u[2]);
		glGetFloatv(GL_MODELVIEW_MATRIX, mat);
		double tmp[3];
		tmp[0] = mat[0] * eyePos[0] + mat[4] * eyePos[1] + mat[8] * eyePos[2];
		tmp[1] = mat[1] * eyePos[0] + mat[5] * eyePos[1] + mat[9] * eyePos[2];
		tmp[2] = mat[2] * eyePos[0] + mat[6] * eyePos[1] + mat[10] * eyePos[2];
		eyePos[0] = tmp[0]; eyePos[1] = tmp[1]; eyePos[2] = tmp[2];
	}
	if (key == 's' || key == 'S') {
		GLfloat mat[16];
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(-focus[0], -focus[1], -focus[2]);
		glRotatef(-0.5, u[0], u[1], u[2]);
		glGetFloatv(GL_MODELVIEW_MATRIX, mat);
		double tmp[3];
		tmp[0] = mat[0] * eyePos[0] + mat[4] * eyePos[1] + mat[8] * eyePos[2];
		tmp[1] = mat[1] * eyePos[0] + mat[5] * eyePos[1] + mat[9] * eyePos[2];
		tmp[2] = mat[2] * eyePos[0] + mat[6] * eyePos[1] + mat[10] * eyePos[2];
		eyePos[0] = tmp[0]; eyePos[1] = tmp[1]; eyePos[2] = tmp[2];
	}
	if (key == 'a' || key == 'A') {
		GLfloat mat[16];
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(-focus[0], -focus[1], -focus[2]);
		glRotatef(0.5, vupVec[0], vupVec[1], vupVec[2]);
		glGetFloatv(GL_MODELVIEW_MATRIX, mat);
		double tmp[3];
		tmp[0] = mat[0] * eyePos[0] + mat[4] * eyePos[1] + mat[8] * eyePos[2];
		tmp[1] = mat[1] * eyePos[0] + mat[5] * eyePos[1] + mat[9] * eyePos[2];
		tmp[2] = mat[2] * eyePos[0] + mat[6] * eyePos[1] + mat[10] * eyePos[2];
		eyePos[0] = tmp[0]; eyePos[1] = tmp[1]; eyePos[2] = tmp[2];
	}
	if (key == 'd' || key == 'D') {
		GLfloat mat[16];
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(-focus[0], -focus[1], -focus[2]);
		glRotatef(-0.5, vupVec[0], vupVec[1], vupVec[2]);
		glGetFloatv(GL_MODELVIEW_MATRIX, mat);
		double tmp[3];
		tmp[0] = mat[0] * eyePos[0] + mat[4] * eyePos[1] + mat[8] * eyePos[2];
		tmp[1] = mat[1] * eyePos[0] + mat[5] * eyePos[1] + mat[9] * eyePos[2];
		tmp[2] = mat[2] * eyePos[0] + mat[6] * eyePos[1] + mat[10] * eyePos[2];
		eyePos[0] = tmp[0]; eyePos[1] = tmp[1]; eyePos[2] = tmp[2];
	}
	if (key == 'z' || key == 'Z') {
		zoomFactor *= 0.9;
	}
	if (key == 'x' || key == 'X') {
		zoomFactor /= 0.9;
	}

	glutPostRedisplay();
}

void setLight() {
	float lightPos[4] = { -1.0,-1.0,-1.0 ,0.0 };
	float lightLa[3] = { 0.4,0.4,0.4 };
	float lightLd[3] = { 0.7,0.7,0.7 };
	float lightLs[3] = { 1.0,1.0,1.0 };

	glUniform3fv(glGetUniformLocation(program, "Light.position"), 1, lightPos);
	glUniform3fv(glGetUniformLocation(program, "Light.La"), 1, lightLa);
	glUniform3fv(glGetUniformLocation(program, "Light.Ld"), 1, lightLd);
	glUniform3fv(glGetUniformLocation(program, "Light.Ls"), 1, lightLs);

	float materialKa[3] = { 0.9,0.85,0.5 };
	float materialKd[3] = { 0.9,0.85,0.5 };
	float materialKs[3] = { 0.3,0.3,0.3 };
	
	glUniform3fv(glGetUniformLocation(program, "Material.Ka"), 1, materialKa);
	glUniform3fv(glGetUniformLocation(program, "Material.Kd"), 1, materialKd);
	glUniform3fv(glGetUniformLocation(program, "Material.Ks"), 1, materialKs);
	glUniform1f(glGetUniformLocation(program, "Material.shiness"), 32.0);

}


