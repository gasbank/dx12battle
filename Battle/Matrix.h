#ifndef __MATRIX_H_
#define __MATRIX_H_

void MatMult(float c[16], float a[16], float b[16]);
void MatTranslate(float a[16], float x, float y, float z);
void MatRotX(float a[16], float t);
void MatRotY(float a[16], float t);
void MatRotZ(float a[16], float t);

#endif
