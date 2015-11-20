// QT class for the console GUI
#ifndef CONSOLEGUI_H
#define CONSOLEGUI_H

#include "cxtypes.h"
#include "cvtypes.h"
#include "highgui.h"

#include <QImage>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QTextEdit>
#include <QTimer>
#include <QWidget>

class ConsoleGUI : public QWidget
{
	Q_OBJECT // macro needed for QT goodness
	
	public:
		ConsoleGUI();
		static void grayCopy(QImage *src, QImage *dest); // copy dest=grayscale(src)
	
	private:
		QImage *images[4]; // the four pictures to be displayed
		QLabel *views[4]; // labels to paint the pictures in
		QLineEdit *commandLine; // box for text entry
		QTextEdit *consoleView; // scrolling window for entry history and display results
		QTimer *clk; // timer for "real-time" capture of pictures
		int rate; // how fast to update the camera
		CvCapture *camera;
		QPushButton *playButton; // start and stop the RT capture
		QString picturefile; // Path to the last opened picture
		bool foundCamera; // whether a camera was detected
	
	signals:
		void textEntered(const QString& text); // emit whenever the Enter key is pressed in the commandline
		void mousePressed(int mousex, int mousey, QRgb pix); // emit whenever the mouse is pressed over an image
		void imagesReady(QImage *color, QImage *gray, QImage *bottomLeft, QImage *bottomRight); // signal that a new frame has been captured from the camera or loaded from a file
	
	public slots:
		void receiveEnter();
		void displayLine(const QString& text); // adds a line of text to the console log
		void cleanExit(); // equivalent to consoleExit(0)
		void consoleExit(int status); // call to close the program
		void snapPicture(); // take another picture
		void loadPicture(); // load a picture from a file
		void toggleAutoSnap(); // toggle the auto-snap feature
		void refreshPictures(); // refresh the on-screen picture display
	
	protected:
		bool eventFilter(QObject *obj, QEvent *event); // Handle mouse clicks on the pictures
};

#endif
