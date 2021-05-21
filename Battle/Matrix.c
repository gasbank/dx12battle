#include <memory.h>
#include "Matrix.h"

void MatMult(float c[16], float a[16], float b[16])
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			float v = 0;
			for (int k = 0; k < 4; k++)
			{
				v += a[4 * i + k] * b[4 * k + j];
			}

			c[4 * i + j] = v;
		}
	}
}

void MatTranslate(float a[16], float x, float y, float z)
{
	float m[] = {
				 1.0f, 0.0f, 0.0f, x,
				 0.0f, 1.0f, 0.0f, y,
				 0.0f, 0.0f, 1.0f, z,
				 0,      0, 0, 1.0f,
	};
	memcpy(a, m, sizeof(m));
}