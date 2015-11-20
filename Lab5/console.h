#include <QImage>
#include <QRgb>

/**
 * Log messages to the display and to the log file.
 */
void console_printf(char *format, ...);

/**
 * Read from the console input.
 * Block until input is read.
 */
int console_scanf(char *format, ...);

/**
 * Request mouse input.
 * Returns the mouse color and location, relative to an image.
 * Returns row=column=-1 if the selected point is not in an image.
 */
void console_mselect(int *row, int *column, QRgb *pix);

/**
 * Load a new image to process
 */
void console_loadImage();

/**
 * Segment, and threshold the loaded image
 */
void console_processImage();

/**
 * Report a windows error and exit
 */
void console_abort(char *src);

/**
 * Close all open files and exit.
 */
void console_exit();

/**
 * Functions to be implemented in the lab
 */
void lab_pick(int row, int column, QRgb pix);
void lab_place(int row, int column, QRgb pix);
void thresholdImage(QImage *src, QImage *dst);
void associateObjects(QImage *src, QImage *dst);
