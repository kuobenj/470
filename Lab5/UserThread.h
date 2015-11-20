/**
 * Run the user's lab in a separate thread to avoid blocking issues.
 */
#ifndef USERTHREAD_H
#define USERTHREAD_H

#include <QMutex>
#include <QQueue>
#include <QImage>
#include <QRgb>
#include <QString>
#include <QThread>
#include <QWaitCondition>


class UserThread : public QThread
{
	Q_OBJECT
	
	public:
		void run();
		QString getLine(); // block until a new text line is entered
		void loadImage(); // block until an image is loaded
		void printf(char *message);
		void mselect(int *row, int *column, QRgb *pix);
	
	public slots:
		void processImages(QImage *color, QImage *gray, QImage *bottomLeft, QImage *bottomRight); // Tell UserThread there are new images ready to be processed
		void textInput(const QString& text);
		void mousePress(int mousex, int mousey, QRgb pix);
	
	signals:
		void imagesProcessed(); // Report that the images have been processed
		void showText(const QString& text);
	
	private:
		QQueue<QString> commandQ;
		QString scanfBuf;
		QMutex mutex;
		QWaitCondition scanfCond, inputCond, loadImageCond, mousePressCond;
		bool scanfWaiting;
		int mouserow, mousecol;
		QRgb mousepix;
		
//		char command[1024];
};

#endif
