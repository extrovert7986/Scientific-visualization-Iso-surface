#include<math.h>
#include<stdlib.h>
#include"Triangle.h"
#include"RawInfoType.h"
#include"Table.h"
#include"Interpolation.h"

unsigned char compute_cellStat(float vertVal[8], const int idx[8]
	, const float isoVal, const float* data);

int marching_cube_genTri(Triangle_t** triList, const float* data, const float isoVal, const float* gra, const rawInf_t rawinf) {
	unsigned char cellStat;
	float vertVal[8];			//vert value in each cell 
	Point_t edgeVert[12];	//interpolate position on edge within each cell
	int triCnt = 0;

	double shiftX, shiftY, shiftZ;
	shiftX = -(rawinf.res.x / 2.0); 
	shiftY = -(rawinf.res.y / 2.0); 
	shiftZ = -(rawinf.res.z / 2.0);
	
	//for each cell
	for (int k = 0; k < rawinf.res.z - 1; k++)
		for (int j = 0; j < rawinf.res.y - 1; j++)
			for (int i = 0; i < rawinf.res.x - 1; i++) {
				//check table by the state
				int index[8] = {
					(i)     + (j)    *rawinf.res.x + (k)    *rawinf.res.x*rawinf.res.y,
					(i + 1) + (j)    *rawinf.res.x + (k)    *rawinf.res.x*rawinf.res.y,
					(i + 1) + (j + 1)*rawinf.res.x + (k)    *rawinf.res.x*rawinf.res.y,
					(i)     + (j + 1)*rawinf.res.x + (k)    *rawinf.res.x*rawinf.res.y,
					(i)     + (j)    *rawinf.res.x + (k + 1)*rawinf.res.x*rawinf.res.y,
					(i + 1) + (j)    *rawinf.res.x + (k + 1)*rawinf.res.x*rawinf.res.y,
					(i + 1) + (j + 1)*rawinf.res.x + (k + 1)*rawinf.res.x*rawinf.res.y,
					(i)     + (j + 1)*rawinf.res.x + (k + 1)*rawinf.res.x*rawinf.res.y
				};
				cellStat = compute_cellStat(vertVal, index, isoVal, data);
				if (edgeTable[cellStat] == 0)continue;
				//interpolate point location by the edge table
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
				if (edgeTable[cellStat] & 1)
					edgeVert[0] = bi_linear_interpolate(isoVal, vertVal[0], vert[0], vertVal[1], vert[1]);
				if (edgeTable[cellStat] & 2)
					edgeVert[1] = bi_linear_interpolate(isoVal, vertVal[1], vert[1], vertVal[2], vert[2]);
				if (edgeTable[cellStat] & 4)
					edgeVert[2] = bi_linear_interpolate(isoVal, vertVal[2], vert[2], vertVal[3], vert[3]);
				if (edgeTable[cellStat] & 8)
					edgeVert[3] = bi_linear_interpolate(isoVal, vertVal[3], vert[3], vertVal[0], vert[0]);
				if (edgeTable[cellStat] & 16)
					edgeVert[4] = bi_linear_interpolate(isoVal, vertVal[4], vert[4], vertVal[5], vert[5]);
				if (edgeTable[cellStat] & 32)
					edgeVert[5] = bi_linear_interpolate(isoVal, vertVal[5], vert[5], vertVal[6], vert[6]);
				if (edgeTable[cellStat] & 64)				  
					edgeVert[6] = bi_linear_interpolate(isoVal, vertVal[6], vert[6], vertVal[7], vert[7]);
				if (edgeTable[cellStat] & 128)
					edgeVert[7] = bi_linear_interpolate(isoVal, vertVal[7], vert[7], vertVal[4], vert[4]);
				if (edgeTable[cellStat] & 256)
					edgeVert[8] = bi_linear_interpolate(isoVal, vertVal[0], vert[0], vertVal[4], vert[4]);
				if (edgeTable[cellStat] & 512)
					edgeVert[9] = bi_linear_interpolate(isoVal, vertVal[1], vert[1], vertVal[5], vert[5]);
				if (edgeTable[cellStat] & 1024)
					edgeVert[10] = bi_linear_interpolate(isoVal, vertVal[2], vert[2], vertVal[6], vert[6]);
				if (edgeTable[cellStat] & 2048)
					edgeVert[11] = bi_linear_interpolate(isoVal, vertVal[3], vert[3], vertVal[7], vert[7]);

				for (int idx = 0; triTable[cellStat][idx] != -1; idx += 3) {
					Triangle_t* tri = (Triangle_t*)malloc(sizeof(Triangle_t));
					tri->p[0] = edgeVert[triTable[cellStat][idx]];
					tri->p[1] = edgeVert[triTable[cellStat][idx + 1]];
					tri->p[2] = edgeVert[triTable[cellStat][idx + 2]];
					
					//insert triangle into linked list
					tri->next = *triList;
					*triList = tri;
					triCnt++;
				}
				

			}
	return triCnt;
}

unsigned char compute_cellStat(float vertVal[8]
	, const int idx[8], const float isoVal, const float* data) 
{
	unsigned char cellStat = 0;

	vertVal[0] = data[idx[0]];
	vertVal[1] = data[idx[1]];
	vertVal[2] = data[idx[2]];
	vertVal[3] = data[idx[3]];
	vertVal[4] = data[idx[4]];
	vertVal[5] = data[idx[5]];
	vertVal[6] = data[idx[6]];
	vertVal[7] = data[idx[7]];
	//record inside/outside state
	if (vertVal[0] < isoVal)cellStat |= 1;
	if (vertVal[1] < isoVal)cellStat |= 2;
	if (vertVal[2] < isoVal)cellStat |= 4;
	if (vertVal[3] < isoVal)cellStat |= 8;
	if (vertVal[4] < isoVal)cellStat |= 16;
	if (vertVal[5] < isoVal)cellStat |= 32;
	if (vertVal[6] < isoVal)cellStat |= 64;
	if (vertVal[7] < isoVal)cellStat |= 128;

	return cellStat;
}
