//#include "rhino.h"
//#include <stdio.h>

#include <stdio.h>
#include <windows.h>
#include <wchar.h>
#include "rhino.h"
#include "labs.h"

/**
 * This starter file should help you do lab 2.
 *
 * Compile instructions:
 * click the green run button
 * 
 */

/**
 * This function just prints a sample banner when you start the program.
 * If you don't use this function, please have your program print a banner like it.
 */
void lab_banner()
{
	// Sample banner
	printf("Robotics Lab 2: The Tower of Hanoi\n");
	printf("Lab partners: Barney Rubble, Fred Flintstone\n");
}

/**
 * If you want, put a description of what your program does and how to use it in here.
 * (purely optional)
 */
void lab_help()
{
	printf("No instructions available.\n");
}

void to_safety()
{
	rhino_move('E',630);
	rhino_move('D',-370);
}

void to_level3()
{

	rhino_move('D',-530+53-34);
	rhino_move('E',816-16);
}
//
void to_level2()
{
	rhino_move('D',-430-36);
	rhino_move('E',754-16);
}


void to_level1()
{
	rhino_move('D',-395-26);
	rhino_move('E',703-17);
}

void to_level1B()
{
	rhino_move('E',700);
	rhino_move('D',-400);
	
}

void to_colA()
{
	rhino_move('F',-100);
}

void to_colB()
{
	rhino_move('F',-20);
}

void to_colC()
{
	rhino_move('F',40);
}

/**
 * Place your lab demo in this function.
 * When you run the program and type "lab", it will call this function.
 */
void lab_main()
{
	char buffer[100];
	int count;

	// Read in some input - a simple user interface.
	
	// Read in some input - a simple user interface.
	int source, dest, wait=TRUE;
	int iterm;
	int choice;
	while(wait)
	{
		buffer[0]='\0'; // clear the buffer

		printf("Enter a number between 1 and 3. For source column.");
		fgets(buffer, 100, stdin);
		count=sscanf_s(buffer, "%d", &choice);

		if(count==0 || count==EOF)
		{
			printf("No number read; try again.\n");
			continue; // skip the rest of this loop
		}

		// validate the input
		if(1>choice || choice>3)
		{
			printf("%d is a bad number - try again.\n", choice);
		}
		else
		{
			// quit waiting for good input
			source=choice;
			wait=FALSE;
		}
	}
	wait = TRUE;
	while(wait)
	{
		buffer[0]='\0'; // clear the buffer

		printf("Enter a number between 1 and 3. For dest column.");
		fgets(buffer, 100, stdin);
		count=sscanf_s(buffer, "%d", &choice);

		if(count==0 || count==EOF)
		{
			printf("No number read; try again.\n");
			continue; // skip the rest of this loop
		}

		// validate the input
		if(1>choice || choice>3)
		{
			printf("%d is a bad number - try again.\n", choice);
		}
		else
		{
			// quit waiting for good input
			printf("The choice is %d: \n",choice);
			dest=choice;
			printf("The choice is %d: \n",dest);
			wait=FALSE;
		}
	}
if (source + dest ==3)
		iterm =3;
	else if (source + dest ==5)
		iterm =1;
	else
		iterm = 2;

// Moving from  pos A to C
	rhino_softhome();

	to_safety();
if(source == 1)
		to_colA();
	else if (source == 2)
		to_colB();
	else if(source == 3)
		to_colC();
	to_level1();
	rhino_grip();
	to_safety();
	if(dest==3)
		to_colC();
	else if (dest==2)
		to_colB();
	else if (dest ==1)
		to_colA();

	to_level3();
	rhino_ungrip();
	
	to_safety();
	if(source == 1)
		to_colA();
	else if (source == 2)
		to_colB();
	else if(source == 3)
		to_colC();
	to_level2();
	rhino_grip();
	to_safety();
	if (iterm==2)
		to_colB();
	else if (iterm==1)
		to_colA();
	else if (iterm==3)
		to_colC();
	to_level3();
	rhino_ungrip();

	to_safety();
	if(dest==3)
		to_colC();
	else if (dest==2)
		to_colB();
	else if (dest ==1)
		to_colA();
	
	to_level3();
	rhino_grip();
	to_safety();
	
	if (iterm==2)
		to_colB();
	else if (iterm==1)
		to_colA();
	else if (iterm==3)
		to_colC();
	to_level2();
	rhino_ungrip();

	to_safety();
	if(source == 1)
		to_colA();
	else if (source == 2)
		to_colB();
	else if(source == 3)
		to_colC();
	to_level3();
	rhino_grip();
	to_safety();
	if(dest==3)
		to_colC();
	else if (dest==2)
		to_colB();
	else if (dest ==1)
		to_colA();
	
	to_level3();
	rhino_ungrip();

	to_safety();
	
	if (iterm==2)
		to_colB();
	else if (iterm==1)
		to_colA();
	else if (iterm==3)
		to_colC();
	to_level2();
	rhino_grip();
	to_safety();
	if(source == 1)
		to_colA();
	else if (source == 2)
		to_colB();
	else if(source == 3)
		to_colC();
	to_level3();
	rhino_ungrip();

	to_safety();
	
	if (iterm==2)
		to_colB();
	else if (iterm==1)
		to_colA();
	else if (iterm==3)
		to_colC();
	to_level3();
	rhino_grip();
	to_safety();
	if(dest==3)
		to_colC();
	else if (dest==2)
		to_colB();
	else if (dest ==1)
		to_colA();
	
	to_level2();
	rhino_ungrip();
	
	to_safety();
	if(source == 1)
		to_colA();
	else if (source == 2)
		to_colB();
	else if(source == 3)
		to_colC();
	to_level3();
	rhino_grip();
	to_safety();
	
	if(dest==3)
		to_colC();
	else if (dest==2)
		to_colB();
	else if (dest ==1)
		to_colA();
	
	to_level1();
	rhino_ungrip();
	to_safety();




	return;
}

