#pragma once
#include"Triangle.h"
#include"RawInfoType.h"

int marching_cube_genTri(Triangle_t** triangle, const float* data, const float isoVal, const float* gra, const rawInf_t rawinf);