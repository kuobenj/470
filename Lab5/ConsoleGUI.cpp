// Class for the console GUI

// OpenCV stuff


// QT stuff
#include <QCoreApplication>
#include <QFileDialog>
#include <QGridLayout>
#include <QScrollBar>
#include <QMouseEvent>

#include "ConsoleGUI.h"
#include "console.h" // to trigger console_processImage()


ConsoleGUI::ConsoleGUI() : QWidget()
{
	// Start the camera
	camera=cvCaptureFromCAM(0); // pick the first available camera

	int width, height;
	
	if(camera)
	{
		foundCamera=true;
		
		// capture a test frame to get the width and height
		IplImage *img=cvQueryFrame(camera);
		width=img->width;
		height=img->height;
	}
	else
	{
		foundCamera=false;
		width=320;
		height=240;
	}

	for(int i=0; i<4; i++)
	{
		images[i]=new QImage(width, height, QImage::Format_RGB32);
		views[i]=new QLabel(this);
		views[i]->setPixmap(QPixmap::fromImage(*images[i], 0));
		views[i]->installEventFilter(this); // capture mouse presses
	}
	commandLine=new QLineEdit(this);
	consoleView=new QTextEdit(this);
	clk=new QTimer();
	rate=0; // as fast as possible
	picturefile="";

	
	// use the clock to auto-snap pictures
	QObject::connect(clk, SIGNAL(timeout()), this, SLOT(snapPicture()));
	
	// Set up the buttons and such
	QPushButton *quit=new QPushButton("Quit", this);
	QPushButton *snap=new QPushButton("Snap", this);
	QPushButton *load=new QPushButton("Load", this);
	playButton=new QPushButton("Start", this);
	
	// Keep the buttons from grabbing focus away from the command line
	playButton->setFocusPolicy(Qt::NoFocus);
	snap->setFocusPolicy(Qt::NoFocus);
	load->setFocusPolicy(Qt::NoFocus);
	quit->setFocusPolicy(Qt::NoFocus);
	// And the console view
	consoleView->setFocusPolicy(Qt::NoFocus);
	
	QGridLayout *layout=new QGridLayout(this);
	layout->addWidget(playButton, 0, 0);
	layout->addWidget(snap, 0, 1);
	layout->addWidget(load, 0, 2);
	layout->addWidget(quit, 0, 3);
	layout->addWidget(views[0], 1, 0, 1, 2);
	layout->addWidget(views[1], 1, 2, 1, 2);
	layout->addWidget(views[2], 2, 0, 1, 2);
	layout->addWidget(views[3], 2, 2, 1, 2);

	layout->addWidget(consoleView, 3, 0, 1, 4);
	layout->addWidget(commandLine, 4, 0, 1, 4);
	
	setLayout(layout);

	// set up the signals and slots
	QObject::connect(quit, SIGNAL(clicked()), this, SLOT(cleanExit()));
	QObject::connect(snap, SIGNAL(clicked()), this, SLOT(snapPicture()));
	QObject::connect(load, SIGNAL(clicked()), this, SLOT(loadPicture()));
	QObject::connect(playButton, SIGNAL(clicked()), this, SLOT(toggleAutoSnap()));
	QObject::connect(commandLine, SIGNAL(returnPressed()), this, SLOT(receiveEnter()));
	
	//QObject::connect(views[0], SIGNAL(
}

// Receive the command line's enter event
void ConsoleGUI::receiveEnter()
{
	displayLine("> "+commandLine->text());
	emit textEntered(commandLine->text());
	commandLine->clear();
}

// Add another line to the console
void ConsoleGUI::displayLine(const QString& text)
{
	consoleView->append(text);
	consoleView->verticalScrollBar()->setValue(consoleView->verticalScrollBar()->maximum());
}

void ConsoleGUI::cleanExit()
{
	consoleExit(0);
}

// Exit the application
void ConsoleGUI::consoleExit(int status)
{
	// close the cameras and do any other housework before exiting
	if(clk->isActive())
	{
		clk->stop();
		playButton->setText("Start");
	}

	displayLine("Exiting...");
	
	// release the camera
	cvReleaseCapture(&camera);
	
	QCoreApplication::instance()->exit(status);
}

// Take a picture
void ConsoleGUI::snapPicture()
{
	if(!foundCamera)
	{
		// Don't print this if in auto-timer mode
		if(!clk->isActive()) console_printf("Note: no camera was detected.");
		
		return;
	}
	
	IplImage *image=cvQueryFrame(camera);
	int height=image->height;
	int width=image->width;
	
	// need to check for the right sizes...
	if( (width != images[0]->width()) || (height != images[0]->height()) || (images[0]->format() != QImage::Format_RGB32) )
	{
		images[0]=new QImage(width, height, QImage::Format_RGB32);
		images[1]=new QImage(width, height, QImage::Format_RGB32);
		images[2]=new QImage(width, height, QImage::Format_RGB32);
		images[3]=new QImage(width, height, QImage::Format_RGB32);
	}


	// flip the image and insert padding for the alpha channel
	char *iplptr=image->imageData;
	char *qtptr=(char *)(images[0]->bits());

	char *ybase, *flipybase;
	char *x4, *x3;
	for(int y=0; y<height; y++)
	{
		ybase=qtptr+y*width*4;
		flipybase=iplptr+(height-y)*3*width;
		
		for(int x=0; x<width; x++)
		{
			x4=ybase+4*x;
			x3=flipybase+3*x;
			
			*((QRgb *)x4)=*((QRgb *)x3);
		}
	}

	grayCopy(images[0], images[1]);
	
	emit imagesReady(images[0], images[1], images[2], images[3]);
}

void ConsoleGUI::loadPicture()
{
	QString filename=QFileDialog::getOpenFileName(
		this,
		"Choose a picture to process",
		picturefile, // default path
		"Pictures (*.jpg *.png *.gif)");

	QImage *temp=new QImage(filename);
	

	if( ! temp->isNull() )
	{
		// check that this is a "good" image type
		switch(temp->format())
		{
			case QImage::Format_Invalid:
			case QImage::Format_Mono:
			case QImage::Format_MonoLSB:
			case QImage::Format_Indexed8:
				displayLine("Error: Invalid image format");
				return;
			
			case QImage::Format_RGB32:
			case QImage::Format_ARGB32:
			case QImage::Format_ARGB32_Premultiplied:
				// good
				break;
			
			default:
				displayLine("Error: Unknown image format");
				return;
		}

		picturefile=filename;
		
		int height=temp->height();
		int width=temp->width();
		
		images[0]=new QImage(*temp); // shallow copy
		// need to check for the right image format
		if( (width != images[0]->width()) 
			|| (height != images[0]->height()) 
			|| (images[0]->format() != QImage::Format_RGB32) )
		{
			images[1]=new QImage(width, height, QImage::Format_RGB32);
			images[2]=new QImage(width, height, QImage::Format_RGB32);
			images[3]=new QImage(width, height, QImage::Format_RGB32);
		}
		
		grayCopy(images[0], images[1]);
		emit imagesReady(images[0], images[1], images[2], images[3]);
	}
	else
	{
		if(filename.length()>0)
		{
			displayLine("Error: Could not open "+filename);
		}
		else
		{
			displayLine("Note: No picture selected.");
		}
	}
}

void ConsoleGUI::grayCopy(QImage *src, QImage *dest)
{
	QRgb *color=(QRgb *)src->bits();
	QRgb *gray=(QRgb *)dest->bits();
	int length=(src->numBytes())/4;
	
	uchar level; // gray level of the pixel
	for(int i=0; i<length; i++)
	{
		level=qGray(*(color+i)); 
		
		*(gray+i)=qRgb(level, level, level);
	}
}

void ConsoleGUI::toggleAutoSnap()
{
	if(clk->isActive())
	{
		clk->stop();
		playButton->setText("Start");
	}
	else
	{
		clk->start(rate);
		if(!foundCamera) console_printf("Note: no camera was detected.");
		playButton->setText("Stop");
	}
}

void ConsoleGUI::refreshPictures()
{
	// show the pictures
	views[0]->setPixmap(QPixmap::fromImage(*images[0], 0));
	views[1]->setPixmap(QPixmap::fromImage(*images[1], 0));
	views[2]->setPixmap(QPixmap::fromImage(*images[2], 0));
	views[3]->setPixmap(QPixmap::fromImage(*images[3], 0));
}

bool ConsoleGUI::eventFilter(QObject *obj, QEvent *event)
{
	if(event->type() == QEvent::MouseButtonPress)
	{
		// Check if it is on a picture
		for(int i=0; i<4; i++)
		{
			if(views[i] == (QLabel *)obj)
			{
				// Yell out what was pressed.
				QMouseEvent *me=(QMouseEvent *)event;
				int mousex=me->x();
				int mousey=me->y();
				QRgb pix=images[3]->pixel(mousex, mousey);
				
				emit mousePressed(mousex, mousey, pix);
				
				return true; // stop propagation of this event
			}
		}
	}
	
	// standard event processing
	return QObject::eventFilter(obj, event);
}
