#include "labs.h"
#include "console.h"
#include "rhino.h"
#include <math.h>
#include <stdio.h>

#define M_PI 3.14159

// -----------------------------------------------------------------------------------------
// You will edit the first two functions in this file (thresholdImage and associateObjects)
// for lab 5.  The functions at the bottom of this file are not to be edited until lab 6.
// -----------------------------------------------------------------------------------------

int DEBUG_LABELS=true; // if set to "true" will generate a text file in your directory displaying the contents of the pixel label array 

void thresholdImage(QImage *gray, QImage *threshed)
// For use with Lab 5
// Take a grayscale bitmap and threshold it.
// The image pointer has dimension width by height pixels.
{
	int   totalpixels;
	uchar graylevel;							// think of the uchar datatype as an integer that occupies only one byte in memory.
	uchar *pfirstgraybyte, *pgraybyte; 
	QRgb  *pfirstthreshedrgb;

	totalpixels	  = gray->numBytes()/4;			// total number of pixels in image
	pfirstgraybyte	  = gray->bits();			// address of first byte of pixel data in image gray
	pfirstthreshedrgb = (QRgb*)threshed->bits();// address of first RGBA triplet in image threshed

	int zt=0; // threshold grayscale value 
	int hist [256];
	for (int i = 0 ; i<256 ; i++)
	{
		hist[i]=0;
	}
	for (int i = 0 ; i<totalpixels; i++)
	{
		int pix_val = (*(pfirstgraybyte+4*i)+*(pfirstgraybyte+4*i+1)+*(pfirstgraybyte+4*i+2))/3;
		hist[pix_val]+=1;
	}
	int max = -999;
	for (int i = 0 ; i<256 ; i++)
	{
		int test_z = i;
		double q0=0;
		double q1=0;
		double u0 = 0;
		double u1 = 0;
		int t1 = 0;
		int t0 = 0;
		for (int j=0 ;j<=test_z; j++)
		{
			q0 = q0+hist[j];
			u0 = u0+j*hist[j];
			t0 = t0+hist[j];
		}
		for (int j=test_z+1; j<256 ; j++)
		{
			q1 = q1+hist[j];
			u1 = u1+j*hist[j];
			t1 = t1+hist[j];
		}
		q0 = q0/totalpixels;
		q1 = q1/totalpixels;
		if(t0==0)
			u0 = u0/.0001;
		else
			u0 = u0/t0;
		
		if(t1==0)
			u1 = u1/.0001;
		else
			u1 = u1/t1;
		
		
		double sigmab = q0*(1-q0)*(u0-u1)*(u0-u1);
		if(sigmab>max)
		{
			zt = i;
			max =sigmab;

		}



	}
	 console_printf("The threshold is zt %d\n",zt);
	// 1. build a histogram for the gray image
	// 2. select value of zt that minimizes the within-group variance
//	zt = 127;  	// blantantly wrong, just here as an example
	
	// threshold the image



	for(int i=0; i<totalpixels; i++)
	{
		pgraybyte = pfirstgraybyte+i*4;
		graylevel = *(pgraybyte);		
		if(graylevel>zt) *(pfirstthreshedrgb+i)=0xffffffff; // set rgb to 255 (white)
		else             *(pfirstthreshedrgb+i)=0x00000000; // set rgb to 0   (black)
	}
}




void associateObjects(QImage *threshed, QImage *associated)
// For use with Lab 5
// Assign a unique color to each object in the thresholded image,
// and change all pixels in each object to the color.
{
	int height,width;
	int red, green, blue;
	QRgb *pfirstthreshedrgb, *pfirstassociatedrgb, *rgbaddress;
	QRgb pixel;

	height = threshed->height();
	width = threshed->width();
	pfirstthreshedrgb=(QRgb*)threshed->bits();
	pfirstassociatedrgb=(QRgb*)associated->bits();
	
	// initialize an array of labels, assigning a label number to each pixel in the image
	int ** pixellabel = new int*[height];
	for (int i=0;i<height;i++) {
		pixellabel[i] = new int[width];
	}
	for(int row=0; row<height; row++)
	{
		for(int col=0; col<width; col++)
		{
			// read red, green, blue values of pixel in threshed image
			rgbaddress=pfirstthreshedrgb+row*width+col;
			pixel = *(rgbaddress);
			red   = qRed(pixel);			// note: r=g=b= {255 or 0}
			
			if (red==0x00)		pixellabel[row][col]=0;		// object/foreground
			else				pixellabel[row][col]=-1;	// background
		}
	}

		
	int label[2000];
	int *equiv[2000];
	for (int i = 0 ; i<2000 ; i++)
	{
		label[i]= 0;
		equiv[i] = &label[i];

	}






	int labelnum=1;
	//----------------------------FIRST RASTER SCAN---------------------------------//
	// assign the same label to all pixels in each object, a unique label for each object
	for(int row=0; row<height; row++)
	{
		for(int col=0; col<width; col++)
		{
			int pixel = pixellabel[row][col];
			int left=-1;
			int above =-1;
			if (col !=0)
				left = pixellabel[row][col-1];
			if (row != 0)
				above = pixellabel[row-1][col];
			if (pixel != -1)
			{
				if((left ==-1) && (above==-1))
				{
					pixellabel[row][col]=labelnum;
					label[labelnum]= labelnum;
					labelnum++;
				}
				else if ((left !=-1) && (above ==-1))
					pixellabel[row][col] = left;
				else if ((left==-1) && (above != -1 ))
					pixellabel[row][col] = above;

				else if ((left !=-1 ) && (above != -1))
				{
					int min =0;
					int max =0;
					int smallerbaselabel = -9999;
					if (*(equiv[left])<*(equiv[above]))
						smallerbaselabel = *equiv[left];
					else
						smallerbaselabel= *equiv[above];
					if (smallerbaselabel == *equiv[left])
					{
						min =left;
					}
					else 
					{
						min = above;
					}
					if (min == left)
						max = above;
					else
						max = left;
					pixellabel[row][col] = smallerbaselabel;
					*equiv[max] = *equiv[min];
					equiv[max]=equiv[min];
				}
			}
		}
	}

	//--------------SECOND RASTER
	for (int row = 0 ; row <height ; row ++)
	{
		for (int col = 0 ; col<width; col++)
		{

			int pixel = pixellabel[row][col];
			if (pixel != -1 )
					pixellabel[row][col] = *equiv[pixel];

		}

	}
	
		if (DEBUG_LABELS)
	{
		console_printf("generating label debugging text file RELEASE/DEBUGFILE.TXT");
		FILE *debugfile;
		fopen_s(&debugfile,"debugfile1.txt","w");
		for(int row=0; row<height; row++)
			{
			for(int col=0; col<width; col++)
				fprintf(debugfile,"%3d",pixellabel[row][col]);
			fprintf(debugfile,"\n");
			}		
		fclose(debugfile);
	}

	// determine number of objects in the image
	// specify a unique color for each object
	// assign color to all pixels in each objet
	int arr_objpixcount [2000];
	int arr_objpixcount_cent_x [2000];
	int arr_objpixcount_cent_y [2000];
	int arr_REAL_objpixcount [2000];

	for (int i = 0 ; i<2000; i++)
	{
		arr_objpixcount[i]=0;
		arr_REAL_objpixcount[i]=0;
		arr_objpixcount_cent_x[i]=0;
		arr_objpixcount_cent_y[i]=0;
	}
	for (int row = 0 ; row<height; row++)
	{
		for(int col =0 ; col<width; col++)
		{
			if(pixellabel[row][col] != -1)
			{
				int label = pixellabel[row][col];
				arr_objpixcount[label]++;

			}
		}
	}
	int count=1;
	int numobj=0;
	for (int i = 0 ; i<2000; i++)
	{
		if(arr_objpixcount[i]<500 && arr_objpixcount[i]>120)
		{
			arr_REAL_objpixcount[count] = arr_objpixcount[i];
			arr_objpixcount[i]=count;
			numobj++;
			count++;

		}
		else
		{
			arr_objpixcount[i]=0;
		}
	}
	for (int row = 0 ; row<height; row++)
	{
		for(int col =0 ; col<width; col++)
		{
			if(pixellabel[row][col] !=-1){
			if(arr_objpixcount[pixellabel[row][col]]!=0)
			{
				pixellabel[row][col]= arr_objpixcount[pixellabel[row][col]];
				arr_objpixcount_cent_x[pixellabel[row][col]] += col;
				arr_objpixcount_cent_y[pixellabel[row][col]] += row;
			}
			else
				pixellabel[row][col]=-1;
			}
		}
	}



	for (int i = 1; i < (numobj+1); i++)
	{
		arr_objpixcount_cent_x[i] = arr_objpixcount_cent_x[i]/arr_REAL_objpixcount[i];
		arr_objpixcount_cent_y[i] = arr_objpixcount_cent_y[i]/arr_REAL_objpixcount[i];
		double xw =  arr_objpixcount_cent_x[i]*.0829 - arr_objpixcount_cent_y[i]*.2612+27.5039;
		double yw =  arr_objpixcount_cent_x[i]*-.2639 -arr_objpixcount_cent_y[i]*.0784+73.7124;
		console_printf("Object %d Centroid: (%d, %d), world: %lf %lf\n", i, arr_objpixcount_cent_x[i], arr_objpixcount_cent_y[i],xw,yw);
		
	}


	
	console_printf("The number of Dan Blocks are %d\n", numobj);
	// assign UNIQUE color to each object
	for(int row=0; row<height; row++)
	{
		for(int col=0; col<width; col++)
		{
			
			switch ((pixellabel[row][col]%3)+1)
			{
				
				case 1:
					red    = 255;
					green = 0;
					blue   = 0;
					break;
				case 2:
					red    = 0;
					green = 255;
					blue   = 0;
					break;
				case 3:
					red    = 0;
					green = 0;
					blue   = 255;
					break;
				default:
					red    = 255;
					green = 255;
					blue   = 255;
					break;					
			}
			
			rgbaddress=pfirstassociatedrgb+row*width+col;
			*(rgbaddress)=qRgb(red,green,blue);
		}
	}
	//crosshairs
	for(int row=0; row<height; row++)
	{
		for(int col=0; col<width; col++)
		{
			for (int i = 1; i < (numobj+1); i++)
			{
				if((arr_objpixcount_cent_x[i] == col) || (arr_objpixcount_cent_y[i] == row))
				{
					red = 0;
					green = 0;
					blue = 0;
					rgbaddress=pfirstassociatedrgb+row*width+col;
					*(rgbaddress)=qRgb(red,green,blue);

				}
			}
		}
	}

		// prints the array of pixel labels to a text file RELEASE/DEBUGFILE.TXT for you to view
	if (DEBUG_LABELS)
	{
		console_printf("generating label debugging text file RELEASE/DEBUGFILE.TXT");
		console_printf("Width is %d , Height is %d \n", width, height);
		FILE *debugfile;
		fopen_s(&debugfile,"debugfile2.txt","w");
		for(int row=0; row<height; row++)
			{
			for(int col=0; col<width; col++)
				fprintf(debugfile,"%3d",pixellabel[row][col]);
			fprintf(debugfile,"\n");
			}		
		fclose(debugfile);
	}

	// clean up memory
	for (int i=0;i<height;i++) 
	{
		delete pixellabel[i];
    }
    delete pixellabel;
}

// -----------------------------------------------
// The functions below will be used with lab 6
// You may ignore them while working on lab 5
// -----------------------------------------------


void lab_pick(int row, int column, QRgb pix)
// For use with Lab 6.
// Pick up a block by clicking on the object in the processed image.
{
	console_printf("Picking up the block at (%d,%d); color is (%d,%d,%d)",
		row, column, qRed(pix), qGreen(pix), qBlue(pix));
	float xw =  column*.0829 - row*.2612+27.5039;
	float yw =  column*-.2639 -row*.0784+73.7124;
	//console_printf("The coordinate are : %lf %lf \n",xw,yw);
	lab_movex(xw, yw, 3.75, 0, 0);
	rhino_grip();
	rhino_softhome();
}


void lab_place(int row, int column, QRgb pix)
// For use with Lab 6
// If the robot is holding a block, place it at the designated row and column.
{
	console_printf("Placing the block at (%d,%d); color is (%d,%d,%d)",
		row, column, qRed(pix), qGreen(pix), qBlue(pix));
	float xw =  column*.0829 - row*.2612+27.5039-1.0;
	float yw =  column*-.2639 -row*.0784+73.7124-0.0;
	lab_movex(xw, yw, 3.75, 0, 0);
	rhino_ungrip();
	rhino_softhome();
}


void lab_main()
{
	// nothing here; leave it blank!
}


void lab_help()
{
	console_printf("find   get color information for a pixel in any of the four images\n");
	console_printf("pick   click on an object to direct the Rhino to pick it up\n");
	console_printf("place  click on a location for the Rhino to place the object in its gripper\n");
}

int lab_movex(float xworld, float yworld, float zworld, float pitch, float roll)
// Computes of the the elbow-up inverse kinematics solutions
{
	double x,y,z,theta5,theta4,theta3,theta2,theta1,wx,wy,wz; 

	float cf = 180.0 / M_PI;
	// First, convert xworld, yworld, zworld to coordinates in the base frame
	x= xworld+11.5;
	y= yworld-15.0 ;
	z= zworld;

	// Now the wrist roll
	theta5 = roll;
	
	// the waist angle
	theta1 = atan2(y,x)*180.0/M_PI;
	
	//effective theta
	float theta_eff = 180.0 - (90.0 - pitch) - (180.0/M_PI)*atan2(16.0,1.0);
	printf("theta_eff %f\n",theta_eff);
	// Now the wrist center
	float r1 = sqrt(1.0+256.0);
	wx= x-r1*sin(theta_eff/cf)*cos(atan2(y,x));
	wy= y-r1*sin(theta_eff/cf)*sin(atan2(y,x));
	wz= z+r1*cos(theta_eff/cf);
	printf("x y z  %f %f %f\n",wx ,wy, wz);

	// The arm angles
	float r2 = sqrt(wx*wx + wy*wy);
	float zoff = wz -26.;
	float r2p = sqrt(r2*r2+ zoff*zoff);
	printf("r2 zoff r2p %f %f %f", r2,zoff,r2p );
	theta2= cf*atan2(zoff, r2)+cf*acos(r2p/46.);
	theta3= -2.0*cf*acos(r2p/46.);

	// Finally, the wrist angle
	theta4= pitch-theta2-theta3;

	// View values
	printf("theta1 = %f degrees\n",theta1);
	printf("theta2 = %f degrees\n",theta2);
	printf("theta3 = %f degrees\n",theta3);
	printf("theta4 = %f degrees\n",theta4);
	printf("theta5 = %f degrees\n",theta5);
	//return 0 ;
	// check that your values are good BEFORE uncommenting the following line.
	return lab_angles((float)theta1,(float)theta2,(float)theta3,(float)theta4,(float)theta5);
	
}
int lab_angles(float theta1, float theta2, float theta3, float theta4, float theta5)
{
	float enc_B, enc_C, enc_D, enc_E, enc_F;      // desired encoder values

	/* c programming note:
     * If the first argument in a division is an INTEGER, c will perform
	 * INTEGER division, eleminating any fractional part of the division.
	 *
	 *      float a;
	 *      a = 5/3;  // assigns the integer 1 to a
	 *
	 * To avoid this problem, type cast the first integer as a float in
	 * one of the two following equivalent ways.
	 *
	 *      float a;
	 *      a = 5.0/3;  	// assigns the value 1.66666666 to a
	 *	a = (float)5/3	// also assigns the value 1.6666666 to a
	 *
	 */

	//Implement joint angle to encoder expressions here.

	enc_B=theta5*282.5/90.0;
	enc_C=(786.0/90.0)*(theta4+(theta3-1090.0*90.0/785.0)+(theta2+(1090.0*90.0/785.0)));
	enc_D=(786.0/90.0)*(theta3+(theta2+(1090.0*90.0/785.0))) -1090;
	enc_E=-(785.0)/(90.0)*theta2 + 1090;
	enc_F=theta1 * (390.0)/(90.0);

	printf("Computed encoder values:\nB:%f \nC:%f \nD:%f \nE:%f \nF:%f\n\n",enc_B,enc_C,enc_D,enc_E,enc_F);
	
	return rhino_mmove((int)enc_B, (int)enc_C, (int)enc_D, (int)enc_E, (int)enc_F);
}