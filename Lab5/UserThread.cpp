#include "UserThread.h"

#include "console.h"
#include "remote.h"

// Process user commands as they are entered
void UserThread::run()
{
	scanfWaiting=false;
	
	while(1)
	{
		// wait for a string to process
		mutex.lock();
		if(commandQ.isEmpty()) inputCond.wait(&mutex);
		// make a local copy of the string
		QString command=commandQ.dequeue();
		mutex.unlock();

		// process the command string
		parseline(command.toAscii().data());
	}
}

QString UserThread::getLine()
{
	// Check if any strings are in the queue
	mutex.lock();
	if(commandQ.isEmpty())
	{
		scanfWaiting=true;
		scanfCond.wait(&mutex);
	}
	
	QString input=commandQ.dequeue();
	mutex.unlock();
	
	return input;
}

void UserThread::loadImage()
{
	// wait for a new frame to be processed
	mutex.lock();
	loadImageCond.wait(&mutex);
	mutex.unlock();
}

void UserThread::printf(char *message)
{
	emit showText(message);
}

void UserThread::mselect(int *row, int *column, QRgb *pix)
{
	// wait for a mouse press
	mutex.lock();
	mousePressCond.wait(&mutex);
	
	*row=mouserow;
	*column=mousecol;
	*pix=mousepix;
	mutex.unlock();
}

void UserThread::textInput(const QString& text)
{
	// process the text
	mutex.lock();
	commandQ.enqueue(text);
	mutex.unlock();

	// process this command, or fulfill a scanf?
	if(scanfWaiting)
	{
		scanfWaiting=false;
		scanfCond.wakeOne();
	}
	else
	{
		inputCond.wakeOne();
	}
	
	if(!isRunning()) start();

}

void UserThread::mousePress(int mousex, int mousey, QRgb pix)
{
	mutex.lock();
	// store the 
	mouserow=mousey;
	mousecol=mousex;
	mousepix=pix;
	
	mousePressCond.wakeOne();
	mutex.unlock();
}

void UserThread::processImages(QImage *color, QImage *gray, QImage *bottomLeft, QImage *bottomRight)
{
	// Threshold the image
	thresholdImage(gray, bottomLeft);
	
	// Segment the image
	associateObjects(bottomLeft, bottomRight);
	
	loadImageCond.wakeOne();
	emit imagesProcessed();
}
