#include "Rectangle.h"
#include <stdlib.h>

Vertex* AllocRectangularGrid(int xCount, int yCount, unsigned int* s)
{
	if (xCount < 1 || yCount < 1)
	{
		*s = 0;
		return NULL;
	}

	int vertexCount = xCount * yCount * 4 * 3;
	Vertex* v = malloc(sizeof(Vertex) * vertexCount);
	float dx = 2.0f / xCount;
	float dy = 2.0f / yCount;
	Vertex* pV = v;
	unsigned int rIndex = 0;
	for (int x = 0; x < xCount; x++)
	{
		for (int y = 0; y < yCount; y++)
		{
			float xc = (-1.0f + dx / 2 + dx * x);
			float yc = (+1.0f - dy / 2 - dy * y);

			int dev = 50;

			float r0 = (33 + (rand() % dev - dev / 2)) / 255.0f;
			float g0 = (27 + (rand() % dev - dev / 2)) / 255.0f;
			float b0 = (102 + (rand() % dev - dev / 2)) / 255.0f;

			float r1 = (78 + (rand() % dev - dev / 2)) / 255.0f;
			float g1 = (63 + (rand() % dev - dev / 2)) / 255.0f;
			float b1 = (242 + (rand() % dev - dev / 2)) / 255.0f;

			float r2 = (53 + (rand() % dev - dev / 2)) / 255.0f;
			float g2 = (43 + (rand() % dev - dev / 2)) / 255.0f;
			float b2 = (166 + (rand() % dev - dev / 2)) / 255.0f;

			float r3 = (66 + (rand() % dev - dev / 2)) / 255.0f;
			float g3 = (53 + (rand() % dev - dev / 2)) / 255.0f;
			float b3 = (204 + (rand() % dev - dev / 2)) / 255.0f;

			float r4 = (75 + (rand() % dev - dev / 2)) / 255.0f;
			float g4 = (60 + (rand() % dev - dev / 2)) / 255.0f;
			float b4 = (230 + (rand() % dev - dev / 2)) / 255.0f;


			// 동
			SetVertexData(pV, xc, yc, 0, r3, g3, b3, 1, rIndex);
			pV++;
			SetVertexData(pV, xc + dx / 2, yc + dy / 2, 0, r1, g1, b1, 1, rIndex);
			pV++;
			SetVertexData(pV, xc + dx / 2, yc - dy / 2, 0, r2, g2, b2, 1, rIndex);
			pV++;

			// 서
			SetVertexData(pV, xc, yc, 0, r3, g3, b3, 1, rIndex);
			pV++;
			SetVertexData(pV, xc - dx / 2, yc - dy / 2, 0, r3, g3, b3, 1, rIndex);
			pV++;
			SetVertexData(pV, xc - dx / 2, yc + dy / 2, 0, r4, g4, b4, 1, rIndex);
			pV++;

			// 남
			SetVertexData(pV, xc, yc, 0, r3, g3, b3, 1, rIndex);
			pV++;
			SetVertexData(pV, xc + dx / 2, yc - dy / 2, 0, r2, g2, b2, 1, rIndex);
			pV++;
			SetVertexData(pV, xc - dx / 2, yc - dy / 2, 0, r3, g3, b3, 1, rIndex);
			pV++;

			// 북
			SetVertexData(pV, xc, yc, 0, r3, g3, b3, 1, rIndex);
			pV++;
			SetVertexData(pV, xc - dx / 2, yc + dy / 2, 0, r4, g4, b4, 1, rIndex);
			pV++;
			SetVertexData(pV, xc + dx / 2, yc + dy / 2, 0, r1, g1, b1, 1, rIndex);
			pV++;

			rIndex = (rIndex + 1) % 48;
		}
	}
	*s = sizeof(Vertex) * xCount * yCount * 4 * 3;
	return v;
}