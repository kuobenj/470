#ifndef LABS_H
#define LABS_H

void lab_help();
void lab_main();

#ifdef VISION

#include <QRgb>
#include <QImage>
void lab_pick(int row, int column, QRgb pix);
void lab_place(int row, int column, QRgb pix);
void thresholdImage(QImage *gray, QImage *threshed);
void associateObjects(QImage *threshed, QImage *associated);
int lab_movex(float xworld, float yworld, float zworld, float pitch, float roll);
int lab_angles(float theta1, float theta2, float theta3, float theta4, float theta5);

#endif

#endif
