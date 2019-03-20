#pragma once
#include<GL\glew.h>

GLuint set_shader(GLenum type, const char* name);
GLuint set_program(GLuint shader[5]);