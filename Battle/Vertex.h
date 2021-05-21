#ifndef __VERTEX_H_
#define __VERTEX_H_

typedef struct
{
	float position[3];
	float color[4];
	unsigned int rindex[4];
} Vertex;

void SetVertexData(Vertex* pV, float x, float y, float z, float cr, float cg, float cb, float ca, unsigned int rindex);

#endif