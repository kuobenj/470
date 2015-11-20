/**
 * rhino.c
 * Copyright (C) 2003,2005 Daniel Herring
 * Licensed under GPL version 2
*/

#include <stdio.h>
#include <windows.h>
#include <wchar.h>
#include "rhino.h"

// MS C docs
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vcstdlib/html/vcoriStandardCLibraryReference.asp
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vclib/html/vcrefruntimelibraryreference.asp

// COMM port stuffs
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/devio/base/communications_resources.asp
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dnfiles/html/msdn_serial.asp
HANDLE hcom;
DCB dcb; // http://msdn.microsoft.com/library/default.asp?url=/library/en-us/devio/base/dcb_str.asp

// Holds the encoder positions for each arm.
int positions[8];

// Holds the target encoder positions for each arm.
int destinations[8];

// Which COM port to connect to the Rhino on.  (1-4)
const char* PortStrings[]={"ERROR", "COM1", "COM2", "COM3", "COM4"};

#ifdef RHINO_DEBUG
FILE *logfile=NULL;
#endif



/**
 * Set up variables, etc.
 */
void rhino_init()
{
	#ifdef RHINO_DEBUG
	if(logfile==NULL)
	{
		logfile=fopen("rhino.log", "w");
	}
	else
	{
		fprintf(logfile, "\nRhino: init()\n\n");
	}
	#endif

	int i;
	for(i=0; i<8; i++)
	{
		positions[i]=0;
	}
}

/**
 * Open communications with the Rhino controller.
 * return<0 indicates an error.
 */
int rhino_opencomm(int port)
{
	if(1>port || port>4)
	{
		PRINTF("Invalid port COM%d\n", port);
		exit(0);
	}
	PRINTF("Connecting to Rhino on COM%d\n", port);

	/* Data format:
		RS-232 COM port
		9600 baud
		7-bit data
		even parity
		2 stop bits
	*/

	// This "open comm" routine was copied from the MinGW-users mailing list.
	// I suspect it originated on MSDN...
	// try to open comm port
	hcom = CreateFileA(
						PortStrings[port],
						GENERIC_READ | GENERIC_WRITE,
						0,                               /* com devs must be opeend w/ exclusive-access */
						NULL,                            /* no security attribs */
						OPEN_EXISTING,                   /* necessary for comm ports */
						0,                               /* no overlapped i/o */
						NULL                             /* must be null for comm */
						);

	if(hcom==INVALID_HANDLE_VALUE)
	{
		PRINTF("could not open comm port %d!\n", port);
		exit(-1);
	}
	else
	{
		PRINTF("comm port %d opened.\n", port);
	}

	// Set up the "Device Control Block"
	if(!GetCommState(hcom, &dcb))
	{
		PRINTF("Error reading serial port status.\n");
	}

#ifdef WIN9X
	// Manually configure the DCB, hoping we didn't miss anything important.
	dcb.BaudRate=9600;
	dcb.Parity=EVENPARITY;
	dcb.ByteSize=7;
	dcb.StopBits=TWOSTOPBITS;
#else
	/*  For some strange reason, this crashes on a Win98 box (robot14)*/
	FillMemory(&dcb, sizeof(dcb), 0);
	dcb.DCBlength=sizeof(dcb);
	if(!BuildCommDCBA("9600,e,7,2", &dcb))
	{
		PRINTF("Invalid DCB\n");
		exit(0);
	}
	//also crashes: BuildCommDCB("COM1:9600,E,7,2,CS,DS,CD", &dcb);
	//doesn't crash: BuildCommDCB("", &dcb);
#endif

	// Set up the port properties
	if(!SetCommState(hcom, &dcb))
	{
		PRINTF("Could not configure COMM port.\n");
		exit(0);
	}
	PRINTF("Configured COMM port.\n");

	// Set up some timeouts
	COMMTIMEOUTS time={
		0, /* No read interval timeout */
		100, /* allocate 50ms for each byte to read */
		500, /* allocate extra time for the read response */
		100, /* 50ms for each byte to write */
		500 /* 100ms for each write */
	};
	if(!SetCommTimeouts(hcom, &time))
	{
		PRINTF("Could not configure COMM timeouts.\n");
		exit(0);
	}

	// Reset the controller
	char buffer[100];
	DWORD read;
	sprintf_s(buffer,sizeof(buffer), "Q\r");
	WriteFile(hcom, &buffer, 2, &read, 0);

	// clear the input buffer
	read=1;
	while(read>0)
	{
		ReadFile(hcom, &buffer, 10, &read, 0);
		#ifdef RHINO_DEBUG
		fprintf(logfile, "Cleared %d bytes.\n", (int)read);
		#endif
	}

	// "warm up" the connection
	#ifdef RHINO_DEBUG
	fprintf(logfile, "A: %d, B: %d, C: %d\n\n", rhino_inquire('A'), rhino_inquire('B'), rhino_inquire('C'));
	#endif
	rhino_inquire('A');
	rhino_inquire('B');
	if(rhino_inquire('C')<0)
	{
		PRINTF("Connection not established.\n");
		return -1;
	}
	else
	{
		rhino_home('A'); // Auto-synch the gripper commands
		PRINTF("Connection established.\n");
	}
	return 0;
}

int rhino_closecomm()
{
	CloseHandle(hcom);
	return 0;
}

/**
 * Retrieve the "error register" from the controller.
 * return - <0 indicates an error, otherwise return the error count.
 */
int rhino_inquire(char motor)
{
	if('A'>motor || motor>'H')
	{
		PRINTF("Illegal motor: %c\n", motor);
		return -1;
	}

	int error;
	error=-1; // default to having an error occur.

	// Send the request to the motor
	char buf[10]; // avoid buffer overflows - much bigger than it needs to be.
	sprintf_s(buf,sizeof(buf), "%c?\r", motor);
	DWORD written=0;
	if(WriteFile(hcom, &buf, 3, &written, 0))
	{
		if(written<3)
		{
			PRINTF("Timeout before writing data.\n");
			return -1;
		}
	}
	else
	{
		PRINTF("Error writing to the Rhino.\n");
		return -1;
	}

	//get the error register for the motor.
	// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/fileio/base/readfile.asp
	DWORD read=0;
	if(ReadFile(hcom, &buf, 1, &read, 0))
	{
		if(read==0)
		{
			PRINTF("Timeout before reading data.\n");
			return -1;
		}
		error=((int)buf[0])-32; // Fix the communications offset
	}
	else
	{
		PRINTF("Error reading the motor position.\n");
		return -1;
	}

	return error;
}

/**
 * Move the motor to the desired postion.
 * This function blocks until the move is complete.
 *
 * motor - 'A' through 'H'
 * position - encoder count from the origin
 * return!=0 indicates an error
 */
int rhino_move(char motor, int destination)
{
	// QUICK FIX - until I can sort things out...
	// If we can, use rhino_mmove() to avoid rhino_move() vs rhino_mmove() errors
	if('A'<motor && motor<'G')
	{
		int i, destinations[JOINTS];
		for(i=0; i<JOINTS; i++)
		{
			destinations[i]=positions[i+1];
		}
		destinations[motor-'B']=destination;

		return rhino_ammove(destinations);
	}

	// The polarity of this move
	int sign;
	// The move command to send to the Rhino
	char command;
	// How many encoder clicks we need to move
	int toMove;
	// Count how many cycles occur without any motion
	int noMove;
	// Flag when the motor has completely stalled
//	int stalled;
	// Command buffer
	char buffer[10];
	// How many bytes are actually written to the Rhino
	DWORD written;
	// The latest status of the Rhino error register
	int error;
	// What the error register should be after a move command and before motion has occurred.
	int oldError;
	// How far to move this iteration
	int increment;
	// How far the motor moved since the last update
	int moved;

	// validate the motor
	if('A'>motor || motor>'H')
	{
		PRINTF("Illegal motor: %c\n", motor);
		return -1;
	}

	// Normalize the direction
	toMove=destination-positions[motor-'A'];
	if(toMove<0)
	{
		sign=-1;
		command=REVERSE;
		toMove=-toMove;
	}
	else
	{
		sign=1;
		command=FORWARD;
	}

	#ifdef RHINO_DEBUG
	fprintf(logfile, "move(%c, %d), position: %d, toMove: %d, sign: %d, command: %c\n",
		motor, destination, positions[motor-'A'], toMove, sign, command);
	#endif

	error=rhino_inquire(motor);
	oldError=error;
	moved=0;
	noMove=0;
	while(destination!=positions[motor-'A'] && noMove<STALL)
	{
		if(error<0)
		{
			#ifdef RHINO_DEBUG
			fprintf(logfile, "Wait; error=%d < 0\n", error);
			#endif

			noMove++; // An invalid value is generally caused by a stall...

			// Read the error register
			error=rhino_inquire(motor);
			continue; // try again
		}

		// move towards the destination
		increment=95-error;

		if(increment>toMove)
		{
			increment=toMove;
		}

		toMove-=increment;

		// send the move instruction
		sprintf_s(buffer,sizeof(buffer), "%c%c%d\r", motor, command, increment);
		if(WriteFile(hcom, &buffer, (DWORD)strlen(buffer), &written, 0))
		{
			if(written<strlen(buffer))
			{
				PRINTF("Timeout before writing data.\n");
				#ifdef RHINO_DEBUG
				fprintf(logfile, "Timeout before writing data.\nTerminating move.\n\n");
				#endif
				return -1;
			}
		}
		else
		{
			PRINTF("Error writing to the Rhino.\n");
			#ifdef RHINO_DEBUG
			fprintf(logfile, "Error writing to the Rhino.\nTerminating move.\n\n");
			#endif
			return -1;
		}

		// update the position
		moved=oldError-error;
		positions[motor-'A']+=sign*moved;
		oldError=error+increment;

		// Stall detection
		if(moved==0)
		{
			noMove++;
		}
		else
		{
			noMove=0;
		}
		#ifdef RHINO_DEBUG
		fprintf(logfile, "            - position: %d, toMove: %d, oldError: %d, error: %d, moved: %d, increment: %d\n",
			positions[motor-'A'], toMove, oldError, error, moved, increment);
		#endif

		// Read the error register
		error=rhino_inquire(motor);
	}

	// Stall correction
	if(noMove>=STALL)
	{
		#ifdef RHINO_DEBUG
		fprintf(logfile, "Clearing a stall.\n");
		#endif

		// back off to clear the stall
		// reverse the direction of motion
		if(sign>0)
		{
			sign=-1;
			command=REVERSE;
		}
		else
		{
			sign=1;
			command=FORWARD;
		}

		// use the last known good error value
		increment=oldError;
		/*
		// Find out how far to go.
		increment=rhino_inquire(motor);
		if(increment<0)
		{
			// use the last known good error value
			increment=oldError;
		}
		*/

		#ifdef RHINO_DEBUG
		fprintf(logfile, "stall reversal increment: %d\n", increment);
		#endif

		// send the move instruction
		sprintf_s(buffer,sizeof(buffer), "%c%c%d\r", motor, command, increment);
		if(WriteFile(hcom, &buffer, (DWORD)strlen(buffer), &written, 0))
		{
			if(written<strlen(buffer))
			{
				PRINTF("Timeout before writing data.\n");
				#ifdef RHINO_DEBUG
				fprintf(logfile, "Timeout before writing data.\nTerminating move.\n\n");
				#endif
				return -1;
			}
		}
		else
		{
			PRINTF("Error writing to the Rhino.\n");
			#ifdef RHINO_DEBUG
			fprintf(logfile, "Error writing to the Rhino.\nTerminating move.\n\n");
			#endif
			return -1;
		}

		// Assume the move succeeds.
		//positions[motor-'A']+=sign*increment;
		// Wait for the move to succeed.
		noMove=0;
		int errors, temp;
		errors=1;
		while(noMove<STALL && error)
		{
			temp=rhino_inquire(motor);
			if(error && error==temp)
			{
				noMove++;
			}
			else
			{
				error=temp;
				noMove=0;
			}

			if(noMove>=STALL)
			{
				#ifdef RHINO_DEBUG
				fprintf(logfile, "Could not clear the stall on motor %c; error=%d.\n", motor, error);
				#endif
				PRINTF("Could not clear the stall on motor %c.\n", motor);
			}
		}
	}

	// return the error - will be 0 if no stall occurs
	return destination-positions[motor-'A'];
}


/**
 * Move all the arm joints at the same time.
 * Parameters: destination position for each joint
 * Return!=0 if an error occurs.
 */
int rhino_mmove(int destB, int destC, int destD, int destE, int destF)
{
	int destination[JOINTS];

	destination[0]=destB;
	destination[1]=destC;
	destination[2]=destD;
	destination[3]=destE;
	destination[4]=destF;

	return rhino_ammove(destination);
}

int rhino_ammove(int destination[])
{
	// Remember that positions[] by 'A' while destination[] is offset by 'B'...

	// The polarity of this move
	int sign[JOINTS];
	// The move command to send to the Rhino
	char command[JOINTS];
	// How many encoder clicks we need to move
	int toMove[JOINTS];
	// Count how many cycles occur without any motion
	int noMove[JOINTS];
	// Flag when the motor has completely stalled
	int stalled;
	// Command buffer
	char buffer[10];
	// How many bytes are actually written to the Rhino
	DWORD written;
	// The latest status of the Rhino error register
	int error[JOINTS];
	// What the error register should be after a move command and before motion has occurred.
	int oldError[JOINTS];
	// How far to move this iteration
	int increment;
	// How far the motor moved since the last update
	int moved[JOINTS];

	int motor;

	// Normalize the direction
	for(motor=0; motor<JOINTS; motor++)
	{
		toMove[motor]=destination[motor]-positions[motor+1];
		if(toMove[motor]<0)
		{
			sign[motor]=-1;
			command[motor]=REVERSE;
			toMove[motor]=-toMove[motor];
		}
		else
		{
			sign[motor]=1;
			command[motor]=FORWARD;
		}
	}

	// Initialize the variables.
	for(motor=0; motor<JOINTS; motor++)
	{
		error[motor]=rhino_inquire((char)(motor+'B'));
		oldError[motor]=error[motor];
		moved[motor]=0;
		noMove[motor]=0;
	}
	stalled=FALSE;

	#ifdef RHINO_DEBUG
	fprintf(logfile, "\nrhino_mmove()\ndestinations - B: %4d, C: %4d, D: %4d, E: %4d, F: %4d\n",
		destination[0], destination[1], destination[2], destination[3], destination[4]);
	#endif

	// Drive the motors to their positions.
	while(!stalled &&
		(   destination['B'-'B']!=positions['B'-'A'] || destination['C'-'B']!=positions['C'-'A']
		 || destination['D'-'B']!=positions['D'-'A'] || destination['E'-'B']!=positions['E'-'A']
		 || destination['F'-'B']!=positions['F'-'A'])
		)
	{
		#ifdef RHINO_DEBUG
		fprintf(logfile, "   positions - B: %4d, C: %4d, D: %4d, E: %4d, F: %4d\n",
			positions['B'-'A'], positions['C'-'A'], positions['D'-'A'], positions['E'-'A'], positions['F'-'A']);
		#endif

		for(motor=0; motor<JOINTS; motor++)
		{
			if(noMove[motor]>STALL)
			{
				// ignore the stall if we are essentially there
				if(error[motor]<10 && abs(destination[motor]-positions[motor+1])<10)
				{
					// move the destination to match the position
					destination[motor]=positions[motor+1];
				}
				else
				{
					#ifdef RHINO_DEBUG
					fprintf(logfile, "Stalling on motor %c.\n", (char)(motor+'B'));
					#endif
					PRINTF("Error: motor %c stalled.\n", (char)(motor+'B'));

					stalled=TRUE;
				}
			}

			if(error[motor]<0)
			{
				#ifdef RHINO_DEBUG
				fprintf(logfile, "Wait; error=%d < 0\n", error[motor]);
				#endif

				noMove[motor]+=6; // An invalid value is generally caused by a stall...

				// Read the error register
				error[motor]=rhino_inquire((char)(motor+'B'));
				continue; // try again
			}

			// move towards the destination
			increment=95-error[motor];

			if(increment>toMove[motor])
			{
				increment=toMove[motor];
			}

			toMove[motor]-=increment;

			// send the move instruction
			sprintf_s(buffer,sizeof(buffer), "%c%c%d\r", (char)(motor+'B'), command[motor], increment);
			if(WriteFile(hcom, &buffer, (DWORD)strlen(buffer), &written, 0))
			{
				if(written<strlen(buffer))
				{
					PRINTF("Timeout before writing data.\n");
					#ifdef RHINO_DEBUG
					fprintf(logfile, "Timeout before writing data.\nTerminating move.\n\n");
					#endif
					return -1;
				}
			}
			else
			{
				PRINTF("Error writing to the Rhino.\n");
				#ifdef RHINO_DEBUG
				fprintf(logfile, "Error writing to the Rhino.\nTerminating move.\n\n");
				#endif
				return -1;
			}

			// update the position
			moved[motor]=oldError[motor]-error[motor];
			positions[motor+1]+=sign[motor]*moved[motor];
			oldError[motor]=error[motor]+increment;

			// Stall detection - catch both no motion and negative motion.
			if(moved[motor]<=0 && destination[motor]!=positions[motor+1])
			{
				#ifdef RHINO_DEBUG
				fprintf(logfile, "noMove: '%c', position: %d, toMove: %d, sign: %d, command: %c, ",
					(char)(motor+'B'), positions[motor], toMove[motor], sign[motor], command[motor]);
				fprintf(logfile, "error: %d, oldError: %d, increment: %d\n",
					error[motor], oldError[motor], increment);
				#endif
				noMove[motor]++;
			}
			else
			{
				noMove[motor]=0;
			}

			// Read the error register
			error[motor]=rhino_inquire((char)(motor+'B'));
		}
	}

	// Clean up any "small epsilons" that weren't accounted for
	if(!stalled)
	{
		for(motor=0; motor<JOINTS; motor++)
		{
			positions[motor+1]+=sign[motor]*error[motor];
		}

		return 0;
	}

	// Stall correction

	// Return the motors to the last pre-stall position.
	if(stalled)
	{
		#ifdef RHINO_DEBUG
		fprintf(logfile, "Clearing a stall.\n");
		#endif
		PRINTF("Clearing the stall.\n");

		for(motor=0; motor<JOINTS; motor++)
		{
			// back off to clear the stall
			// reverse the direction of motion
			if(sign[motor]>0)
			{
				sign[motor]=-1;
				command[motor]=REVERSE;
			}
			else
			{
				sign[motor]=1;
				command[motor]=FORWARD;
			}

			// use the last known good error value
			increment=oldError[motor];

			#ifdef RHINO_DEBUG
			fprintf(logfile, "stall reversal increment: %d\n", increment);
			#endif

			// send the move instruction
			sprintf_s(buffer,sizeof(buffer), "%c%c%d\r", (char)(motor+'B'), command[motor], increment);
			if(WriteFile(hcom, &buffer, (DWORD)strlen(buffer), &written, 0))
			{
				if(written<strlen(buffer))
				{
					PRINTF("Timeout before writing data.\n");
					#ifdef RHINO_DEBUG
					fprintf(logfile, "Timeout before writing data.\nTerminating move.\n\n");
					#endif
					return -1;
				}
			}
			else
			{
				PRINTF("Error writing to the Rhino.\n");
				#ifdef RHINO_DEBUG
				fprintf(logfile, "Error writing to the Rhino.\nTerminating move.\n\n");
				#endif
				return -1;
			}

			// reset the stall counter
			noMove[motor]=0;
		}

		// Wait for the move to succeed.
		stalled=FALSE;
		int errors, temp;
		errors=1;
		while(!stalled && errors)
		{
			errors=0;
			for(motor=0; motor<JOINTS; motor++)
			{
				temp=rhino_inquire((char)(motor+'B'));
				if(temp<0)
				{
					noMove[motor]+=6;
				}
				else if(error[motor] && error[motor]==temp)
				{
					noMove[motor]++;
				}
				else
				{
					error[motor]=temp;
					noMove[motor]=0;
				}

				if(noMove[motor]>STALL)
				{
					#ifdef RHINO_DEBUG
					fprintf(logfile, "Could not clear the stall on motor %c; error=%d.\n", (char)(motor+'B'), error[motor]);
					#endif
					PRINTF("Could not clear the stall on motor %c.\n", (char)(motor+'B'));
					stalled=TRUE;
				}

				errors+=error[motor];
			}
		}
	}

	// return the error - will be 0 if no stall occurs
	return -1*stalled;
}

/**
 * Close the gripper
 * return the width of the object in clicks
 */
int rhino_grip()
{
	rhino_move('A', CLOSE_A); // the "closed" position
	return CLOSE_A-positions[0];
}

/**
 * Open the gripper
 */
void rhino_ungrip()
{
	rhino_move('A', 0);
}

/**
 * Return the encoder position of the desired motor.
 * An invalid motor returns 0.
 */
int rhino_position(char motor)
{
	if('A'>motor || motor>'H')
	{
		PRINTF("Illegal motor: %c\n", motor);
		return 0;
	}

	return positions[motor-'A'];
}

/**
 * Do a software "hard home" routine.
 * Resets the counters to 0.
 * return<0 indicates an error.
 */
int rhino_hardhome()
{
	// Don't drag things around with us.
	rhino_ungrip();

	// The sequence used by the Rhino people
	rhino_home('E');
	rhino_home('D');
	rhino_home('B');
	rhino_home('C');
	rhino_home('F');
	rhino_home('A');

	rhino_init(); // reset the position counters. - don't reset the com line.
	return 0;
}

int rhino_softhome()
{
	int destinations[JOINTS], i;
	for(i=0; i<JOINTS; i++)
	{
		destinations[i]=0;
	}

	rhino_ammove(destinations);

	return 0;
}

/*
 * Run a "hard home" routine for the specified motor only.
 */
int rhino_home(char motor)
{
	// Try moving the specified motor to the soft home position first (to improve switch finding)
	rhino_move(motor, 0);

	int found, delta;
	switch(motor)
	{
		case 'A':
			// Close the gripper until it stalls; then open it 50 clicks.
			rhino_move('A', 200); // The range of travel is only 70 or 80 clicks; this has to stall.
			positions[0]=0;
			rhino_move('A', -CLOSE_A); // open the gripper its default distance
			positions[0]=0;
			break;
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
			// Rotate one way until the switch is found, or a stall occurs.
			found=FALSE;
			delta=20;
			if(rhino_switch(motor)>0)
			{
				found=TRUE;
			}
			while(!found)
			{
				// move strategy
				// go + for a long time or until a stall occurs.
				// then reverse direction.
				if(rhino_move(motor, delta))
				{
					delta=-delta;
				}
				positions[motor-'A']=0; // reset the position tracker

				if(rhino_switch(motor)>0)
				{
					found=TRUE;
				}
			}

			// Center the switch

			// Reset the software position
			positions[motor-'A']=0;
			break;

		// In our setup, G and H don't have position sensors.
		case 'G':
		case 'H':
			break;

		// Must be invalid
		default:
			PRINTF("Invalid motor: %c\n", motor);
			return -1;
	}

	return 0;
}

/**
 * Reads the microswitch for each motor.
 * Returns 1 (TRUE) if it is closed, 0 (FALSE) if it is not.
 * -1 indicates an error occurred.
 */
int rhino_switch(char motor)
{
	if('A'>motor || motor>'H')
	{
		PRINTF("Illegal motor: %c\n", motor);
		return -1;
	}

	int error;
	error=-1; // default to having an error occur.

	char buf[10];
	if(motor<'C') // A and B
	{
		sprintf_s(buf,sizeof(buf), "J\r");
	}
	else // C,D,E,F,G,H
	{
		sprintf_s(buf,sizeof(buf), "I\r");
	}

	// Send the request
	DWORD written=0;
	if(WriteFile(hcom, &buf, 2, &written, 0))
	{
		if(written<2)
		{
			PRINTF("Timeout before writing data.\n");
			return -1;
		}
	}
	else
	{
		PRINTF("Error writing to the Rhino.\n");
		return -1;
	}

	//get the error register for the motor.
	// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/fileio/base/readfile.asp
	DWORD read=0;
	if(ReadFile(hcom, &buf, 1, &read, 0))
	{
		if(read==0)
		{
			PRINTF("Timeout before reading data.\n");
			return -1;
		}
		error=((int)buf[0])-32; // Fix the communications offset
	}
	else
	{
		PRINTF("Error reading the motor position.\n");
		return -1;
	}

	// Decode the switch information
	// An open switch is a '1'.
	if(motor=='A')
	{
		return !(error&0x10);
	}
	else if(motor=='B')
	{
		return !(error&0x20);
	}
	else if(motor=='C')
	{
		return !(error&0x01);
	}
	else if(motor=='D')
	{
		return !(error&0x02);
	}
	else if(motor=='E')
	{
		return !(error&0x04);
	}
	else if(motor=='F')
	{
		return !(error&0x08);
	}
	else if(motor=='G')
	{
		return !(error&0x10);
	}
	else if(motor=='H')
	{
		return !(error&0x20);
	}

	return error;
}


#ifdef RHINO_MAIN
// A simple built-in test of functionality.
int main(int argc, char **argv)
{
	rhino_init();
	rhino_opencomm(1);

//	rhino_move('@', 0);
	rhino_move('F', 200);
	rhino_move('F', -200);
	rhino_move('F', 0);
//	rhino_move('H', 0);
//	rhino_move('I', 0);

	rhino_inquire('A');
//	rhino_inquire('A');
	rhino_closecomm();

	//port=2;
	rhino_opencomm(2);
	rhino_inquire('A');
//	rhino_inquire('A');
	rhino_closecomm();
	return 0;
}
#endif

