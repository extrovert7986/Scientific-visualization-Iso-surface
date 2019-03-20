#include<stdio.h>
#include<stdlib.h>
#include"ShaderProgram.h"

GLuint set_program(GLuint shader[5]) {
	//create program object
	GLuint program;
	if ((program = glCreateProgram()) == 0) {
		fprintf(stderr, "Error creating program object");
		exit(0);
	}
	//attach shader to program
	for (int i = 0; i < 5; i++) {
		if (shader[i])
			glAttachShader(program, shader[i]);
	}
	//link program
	glLinkProgram(program);
	//verify the link status
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		fprintf(stderr, "Failed to link shader program.\n");
		GLint logLen;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0) {
			char* log = (char*)calloc(logLen, sizeof(char));
			GLsizei written;
			glGetProgramInfoLog(program, logLen, &written, log);
			fprintf(stderr, "program log: \n%s", log);
			free(log);
		}
	}
	return program;
}

char* read_source_code(const char* name);
GLuint set_shader(GLenum type, const char* name) {
	//Read shader content in
	const GLchar* sourceCode = read_source_code(name);
	
	const GLchar* codeArray[] = { sourceCode };
	//create shader object
	GLuint shader;
	if ((shader = glCreateShader(type)) == 0) {
		fprintf(stderr, "Error creating shader.\n");
		exit(0);
	}
	//copy shader source code into shader object
	glShaderSource(shader, 1, codeArray, NULL);
	free((void*)sourceCode);
	//compile
	glCompileShader(shader);
	//verify the compilation status
	GLint result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		fprintf(stderr, "Shader compilation failed.\n");
		GLint logLen;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0) {
			char* log = (char*)calloc(logLen, sizeof(char));

			GLsizei written;
			glGetShaderInfoLog(shader, logLen, &written, log);

			fprintf(stderr, "shader log:\n%s", log);
			free(log);
		}
	}

	return shader;
}

char* read_source_code(const char* name) {
	FILE* fp = fopen(name, "r");
	if (!(fp = fopen(name, "r"))) {
		printf("File %s opened failed.\n", name);
		exit(0);
	}
	fseek(fp, 0, SEEK_END);
	int fSize = ftell(fp);
	rewind(fp);
	char* content = (char*)calloc(fSize, sizeof(char));
	fread(content, sizeof(char), fSize, fp);
	fclose(fp);
	printf("------------------  shader one  ------------------\n");
	printf("%s\n", content);
	return content;
}