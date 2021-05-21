#include "Vertex.h"

void SetVertexData(Vertex* pV, float x, float y, float z, float cr, float cg, float cb, float ca, unsigned int rindex)
{
	pV->position[0] = x;
	pV->position[1] = y;
	pV->position[2] = z;
	pV->color[0] = cr;
	pV->color[1] = cg;
	pV->color[2] = cb;
	pV->color[3] = ca;
	pV->rindex[0] = pV->rindex[1] = pV->rindex[2] = pV->rindex[3] = rindex;
}
