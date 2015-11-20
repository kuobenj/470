
#include <math.h>
#include <stdio.h>
#include "rhino.h"
#include "labs.h"

/**
 * This function just prints a sample banner when you start the program.
 * If you don't use this function, please have your program print a banner like it.
 * You must COMMENT OUT lab_banner() in lab3.cpp
 */ 
void lab_banner()
{
	printf("Robotics Lab 4: Inverse Kinematics\n");
	printf("Lab partners: Barney Rubble, Fred Flintstone\n");
}


/*
 * For floating point arithmetic, here are the basic trig functions:
 * sin(x), asin(x), cos(x), acos(x), tan(x), atan2(y,x)
 *
 * NOTICE THAT THE ARGUMENTS OF ATAN2 ARE REVERSED FROM THE TEXT!
 *
 * For pi, you can use the constant M_PI=3.14159...
 *
 * In c, the carot "5^6" will not give you an exponent.  Instead,
 * write 5*5*5*5*5*5 or pow(5,6)
 */

/* Use care when working with angles on this lab.
 * The angles coming in are in DEGREES.
 * The angles used by the trig functions are in RADIANS.
 * The angles going out are in DEGREES.
 */






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