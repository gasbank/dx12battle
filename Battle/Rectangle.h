#ifndef __RECTANGLE_H_
#define __RECTANGLE_H_

#include "Vertex.h"

Vertex* AllocRectangularGrid(int xCount, int yCount, unsigned int* s);
Vertex* AllocRectangularGridDoubleSided(int xCount, int yCount, unsigned int* s);

#endif // !__RECTANGLE_H_
