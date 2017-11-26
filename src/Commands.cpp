/*
     This file is part of RaspberryPICANSlave
     (C) Riccardo Ventrella 2017
     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Lesser General Public
     License as published by the Free Software Foundation; either
     version 3.0 of the License, or (at your option) any later version.

     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Lesser General Public License for more details.

     You should have received a copy of the GNU Lesser General Public
     License along with this library; if not, write to the Free Software
     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <Commands.h>
#include <Slaves.h>

// Mutex which protects the commands 
static pthread_mutex_t sCmdMutex;


void CommandInit(void)
{
   pthread_mutex_init(&sCmdMutex, NULL);
}


void CommandQuit(void)
{
   pthread_mutex_destroy(&sCmdMutex);
}

// This command takes the payload given from the AJAX request, which sounds like this:
//
// ID + "R" + Idx + "C" + Cmd
//
// ID  	= CANId related to the Slave this command has to be applied
// Idx	= Relay idx to be updated (0-based)
// Cmd	= 1 = Set, 2 = Reset, 3 = Toggle
//
static void CommandRelay(const char *pId, const char *pR, const char *pC)
{
	if(!pId || !pR || !pC)
		return;
	// The Id comes as HEX string
	int Id = strtol(pId, nullptr, 16);
	int Relay = atoi(pR);
	int Cmd = atoi(pC);
	
	byte PayLoad[NUM_RELAYS];
	int  CANCTRLId;
	
	// Apply the command and retrieve the CTRL Id and the PayLoad to be sent by CAN
	if (SlaveUpdateRelay(&CANCTRLId, PayLoad, Id, Relay, Cmd))
	{
		// Send the CTRL message by CAN, updating the slave
		
	}
}

void CommandDispatcher(const char **ppXMLSnapShot, const char Cmd[], const char *pId, const char *pR, const char *pC)
{
   // These mutex could be avoided, since we are using the microwebserver
   // not in multithreaded mode, so each connection is served sequentially by a queue
   // but we leave it here to support the webserver in multithreaded mode
   pthread_mutex_lock(&sCmdMutex);
   
   //printf("%s\n", Cmd);

   // Switch among commands:
   //
   if (!strcmp(Cmd, "Relay"))
	   CommandRelay(pId, pR, pC);

   // Always returns a Slaves snapshot
   GetSlavesXMLSnapShot(ppXMLSnapShot);
   
#ifdef DUMP
   static int Count = 1;
   
    if (Count++ == 1)
	   printf("%s\n", *ppXMLSnapShot);
#endif

   pthread_mutex_unlock(&sCmdMutex);
}