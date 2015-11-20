/**
 * A simple "remote control" utility for the Rhino.
 * This file can also be used as an interface for your lab projects by using the -DLAB flag on the compile line.
 */
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rhino.h"
#include "remote.h"
#include "labs.h"

//#include "console.h"
#ifdef VISION
//#include <magick/api.h>
#endif

char *STATES[3]={"open", "closed", "ERROR!"};
char * readswitch(char motor)
{
	int value;
	value=rhino_switch(motor);
	if(0>value || value>1)
	{
		return STATES[2];
	}

	return STATES[value];
}

void parseline(const char *buffer)
{
	int count, value;
	char command[BIG], arg[BIG];
	
//console_printf("parseline(%s)", buffer);

	count=sscanf_s(buffer, "%s", command,sizeof(command));

	if(count==0 || count==EOF)
	{
		return;
	}

	if(!_stricmp(command, "quit") || !_stricmp(command, "exit"))
	{
		PRINTF("Returning home.\n");
		PRINTF("Shutting down.\n");
		rhino_softhome();
		rhino_closecomm();
		exit(0);
	}
	else if(!_stricmp(command, "help"))
	{
		PRINTF("Commands:\n");
		PRINTF(" help - print this message\n");
		PRINTF(" quit/exit - exit the program\n");
		PRINTF(" grip/ungrip - open or close the gripper\n");
		PRINTF(" move b c d e f - move motors b-f to the specified encoder locations\n");
#ifdef ANGLES
		PRINTF(" angles b c d e f - move motors b-f to the specified angles (in degrees)\n");
#endif
#ifdef CARTESIAN
		PRINTF(" movex x y z pitch roll - move the Rhino to the specified position and orientation (in degrees)\n");
#endif
		PRINTF(" show - display the motor positons\n");
		PRINTF(" softhome - return the Rhino to its home position\n");
#ifdef VISION
		PRINTF(" load - load an image to process.\n");
		PRINTF(" find - display the x,y,color info for a point selected with the mouse.\n");
		PRINTF(" pick/place - move a block that is selected with the mouse.\n");
#endif
#ifdef LAB
		PRINTF(" lab - run your lab demo.\n");
#endif
		PRINTF("\n");
		PRINTF("To move a single motor, type the letter of the motor followed by the destination.\n");
		PRINTF(" Example: \"c -50\"\n");
		PRINTF("\n");
	}
	else if(!_stricmp(command, "grip"))
	{
		PRINTF("Holding object of size %d\n", rhino_grip());
	}
	else if(!_stricmp(command, "ungrip"))
	{
		rhino_ungrip();
	}
	else if(!_stricmp(command, "move"))
	{
		int b, c, d, e, f;
		count=sscanf_s(buffer, " %*s %d %d %d %d %d ", &b, &c, &d, &e, &f);

		if(count<JOINTS)
		{
			PRINTF("Error: didn't input %d joint positions.\n", JOINTS);
			return;
		}

		rhino_mmove(b, c, d, e, f);
	}
#ifdef ANGLES
	else if(!_stricmp(command, "angles"))
	{
		float b, c, d, e, f;
		count=sscanf_s(buffer, " %*s %f %f %f %f %f ", &b, &c, &d, &e, &f);

		if(count<JOINTS)
		{
			PRINTF("Error: didn't input %d joint positions.\n", JOINTS);
			return;
		}

		lab_angles(b, c, d, e, f);
	}
#endif
#ifdef CARTESIAN
	else if(!_stricmp(command, "movex"))
	{
		float x, y, z, roll, pitch, yaw;
		count=sscanf_s(buffer, " %*s %f %f %f %f %f %f ", &x, &y, &z, &roll, &pitch, &yaw);

		if(count<6)
		{
			PRINTF("Error: didn't input all 6 coordinates.\n");
			return;
		}

		lab_movex(x, y, z, roll, pitch, yaw);
	}
#endif
	else if(!_stricmp(command, "show"))
	{
		PRINTF("Motor positions:\n");
		PRINTF(" A =%5d\tE =%5d\n", rhino_position('A'), rhino_position('E'));
		PRINTF(" B =%5d\tF =%5d\n", rhino_position('B'), rhino_position('F'));
		PRINTF(" C =%5d\tG =%5d\n", rhino_position('C'), rhino_position('G'));
		PRINTF(" D =%5d\tH =%5d\n", rhino_position('D'), rhino_position('H'));
	}
	else if(!_stricmp(command, "softhome"))
	{
		rhino_softhome();
	}
	else if(!_stricmp(command, "hardhome"))
	{
		rhino_hardhome();
	}
	else if(!_stricmp(command, "home"))
	{
		count=sscanf_s(buffer, " %*s %s ", arg, sizeof(arg));

		if(count==0 || count==EOF)
		{
			PRINTF("Error: no motor specified.\n");
			return;
		}

		rhino_home(toupper(arg[0]));
	}
	else if(!_stricmp(command, "switch"))
	{
		count=sscanf_s(buffer, " %*s %s ", arg, sizeof(arg) );

		if(count==0 || count==EOF)
		{
			PRINTF("Error: no motor specified.\n");
			return;
		}

		PRINTF("Switch %c is %s.\n", toupper(arg[0]), readswitch(toupper(arg[0])));
	}
	else if(!_stricmp(command, "switches"))
	{
		PRINTF("Switches:\n");
		PRINTF(" A - %-6s\tE- %-6s\n", readswitch('A'), readswitch('E'));
		PRINTF(" B - %-6s\tF- %-6s\n", readswitch('B'), readswitch('F'));
		PRINTF(" C - %-6s\tG- %-6s\n", readswitch('C'), readswitch('G'));
		PRINTF(" D - %-6s\tH- %-6s\n", readswitch('D'), readswitch('H'));
	}
	else if(strlen(command)==1) // Move an individual motor
	{
		count=sscanf_s(buffer, " %*s %s ", arg, sizeof(arg));

		if(count==0 || count==EOF)
		{
PRINTF("Command: %s", command);
			PRINTF("Error: no destination.\n");
			return;
		}

		value=atoi(arg);
		if(value==0 && strlen(arg)>0 && strcmp(arg, "0"))
		{
			PRINTF("Invalid destination: \"%s\"\n", arg);
		}
		else
		{
			int err;
			err=rhino_move(toupper(command[0]), value);
			if(err)
			{
				PRINTF("Error: didn't move %d clicks.\n", err);
			}
		}
	}
#ifdef VISION
	else if(!_stricmp(command, "load"))
	{
		console_loadImage();
		console_processImage();
	}
	else if(!_stricmp(command, "find"))
	{
		int row, column;
		QRgb pix;
		
		PRINTF("Click on a picture for information about the point.");
		
		console_mselect(&row, &column, &pix);

		PRINTF("row:%d, column:%d, red:%d, green:%d, blue:%d\n",
			row, column, qRed(pix), qGreen(pix), qBlue(pix));
	}
	else if(!_stricmp(command, "pick"))
	{
		PRINTF("Click on a block to pick up. (any picture)");

		int row, column;
		QRgb pix;
		console_mselect(&row, &column, &pix);

		lab_pick(row, column, pix);
	}
	else if(!_stricmp(command, "place"))
	{
		PRINTF("Click on a location to place the block. (any picture)");

		int row, column;
		QRgb pix;
		console_mselect(&row, &column, &pix);

		lab_place(row, column, pix);
	}
	else if(!_stricmp(command, "scanf"))
	{
		//PRINTF("Enter for \%s \%d");
		
		char buf[100];
		int x;
		console_scanf(" %s %d ", buf, &x);
		PRINTF("You entered: %s %d", buf, x);
	}
#endif
#ifdef LAB
	else if(!_stricmp(command, "lab"))
	{
		count=sscanf_s(buffer, " %*s %s ", arg, sizeof(arg) );

		lab_main();
	}
#endif
	else
	{
		PRINTF("Unknown command \"%s\".\n", command);
	}
}


#ifndef VISION
int main(int argc, char **argv)
{
	char buffer[BIG];

	rhino_init();
	rhino_opencomm(1);

#ifdef LAB
	PRINTF("\n");
	lab_banner();
	PRINTF("Type \"help\" for help.\n");
#else
	PRINTF("\nRhino remote control.\nType \"help\" for help.\n");
#endif
	while(1)
	{
		PRINTF("> ");
		fgets(buffer, BIG, stdin);
		parseline(buffer);
	}
}
#endif //VISION
