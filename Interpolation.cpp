#include<math.h>
#include"Triangle.h"
#include"Interpolation.h"

Gradient_t Slerp(Gradient_t v0, Gradient_t v1, double t);

Point_t bi_linear_interpolate(float isoVal, float P0Val, Point_t P0, float P1Val, Point_t P1) {
	float mu;
	Point_t p;
	if (fabs(isoVal - P0Val) < 0.00001) {
		return P0;
	}
	if (fabs(isoVal - P1Val) < 0.00001) {
		return P1;
	}
	if (fabs(P1Val - P0Val) < 0.00001) {
		return P0;
	}
	mu = (isoVal - P0Val) / (P1Val - P0Val);

	p.x = (1 - mu)*P0.x + mu * P1.x;
	p.y = (1 - mu)*P0.y + mu * P1.y;
	p.z = (1 - mu)*P0.z + mu * P1.z;
	p.gra = Slerp(P0.gra, P1.gra, mu);

	return p;
}

Gradient_t Slerp(Gradient_t v0, Gradient_t v1, double t) {
	double magV0, magV1, theta;
	Gradient_t unitV0, unitV1, vecReturn;

	magV0 = sqrt(v0.x*v0.x + v0.y*v0.y + v0.z*v0.z);
	magV1 = sqrt(v1.x*v1.x + v1.y*v1.y + v1.z*v1.z);

	unitV0 = normalize(v0);
	unitV1 = normalize(v1);

	double dot = unitV0.x*unitV1.x + unitV0.y*unitV1.y + unitV0.z*unitV1.z;
	if (dot < 0.0f) {
		unitV1.x = -unitV1.x;
		unitV1.y = -unitV1.y;
		unitV1.z = -unitV1.z;
		dot = -dot;
	}
	if (dot > 0.9995) {
		vecReturn.x = (1 - t)*unitV0.x + t * unitV1.x;
		vecReturn.y = (1 - t)*unitV0.y + t * unitV1.y;
		vecReturn.z = (1 - t)*unitV0.z + t * unitV1.z;
		vecReturn = normalize(vecReturn);
		return vecReturn;
	}
	dot = dot > 1.0 ? 1 : dot;
	dot = dot < -1.0 ? -1 : dot;

	theta = acos(dot);

	double coef0, coef1;
	coef0 = sin((1 - t)*theta) / sin(theta);
	coef1 = sin(t*theta) / sin(theta);
	vecReturn.x = coef0 * unitV0.x + coef1 * unitV1.x;
	vecReturn.y = coef0 * unitV0.y + coef1 * unitV1.y;
	vecReturn.z = coef0 * unitV0.z + coef1 * unitV1.z;

	return vecReturn;
}

Gradient_t normalize(Gradient_t v) {
	double mag = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	if (mag < 1e-4) {
		return { 0.0,0.0,0.0 };
	}
	v.x /= mag;
	v.y /= mag;
	v.z /= mag;

	return v;

}