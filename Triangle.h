#pragma once

struct Gradient_t {
	float x;
	float y;
	float z;
};

struct Point_t {
	float x;
	float y;
	float z;
	Gradient_t gra;
};

struct Triangle_t {
	Point_t p[3];
	Triangle_t* next;
};