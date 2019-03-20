#pragma once
#include"Triangle.h"

Point_t bi_linear_interpolate(float isoVal, float P0Val, Point_t P0, float P1Val, Point_t P1);
Gradient_t normalize(Gradient_t v);