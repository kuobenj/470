#ifndef LABS_H
#define LABS_H

void lab_help();
void lab_main();
void lab_banner();

#ifdef CARTESIAN

		int lab_movex(float xworld, float yworld, float zworld, float pitch, float roll);
#endif


#ifdef VISION

#include <QRgb>
#include <QImage>
void lab_pick(int row, int column, QRgb pix);
void lab_place(int row, int column, QRgb pix);
void thresholdImage(QImage *gray, QImage *threshed);
void associateObjects(QImage *threshed, QImage *associated);
#endif

#endif
