#include<stdlib.h>
#include"Triangle.h"
#include"CubeSliceTable.h"
#include"RawInfoType.h"
#include"Interpolation.h"

int gen_tetra_tri(Triangle_t** triList, const int index[4], const float isoVal, const float vertVal[8], const Point_t P[8]);

int marching_tetra_genTri(Triangle_t** triList, const float* data, const float isoVal, const float* gra, const rawInf_t rawinf) {

	double shiftX, shiftY, shiftZ;
	shiftX = -(rawinf.res.x / 2.0);
	shiftY = -(rawinf.res.y / 2.0);
	shiftZ = -(rawinf.res.z / 2.0);
	int Cnt = 0;
	for (int k = 0; k < rawinf.res.z - 1; k++) 
		for (int j = 0; j < rawinf.res.y - 1; j++) 
			for (int i = 0; i < rawinf.res.x - 1; i++) {
				int index[8] = {
					(i)     +(j)    *rawinf.res.x + (k)    *rawinf.res.x*rawinf.res.y,
					(i + 1) +(j)    *rawinf.res.x + (k)    *rawinf.res.x*rawinf.res.y,
					(i + 1) +(j + 1)*rawinf.res.x + (k)    *rawinf.res.x*rawinf.res.y,
					(i)     +(j + 1)*rawinf.res.x + (k)    *rawinf.res.x*rawinf.res.y,
					(i)     +(j)    *rawinf.res.x + (k + 1)*rawinf.res.x*rawinf.res.y,
					(i + 1) +(j)    *rawinf.res.x + (k + 1)*rawinf.res.x*rawinf.res.y,
					(i + 1) +(j + 1)*rawinf.res.x + (k + 1)*rawinf.res.x*rawinf.res.y,
					(i)     +(j + 1)*rawinf.res.x + (k + 1)*rawinf.res.x*rawinf.res.y
				};
				float vertVal[8] = {
					data[index[0]],
					data[index[1]],
					data[index[2]],
					data[index[3]],
					data[index[4]],
					data[index[5]],
					data[index[6]],
					data[index[7]]
				};
				Point_t vert[8] = {
					{ (i     + shiftX)*rawinf.ratio.x,(j     + shiftY)*rawinf.ratio.y,(k     + shiftZ)*rawinf.ratio.z,{ gra[index[0] * 3],gra[index[0] * 3 + 1],gra[index[0] * 3 + 2] } },		//0
					{ (i + 1 + shiftX)*rawinf.ratio.x,(j     + shiftY)*rawinf.ratio.y,(k     + shiftZ)*rawinf.ratio.z,{ gra[index[1] * 3],gra[index[1] * 3 + 1],gra[index[1] * 3 + 2] } },		//1
					{ (i + 1 + shiftX)*rawinf.ratio.x,(j + 1 + shiftY)*rawinf.ratio.y,(k     + shiftZ)*rawinf.ratio.z,{ gra[index[2] * 3],gra[index[2] * 3 + 1],gra[index[2] * 3 + 2] } },		//2
					{ (i     + shiftX)*rawinf.ratio.x,(j + 1 + shiftY)*rawinf.ratio.y,(k     + shiftZ)*rawinf.ratio.z,{ gra[index[3] * 3],gra[index[3] * 3 + 1],gra[index[3] * 3 + 2] } },		//3
					{ (i     + shiftX)*rawinf.ratio.x,(j     + shiftY)*rawinf.ratio.y,(k + 1 + shiftZ)*rawinf.ratio.z,{ gra[index[4] * 3],gra[index[4] * 3 + 1],gra[index[4] * 3 + 2] } },		//4
					{ (i + 1 + shiftX)*rawinf.ratio.x,(j     + shiftY)*rawinf.ratio.y,(k + 1 + shiftZ)*rawinf.ratio.z,{ gra[index[5] * 3],gra[index[5] * 3 + 1],gra[index[5] * 3 + 2] } },		//5
					{ (i + 1 + shiftX)*rawinf.ratio.x,(j + 1 + shiftY)*rawinf.ratio.y,(k + 1 + shiftZ)*rawinf.ratio.z,{ gra[index[6] * 3],gra[index[6] * 3 + 1],gra[index[6] * 3 + 2] } },		//6
					{ (i     + shiftX)*rawinf.ratio.x,(j + 1 + shiftY)*rawinf.ratio.y,(k + 1 + shiftZ)*rawinf.ratio.z,{ gra[index[7] * 3],gra[index[7] * 3 + 1],gra[index[7] * 3 + 2] } }		//7
				};
				Cnt += gen_tetra_tri(triList, Tetrahedron[0], isoVal, vertVal, vert);
				Cnt += gen_tetra_tri(triList, Tetrahedron[1], isoVal, vertVal, vert);
				Cnt += gen_tetra_tri(triList, Tetrahedron[2], isoVal, vertVal, vert);
				Cnt += gen_tetra_tri(triList, Tetrahedron[3], isoVal, vertVal, vert);
				Cnt += gen_tetra_tri(triList, Tetrahedron[4], isoVal, vertVal, vert);
				Cnt += gen_tetra_tri(triList, Tetrahedron[5], isoVal, vertVal, vert);
			}
	return Cnt;
		
	
}

int gen_tetra_tri(Triangle_t** triList, const int index[4], const float isoVal, const float vertVal[8], const Point_t P[8]) {
	Triangle_t* tri;
	unsigned char tetraStat = 0;
	//compute tetraStat for each input tetrahedra
	if (vertVal[index[0]] < isoVal)tetraStat |= 1;
	if (vertVal[index[1]] < isoVal)tetraStat |= 2;
	if (vertVal[index[2]] < isoVal)tetraStat |= 4;
	if (vertVal[index[3]] < isoVal)tetraStat |= 8;
	//each case refers to its case
	switch (tetraStat) {
	case 0x00:
	case 0x0F:
		return 0;
		break;
	case 0x01:
	case 0x0E:
		tri = (Triangle_t*)calloc(1, sizeof(Triangle_t));
		tri->p[0] = bi_linear_interpolate(isoVal, vertVal[index[0]], P[index[0]], vertVal[index[1]], P[index[1]]);
		tri->p[1] = bi_linear_interpolate(isoVal, vertVal[index[0]], P[index[0]], vertVal[index[2]], P[index[2]]);
		tri->p[2] = bi_linear_interpolate(isoVal, vertVal[index[0]], P[index[0]], vertVal[index[3]], P[index[3]]);
		tri->next = *triList;
		*triList = tri;
		return 1;
		break;
	case 0x02:
	case 0x0D:
		tri = (Triangle_t*)calloc(1, sizeof(Triangle_t));
		tri->p[0] = bi_linear_interpolate(isoVal, vertVal[index[1]], P[index[1]], vertVal[index[0]], P[index[0]]);
		tri->p[1] = bi_linear_interpolate(isoVal, vertVal[index[1]], P[index[1]], vertVal[index[2]], P[index[2]]);
		tri->p[2] = bi_linear_interpolate(isoVal, vertVal[index[1]], P[index[1]], vertVal[index[3]], P[index[3]]);
		tri->next = *triList;
		*triList = tri;
		return 1;
		break;
	case 0x03:
	case 0x0C:
		tri = (Triangle_t*)calloc(1, sizeof(Triangle_t));
		tri->p[0] = bi_linear_interpolate(isoVal, vertVal[index[0]], P[index[0]], vertVal[index[2]], P[index[2]]);
		tri->p[1] = bi_linear_interpolate(isoVal, vertVal[index[1]], P[index[1]], vertVal[index[3]], P[index[3]]);
		tri->p[2] = bi_linear_interpolate(isoVal, vertVal[index[0]], P[index[0]], vertVal[index[3]], P[index[3]]);
		tri->next = *triList;
		*triList = tri;
		tri = (Triangle_t*)calloc(1, sizeof(Triangle_t));
		tri->p[0] = (*triList)->p[0];
		tri->p[1] = (*triList)->p[1];
		tri->p[2] = bi_linear_interpolate(isoVal, vertVal[index[1]], P[index[1]], vertVal[index[2]], P[index[2]]);
		tri->next = *triList;
		*triList = tri;
		return 2;
		break;
	case 0x04:
	case 0x0B:
		tri = (Triangle_t*)calloc(1, sizeof(Triangle_t));
		tri->p[0] = bi_linear_interpolate(isoVal, vertVal[index[2]], P[index[2]], vertVal[index[0]], P[index[0]]);
		tri->p[1] = bi_linear_interpolate(isoVal, vertVal[index[2]], P[index[2]], vertVal[index[1]], P[index[1]]);
		tri->p[2] = bi_linear_interpolate(isoVal, vertVal[index[2]], P[index[2]], vertVal[index[3]], P[index[3]]);
		tri->next = *triList;
		*triList = tri;
		return 1;
		break;
	case 0x05:
	case 0x0A:
		tri = (Triangle_t*)calloc(1, sizeof(Triangle_t));
		tri->p[0] = bi_linear_interpolate(isoVal, vertVal[index[0]], P[index[0]], vertVal[index[1]], P[index[1]]);
		tri->p[1] = bi_linear_interpolate(isoVal, vertVal[index[2]], P[index[2]], vertVal[index[3]], P[index[3]]);
		tri->p[2] = bi_linear_interpolate(isoVal, vertVal[index[0]], P[index[0]], vertVal[index[3]], P[index[3]]);
		tri->next = *triList;
		*triList = tri;
		tri = (Triangle_t*)calloc(1, sizeof(Triangle_t));
		tri->p[0] = (*triList)->p[0];
		tri->p[1] = (*triList)->p[1];
		tri->p[2] = bi_linear_interpolate(isoVal, vertVal[index[1]], P[index[1]], vertVal[index[2]], P[index[2]]);
		tri->next = *triList;
		*triList = tri;
		return 2;
		break;
	case 0x06:
	case 0x09:
		tri = (Triangle_t*)calloc(1, sizeof(Triangle_t));
		tri->p[0] = bi_linear_interpolate(isoVal, vertVal[index[0]], P[index[0]], vertVal[index[2]], P[index[2]]);
		tri->p[1] = bi_linear_interpolate(isoVal, vertVal[index[1]], P[index[1]], vertVal[index[3]], P[index[3]]);
		tri->p[2] = bi_linear_interpolate(isoVal, vertVal[index[0]], P[index[0]], vertVal[index[1]], P[index[1]]);
		tri->next = *triList;
		*triList = tri;
		tri = (Triangle_t*)calloc(1, sizeof(Triangle_t));
		tri->p[0] = (*triList)->p[0];
		tri->p[1] = (*triList)->p[1];
		tri->p[2] = bi_linear_interpolate(isoVal, vertVal[index[2]], P[index[2]], vertVal[index[3]], P[index[3]]);
		tri->next = *triList;
		*triList = tri;
		return 2;
		break;
	case 0x07:
	case 0x08:
		tri = (Triangle_t*)calloc(1, sizeof(Triangle_t));
		tri->p[0] = bi_linear_interpolate(isoVal, vertVal[index[3]], P[index[3]], vertVal[index[0]], P[index[0]]);
		tri->p[1] = bi_linear_interpolate(isoVal, vertVal[index[3]], P[index[3]], vertVal[index[1]], P[index[1]]);
		tri->p[2] = bi_linear_interpolate(isoVal, vertVal[index[3]], P[index[3]], vertVal[index[2]], P[index[2]]);
		tri->next = *triList;
		*triList = tri;
		return 1;
		break;

	}

}
